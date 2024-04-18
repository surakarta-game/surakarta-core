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
    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor color [[maybe_unused]]) override {
        return std::make_unique<SurakartaAgentRandom>(
            daemon.Board(),
            daemon.GameInfo(),
            daemon.RuleManager());
    }
};
