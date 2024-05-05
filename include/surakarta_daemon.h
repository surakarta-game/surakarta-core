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
        virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(
            std::shared_ptr<SurakartaGameInfo> game_info,
            std::shared_ptr<SurakartaBoard> board,
            std::shared_ptr<SurakartaRuleManager> rule_manager,
            SurakartaDaemon& daemon,
            PieceColor my_color) = 0;
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

    /// @brief This method will create a temporary game agent, using a copy of the current game state.
    std::unique_ptr<SurakartaAgentBase> CreateTemporaryGameAgent(AgentFactory& factory, PieceColor my_color);

    SurakartaEvent<> OnUpdateBoard;
    ExecuteStatus Status() const { return status_; }

   protected:
    SurakartaGame game_;
    const std::shared_ptr<AgentFactory> black_agent_factory_;
    const std::shared_ptr<AgentFactory> white_agent_factory_;
    ExecuteStatus status_ = ExecuteStatus::NOT_STARTED;
};
