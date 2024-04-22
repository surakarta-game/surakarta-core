#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include "surakarta_agent_base.h"
#include "surakarta_daemon.h"
#include "surakarta_piece.h"
#include "surakarta_utils.h"

class SurakartaAgentInteractiveFactory;

/// @note This class is thread-safe.
class SurakartaAgentInteractiveHandler {
   public:
    SurakartaAgentInteractiveHandler();
    ~SurakartaAgentInteractiveHandler();

    /// @note The factory you get is thread-safe.
    std::shared_ptr<SurakartaDaemon::AgentFactory> GetAgentFactory();
    bool IsAgentCreated();

    /// @brief Agent creation will not happen until UnblockAgentCreation is called.
    void BlockAgentCreation();
    /// @brief Eliminate the effect of BlockAgentCreation.
    void UnblockAgentCreation();

    // If the agent is not created, the following functions will return false or default values.

    bool IsMyTurn();
    PieceColor MyColor();

    // id starts from 0, self-increasing, from black to white.
    std::unique_ptr<std::vector<SurakartaPositionWithId>> CopyMyPieces();
    std::unique_ptr<std::vector<SurakartaPositionWithId>> CopyOpponentPieces();

    SurakartaPositionWithId SelectedPiece();
    bool CanSelectPiece(SurakartaPosition position);
    bool SelectPiece(SurakartaPosition position);

    std::optional<SurakartaPosition> SelectedDestination();
    bool CanSelectDestination(SurakartaPosition position);
    bool SelectDestination(SurakartaPosition position);

    bool CanCommitMove();
    bool CommitMove();

    SurakartaEvent<> OnAgentCreated;
    SurakartaEvent<> OnWaitingForMove;
    SurakartaEvent<SurakartaMoveTrace> OnMoveCommitted;

   private:
    std::shared_ptr<SurakartaAgentInteractiveFactory> agent_factory_;
    // std::shared_ptr<std::mutex> mutex_;
};
