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
    virtual SurakartaMove CalculateMove() override { return inner_agent_.CalculateMove(); }

    bool IsMyTurn();
    std::shared_ptr<std::vector<SurakartaPositionWithId>> MyPieces() { return mine_pieces_; }
    std::shared_ptr<std::vector<SurakartaPositionWithId>> OpponentPieces() { return oppo_pieces_; }

    SurakartaPositionWithId SelectedPiece() { return selected_piece_; }
    bool CanSelectPiece(SurakartaPosition position);
    bool SelectPiece(SurakartaPosition position);

    SurakartaPositionWithId SelectedDestination() { return selected_destination_; }
    bool CanSelectDestination(SurakartaPosition position);
    bool SelectDestination(SurakartaPosition position);

    bool CanCommitMove();
    bool CommitMove();

    SurakartaEvent<SurakartaMoveTrace> OnMoveCommitted;

   private:
    friend class SurakartaAgentInteractiveFactory;
    SurakartaAgentInteractive(SurakartaDaemon& daemon, PieceColor my_color);

    class SurakartaAgentPassive : public SurakartaAgentBase {
       public:
        bool IsWaitingForMove() const { return is_waiting_for_move_; }
        virtual SurakartaMove CalculateMove() override;
        void CommitMove(SurakartaMove move);

       private:
        friend class SurakartaAgentInteractive;
        SurakartaAgentPassive(SurakartaDaemon& daemon)
            : SurakartaAgentBase(daemon.Board(), daemon.GameInfo(), daemon.RuleManager()) {}
        volatile bool is_waiting_for_move_ = false;
        volatile SurakartaMove move_;
        std::mutex move_mutex_;
        std::condition_variable move_cv_;
    };

    SurakartaAgentPassive inner_agent_;
    SurakartaDaemon& daemon_;
    PieceColor my_color_;
    std::shared_ptr<std::vector<SurakartaPositionWithId>> mine_pieces_;
    std::shared_ptr<std::vector<SurakartaPositionWithId>> oppo_pieces_;
    SurakartaOnBoardUpdateUtil on_board_update_util_;
    SurakartaGetAllLegalTargetUtil get_all_legal_target_util_;
    SurakartaPositionWithId selected_piece_ = SurakartaPositionWithId(0, 0, -1);
    SurakartaPositionWithId selected_destination_ = SurakartaPositionWithId(0, 0, -1);
};

class SurakartaAgentInteractiveFactory : public SurakartaDaemon::AgentFactory {
   private:
    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor color [[maybe_unused]]) override {
        const SurakartaAgentInteractive* agent = new SurakartaAgentInteractive(daemon, color);
        return std::unique_ptr<SurakartaAgentBase>(const_cast<SurakartaAgentInteractive*>(agent));
    }
};
