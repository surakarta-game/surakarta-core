#include <chrono>
#include <iostream>
#include <thread>
#include "surakarta/global_random_generator.h"
#include "surakarta/surakarta_agent/surakarta_agent_mine.h"
#include "surakarta/surakarta_agent/surakarta_agent_random.h"
#include "surakarta/surakarta_common.h"
#include "surakarta/surakarta_game.h"

#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_MOVE_TO_START "\033[H"

#define WIN_MIME 1
#define WIN_RANDOM 2
#define STALEMATE 3

constexpr int total_rounds = 100000000;

int play(int miliseconds = 50, bool display = true) {
    SurakartaGame game;
    game.StartGame();

    if (display)
        std::cout << ANSI_CLEAR_SCREEN << ANSI_MOVE_TO_START;
    const bool is_mine_at_start = GlobalRandomGenerator().getInstance()() % 2;
    bool is_mine = is_mine_at_start;
    const std::shared_ptr<SurakartaAgentBase> agent_mine = std::make_shared<SurakartaAgentMine>(
        game.GetBoard(),
        game.GetGameInfo(),
        game.GetRuleManager(),
        is_mine ? SurakartaPlayer::BLACK : SurakartaPlayer::WHITE);
    const std::shared_ptr<SurakartaAgentBase> agent_random = std::make_shared<SurakartaAgentRandom>(
        game.GetBoard(),
        game.GetGameInfo(),
        game.GetRuleManager());
    while (!game.IsEnd()) {
        auto move = (is_mine ? agent_mine : agent_random)->CalculateMove();
        game.Move(move);
        is_mine = !is_mine;
        if (display) {
            std::cout << "B: " << (is_mine_at_start ? "Mine" : "Random") << std::endl;
            std::cout << "W: " << (is_mine_at_start ? "Random" : "Mine") << std::endl;
            std::cout << *game.GetBoard() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
            std::cout << ANSI_CLEAR_SCREEN << ANSI_MOVE_TO_START;
        }
    }
    const bool is_stalemate = game.GetGameInfo()->Winner() == SurakartaPlayer::NONE;
    const bool has_win = !((game.GetGameInfo()->Winner() == SurakartaPlayer::BLACK) ^ is_mine_at_start);
    if (display) {
        std::cout << "B: " << (is_mine_at_start ? "Mine" : "Random") << std::endl;
        std::cout << "W: " << (is_mine_at_start ? "Random" : "Mine") << std::endl;
        std::cout << *game.GetBoard();
        std::cout << game.GetGameInfo()->end_reason_ << std::endl;
        std::cout << (is_stalemate ? "Stalemate" : (has_win ? "Mine" : "Random"))
                  << std::endl;
    }
    return is_stalemate ? STALEMATE : (has_win ? WIN_MIME : WIN_RANDOM);
}

int main() {
    // play(0, true);

    int cnt_play = 0;
    int cnt_win = 0;
    int cnt_lost = 0;
    for (int i = 0; i < total_rounds; i++) {
        printf(
            "Win Rate: %5.2f   Not Lost Rate: %5.2f   Win: %5d Lost: %5d Stalemate: %5d\n",
            100.0 * cnt_win / cnt_play,
            100.0 * (cnt_play - cnt_lost) / cnt_play,
            cnt_win,
            cnt_lost,
            cnt_play - cnt_win - cnt_lost);
        const int result = play(0, false);
        cnt_play++;
        if (result == WIN_MIME)
            cnt_win++;
        if (result == WIN_RANDOM)
            cnt_lost++;
    }
    return 0;
}