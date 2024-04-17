#pragma once

#include <memory>
#include "surakarta_agent_base.h"
#include "surakarta_daemon.h"

class SurakartaAgentFactory {
   public:
    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(std::shared_ptr<SurakartaDaemon> daemon) = 0;
};
