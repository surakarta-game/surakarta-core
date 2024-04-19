#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include "surakarta_agent_base.h"
#include "surakarta_daemon.h"
#include "surakarta_piece.h"
#include "surakarta_utils.h"

class SurakartaAgentInteractive : public SurakartaAgentBase {
   public:
    SurakartaAgentInteractive(std::shared_ptr<SurakartaBoard> board,
                              std::shared_ptr<SurakartaGameInfo> game_info,
                              std::shared_ptr<SurakartaRuleManager> rule_manager)
        : SurakartaAgentBase(board, game_info, rule_manager) {}
    virtual SurakartaMove CalculateMove() override = 0;

    virtual bool IsMyTurn() = 0;
    virtual std::shared_ptr<std::vector<SurakartaPositionWithId>> MyPieces() = 0;
    virtual std::shared_ptr<std::vector<SurakartaPositionWithId>> OpponentPieces() = 0;

    virtual SurakartaPositionWithId SelectedPiece() = 0;
    virtual bool CanSelectPiece(SurakartaPosition position) = 0;
    virtual bool SelectPiece(SurakartaPosition position) = 0;

    virtual SurakartaPositionWithId SelectedDestination() = 0;
    virtual bool CanSelectDestination(SurakartaPosition position) = 0;
    virtual bool SelectDestination(SurakartaPosition position) = 0;

    virtual bool CanCommitMove() = 0;
    virtual bool CommitMove() = 0;

    SurakartaEvent<SurakartaMoveTrace> OnMoveCommitted;
};

class SurakartaAgentInteractiveFactory : public SurakartaDaemon::AgentFactory {
   private:
    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor color [[maybe_unused]]) override;
};
