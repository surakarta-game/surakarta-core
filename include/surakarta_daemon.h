#pragma once

#include "surakarta_board.h"
#include "surakarta_event.h"
#include "surakarta_game.h"

class SurakartaDaemon {
   public:
    class AgentFactory {
       public:
        virtual ~AgentFactory() = default;

       private:
        friend SurakartaDaemon;
        virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor my_color) = 0;
    };

    enum class ExecuteStatus {
        NOT_STARTED,
        WAITING_FOR_BLACK_AGENT,
        WAITING_FOR_WHITE_AGENT,
        ENDED,
    };

    SurakartaDaemon(
        int board_size = BOARD_SIZE,
        int max_no_capture_round = MAX_NO_CAPTURE_ROUND,
        std::shared_ptr<AgentFactory> black_agent_factory = nullptr,
        std::shared_ptr<AgentFactory> white_agent_factory = nullptr);

    void Execute();

    SurakartaEvent<> OnUpdateBoard;
    std::shared_ptr<SurakartaGameInfo> GameInfo() const { return game_.GetGameInfo(); }
    std::shared_ptr<SurakartaBoard> Board() const { return game_.GetBoard(); }
    std::shared_ptr<SurakartaRuleManager> RuleManager() const { return game_.GetRuleManager(); }
    ExecuteStatus Status() const { return status_; }

   private:
    SurakartaGame game_;
    std::unique_ptr<SurakartaAgentBase> black_agent_;
    std::unique_ptr<SurakartaAgentBase> white_agent_;
    ExecuteStatus status_ = ExecuteStatus::NOT_STARTED;
};
