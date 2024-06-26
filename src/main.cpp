#include <climits>
#include <cstring>
#include <thread>

#include "surakarta.h"

#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_MOVE_TO_START "\033[H"

#define WIN_MIME 1
#define WIN_RANDOM 2
#define STALEMATE 3

class ExposiveSurakartaDaemon : public SurakartaDaemon {
   public:
    ExposiveSurakartaDaemon(
        int board_size,
        int max_no_capture_round,
        std::shared_ptr<AgentFactory> black_agent_factory,
        std::shared_ptr<AgentFactory> white_agent_factory)
        : SurakartaDaemon(board_size, max_no_capture_round, black_agent_factory, white_agent_factory) {}

    std::shared_ptr<SurakartaGameInfo> GameInfo() {
        return game_.GetGameInfo();
    }

    std::shared_ptr<SurakartaBoard> Board() {
        return game_.GetBoard();
    }
};

int play(int miliseconds = 50,
         bool display = true,
         int depth = SurakartaMoveWeightUtil::DefaultDepth,
         double alpha = SurakartaMoveWeightUtil::DefaultAlpha,
         double beta = SurakartaMoveWeightUtil::DefaultBeta) {
    const auto move_weight_util_factory = std::make_shared<SurakartaAgentMineFactory::SurakartaMoveWeightUtilFactory>(depth, alpha, beta);
    const auto agent_factory_mine = std::make_shared<SurakartaAgentMineFactory>(move_weight_util_factory);
    const auto agent_factory_random = std::make_shared<SurakartaAgentRandomFactory>();
    const auto my_colour = GlobalRandomGenerator().getInstance()() % 2 ? PieceColor::BLACK : PieceColor::WHITE;
    const auto agent_factory_black = my_colour == PieceColor::BLACK ? (std::shared_ptr<SurakartaDaemon::AgentFactory>)agent_factory_mine : agent_factory_random;
    const auto agent_factory_white = my_colour == PieceColor::WHITE ? (std::shared_ptr<SurakartaDaemon::AgentFactory>)agent_factory_mine : agent_factory_random;
    auto daemon = ExposiveSurakartaDaemon(BOARD_SIZE, MAX_NO_CAPTURE_ROUND, agent_factory_black, agent_factory_white);

    const auto black_pieces = std::make_shared<std::vector<SurakartaPositionWithId>>();
    const auto white_pieces = std::make_shared<std::vector<SurakartaPositionWithId>>();
    bool piece_lists_initialized = false;
    SurakartaOnBoardUpdateUtil on_board_update_util(black_pieces, white_pieces, daemon.Board());

    daemon.OnUpdateBoard.AddListener([&]() {
        if (!piece_lists_initialized) {
            const auto lists = SurakartaInitPositionListsUtil(daemon.Board()).InitPositionList();
            *black_pieces = *lists.black_list;
            *white_pieces = *lists.white_list;
            piece_lists_initialized = true;
        }
        const auto opt_trace = on_board_update_util.UpdateAndGetTrace();
        if (opt_trace.has_value()) {
            PieceColor moved_colour = PieceColor::NONE;
            for (auto& item : *black_pieces) {
                if (item.id == opt_trace->moved_piece.id)
                    moved_colour = PieceColor::BLACK;
            }
            for (auto& item : *white_pieces) {
                if (item.id == opt_trace->moved_piece.id)
                    moved_colour = PieceColor::WHITE;
            }
            if (moved_colour == PieceColor::NONE)
                throw std::runtime_error("moved piece not found in black_pieces or white_pieces");
            const auto guard = opt_trace.value().is_capture ? SurakartaTemporarilyChangeColorGuardUtil(
                                                                  daemon.Board(),
                                                                  SurakartaPosition(
                                                                      opt_trace.value().captured_piece.x,
                                                                      opt_trace.value().captured_piece.y),
                                                                  ReverseColor(moved_colour))
                                                            : SurakartaTemporarilyChangeColorGuardUtil();
            for (auto& fragment : opt_trace.value().path) {
                SurakartaTemporarilyChangeColorGuardUtil guard1(daemon.Board(), fragment.From(), PieceColor::NONE);
                SurakartaTemporarilyChangeColorGuardUtil guard2(daemon.Board(), fragment.To(), moved_colour);
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    fragment.is_curve ? miliseconds * 2 : 0));

                if (display) {
                    std::cout << ANSI_CLEAR_SCREEN << ANSI_MOVE_TO_START;
                    std::cout << "B: " << (my_colour == PieceColor::BLACK ? "Mine" : "Random") << std::endl;
                    std::cout << "W: " << (my_colour == PieceColor::WHITE ? "Mine" : "Random") << std::endl;
                    std::cout << std::endl;
                    std::cout << *daemon.Board() << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
            }
        }
    });

    daemon.Execute();

    const bool is_stalemate = daemon.GameInfo()->Winner() == SurakartaPlayer::NONE;
    const bool has_win = !((daemon.GameInfo()->Winner() == SurakartaPlayer::BLACK) ^ (my_colour == PieceColor::BLACK));
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
        play(delay, true, depth, alpha, beta);
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

        const auto threads = std::make_unique<std::thread[]>(concurrency);
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