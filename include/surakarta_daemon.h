#pragma once

#include "surakarta_agent_factory.h"
#include "surakarta_board.h"
#include "surakarta_game.h"

class SurakartaDaemon {
   public:
    SurakartaDaemon(
        int board_size = BOARD_SIZE,
        int max_no_capture_round = 40,
        std::shared_ptr<SurakartaAgentFactory> black_agent_factory = nullptr,
        std::shared_ptr<SurakartaAgentFactory> white_agent_factory = nullptr);

    void BeginLoop();
    SurakartaGameInfo GameInfo() const;
    std::shared_ptr<const SurakartaBoard> Board() const;

   private:
    std::shared_ptr<SurakartaGame> game_;
    std::shared_ptr<SurakartaBoard> board_;
};
