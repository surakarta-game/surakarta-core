#include <climits>
#include <cstring>
#include <thread>

#include "surakarta.h"

#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_MOVE_TO_START "\033[H"

#define WIN_MIME 1
#define WIN_RANDOM 2
#define STALEMATE 3

int play(int miliseconds = 50,
         bool display = true,
         int depth = SurakartaMoveWeightUtil::DefaultDepth,
         double alpha = SurakartaMoveWeightUtil::DefaultAlpha,
         double beta = SurakartaMoveWeightUtil::DefaultBeta) {
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
        is_mine ? SurakartaPlayer::BLACK : SurakartaPlayer::WHITE,
        std::make_shared<SurakartaMoveWeightUtil>(game.GetBoard(),
                                                  is_mine ? SurakartaPlayer::BLACK : SurakartaPlayer::WHITE,
                                                  depth,
                                                  alpha,
                                                  beta));
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
            std::cout << std::endl;
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
        std::cout << std::endl;
        std::cout << *game.GetBoard();
        std::cout << std::endl;
        std::cout << game.GetGameInfo()->end_reason_ << ": "
                  << (is_stalemate ? "None" : (has_win ? "Mine" : "Random"))
                  << std::endl;
    }
    return is_stalemate ? STALEMATE : (has_win ? WIN_MIME : WIN_RANDOM);
}

int main(int argc, char** argv) {
    int depth = SurakartaMoveWeightUtil::DefaultDepth;
    double alpha = SurakartaMoveWeightUtil::DefaultAlpha;
    double beta = SurakartaMoveWeightUtil::DefaultBeta;
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--depth") == 0 || strcmp(argv[i], "-d") == 0) {
            depth = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--alpha") == 0 || strcmp(argv[i], "-a") == 0) {
            alpha = atof(argv[++i]);
        } else if (strcmp(argv[i], "--beta") == 0 || strcmp(argv[i], "-b") == 0) {
            beta = atof(argv[++i]);
        }
    }
    if (argc > 1 && strcmp(argv[1], "play") == 0) {
        int delay = 500;
        for (int i = 1; i < argc - 1; i++) {
            if (strcmp(argv[i], "--delay") == 0 || strcmp(argv[i], "-D") == 0) {
                delay = atoi(argv[++i]);
            }
        }
        play(delay, true);
    } else if (argc > 1 && strcmp(argv[1], "statistic") == 0) {
        int concurrency = std::thread::hardware_concurrency();
        int total_rounds = INT_MAX;
        for (int i = 1; i < argc - 1; i++) {
            if (strcmp(argv[i], "-j") == 0) {
                concurrency = atoi(argv[++i]);
            } else if (strcmp(argv[i], "-n") == 0) {
                total_rounds = atoi(argv[++i]);
            }
        }

        int cnt_play = 0;
        int cnt_win = 0;
        int cnt_lost = 0;

        std::thread threads[concurrency];
        for (int i = 0; i < concurrency; i++) {
            threads[i] = std::thread([&]() {
                while (true) {
                    const int result = play(0, false, depth, alpha, beta);
                    cnt_play++;
                    if (result == WIN_MIME)
                        cnt_win++;
                    if (result == WIN_RANDOM)
                        cnt_lost++;
                    if (cnt_play <= total_rounds)
                        printf(
                            "Win Rate: %6.2f   Not Lost Rate: %6.2f   Win: %5d Lost: %5d Stalemate: %5d\n",
                            100.0 * cnt_win / cnt_play,
                            100.0 * (cnt_play - cnt_lost) / cnt_play,
                            cnt_win,
                            cnt_lost,
                            cnt_play - cnt_win - cnt_lost);
                    if (cnt_play >= total_rounds)
                        break;
                }
            });
        }
        for (int i = 0; i < concurrency; i++) {
            threads[i].join();
        }
    } else {
        std::cout << "Usage: " << argv[0] << " play [args..] [--delay|-D <delay>]" << std::endl;
        std::cout << "       " << argv[0] << " statistic [args..] [-j <concurrency>] [-n <total_rounds>]" << std::endl;
        std::cout << "Args:" << std::endl;
        std::cout << "  --depth|-d <depth>  The depth of the search tree, default: " << SurakartaMoveWeightUtil::DefaultDepth << std::endl;
        std::cout << "  --alpha|-a <alpha>  The reduce rate for being captured, default: " << SurakartaMoveWeightUtil::DefaultAlpha << std::endl;
        std::cout << "  --beta|-b <beta>    The reduce rate for capturing, default: " << SurakartaMoveWeightUtil::DefaultBeta << std::endl;
    }
    return 0;
}