#pragma once

#include <memory>
#include "surakarta_agent_base.h"
#include "surakarta_daemon.h"

class SurakartaAgentRandom : public SurakartaAgentBase {
   public:
    using SurakartaAgentBase::SurakartaAgentBase;
    SurakartaMove CalculateMove() override;
};

class SurakartaAgentRandomFactory : public SurakartaDaemon::AgentFactory {
   public:
    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(
        std::shared_ptr<SurakartaGameInfo> game_info,
        std::shared_ptr<SurakartaBoard> board,
        std::shared_ptr<SurakartaRuleManager> rule_manager,
        SurakartaDaemon& daemon [[maybe_unused]],
        PieceColor color [[maybe_unused]]) override {
        return std::make_unique<SurakartaAgentRandom>(
            board,
            game_info,
            rule_manager);
    }
};
