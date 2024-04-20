#pragma once

#include <memory>
#include "surakarta_common.h"
#include "surakarta_rule_manager.h"

class SurakartaAgentBase {
   public:
    SurakartaAgentBase(std::shared_ptr<SurakartaBoard> board,
                       std::shared_ptr<SurakartaGameInfo> game_info,
                       std::shared_ptr<SurakartaRuleManager> rule_manager)
        : board_size_(board->board_size_),
          board_(board),
          game_info_(game_info),
          rule_manager_(rule_manager) {}

    virtual ~SurakartaAgentBase() = default;

    virtual SurakartaMove CalculateMove() = 0;

   protected:
    int board_size_;
    std::shared_ptr<SurakartaBoard> board_;
    std::shared_ptr<SurakartaGameInfo> game_info_;
    std::shared_ptr<SurakartaRuleManager> rule_manager_;
};
