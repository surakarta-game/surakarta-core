#include "surakarta_agent_interactive.h"
#include <condition_variable>
#include "surakarta_utils.h"

/// @note Instance of this class can only be owned by daemon thread.
class SurakartaAgentInteractive : SurakartaAgentBase {
   public:
    virtual SurakartaMove CalculateMove() override {
        return inner_agent_.CalculateMove();
    }

    bool IsMyTurn() {
        return inner_agent_.IsWaitingForMove();
    }

    PieceColor MyColor() {
        return my_color_;
    }

    std::shared_ptr<std::vector<SurakartaPositionWithId>> MyPieces() {
        return mine_pieces_;
    }

    std::shared_ptr<std::vector<SurakartaPositionWithId>> OpponentPieces() {
        return oppo_pieces_;
    }

    SurakartaPositionWithId SelectedPiece() {
        return selected_piece_;
    }

    bool CanSelectPiece(SurakartaPosition position) {
        if (!IsMyTurn()) {
            return false;
        }
        for (auto& piece : *mine_pieces_) {
            if (piece.x == position.x && piece.y == position.y) {
                return true;
            }
        }
        return false;
    }

    bool SelectPiece(SurakartaPosition position) {
        if (!CanSelectPiece(position)) {
            return false;
        }
        for (auto& piece : *mine_pieces_) {
            if (piece.x == position.x && piece.y == position.y) {
                selected_piece_ = piece;
                return true;
            }
        }
        throw std::runtime_error("piece can be selected, but not found in mine_pieces_");
    }

    SurakartaPositionWithId SelectedDestination() {
        return selected_destination_;
    }

    bool CanSelectDestination(SurakartaPosition position) {
        if (!IsMyTurn()) {
            return false;
        }
        if (selected_piece_.id == -1) {
            return false;
        }
        const auto targets = get_all_legal_target_util_.GetAllLegalTargets(
            *(*daemon_.Board())[selected_piece_.x][selected_piece_.y]);
        for (auto& target : *targets) {
            if (target.x == position.x && target.y == position.y) {
                return true;
            }
        }
        return false;
    }

    bool SelectDestination(SurakartaPosition position) {
        if (!CanSelectDestination(position)) {
            return false;
        }
        for (auto& piece : *oppo_pieces_) {
            if (piece.x == position.x && piece.y == position.y) {
                selected_destination_ = piece;
                return true;
            }
        }
        throw std::runtime_error("destination can be selected, but not found in oppo_pieces_");
    }

    bool CanCommitMove() {
        return selected_piece_.id != -1 && selected_destination_.id != -1 && IsMyTurn();
    }

    bool CommitMove() {
        if (!CanCommitMove()) {
            return false;
        }
        inner_agent_.CommitMove(SurakartaMove(selected_piece_, selected_destination_, my_color_));
        return true;
    }

    SurakartaEvent<SurakartaMoveTrace> OnMoveCommitted;

   private:
    friend class SurakartaAgentInteractiveFactory;

    SurakartaAgentInteractive(SurakartaDaemon& daemon, PieceColor my_color)
        : SurakartaAgentBase(daemon.Board(), daemon.GameInfo(), daemon.RuleManager()),
          inner_agent_(daemon),
          daemon_(daemon),
          my_color_(my_color),
          on_board_update_util_(
              my_color == PieceColor::BLACK ? mine_pieces_ : oppo_pieces_,
              my_color == PieceColor::WHITE ? mine_pieces_ : oppo_pieces_,
              daemon.Board()),
          get_all_legal_target_util_(daemon.Board()) {
        const auto piece_lists = SurakartaInitPositionListsUtil(daemon.Board()).InitPositionList();
        mine_pieces_ = piece_lists.black_list;
        oppo_pieces_ = piece_lists.white_list;
        auto& util = on_board_update_util_;
        daemon_.OnUpdateBoard.AddListener([this, util]() {
            const auto opt_trace = util.UpdateAndGetTrace();
            if (opt_trace.has_value())
                OnMoveCommitted.Invoke(opt_trace.value());
        });
    }

    /// @note This class is thread safe.
    class SurakartaAgentPassive : public SurakartaAgentBase {
       public:
        bool IsWaitingForMove() const { return is_waiting_for_move_; }
        SurakartaMove CalculateMove() {
            if (is_waiting_for_move_)
                throw std::runtime_error("Cannot calculate move while already waiting for one!");
            auto lk = std::unique_lock<std::mutex>(move_mutex_);
            is_waiting_for_move_ = true;
            move_cv_.wait(lk, [this] { return !is_waiting_for_move_; });
            return SurakartaMove(
                SurakartaPosition(move_.from.x, move_.from.y),
                SurakartaPosition(move_.to.x, move_.to.y),
                move_.player);
        }
        void CommitMove(SurakartaMove move) {
            if (!is_waiting_for_move_)
                throw std::runtime_error("Cannot commit move while not waiting for one!");
            {
                auto lk = std::lock_guard<std::mutex>(move_mutex_);
                move_.from.x = move.from.x;
                move_.from.y = move.from.y;
                move_.to.x = move.to.x;
                move_.to.y = move.to.y;
                move_.player = move.player;
                is_waiting_for_move_ = false;
            }
            move_cv_.notify_one();
        }

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

// This class is thread safe, and its instance can be owned both by daemon thread and other threads.
class SurakartaAgentInteractiveFactory : public SurakartaDaemon::AgentFactory {
   private:
    // Instance of this class should be owned by daemon thread.
    class AgentProxy : public SurakartaAgentBase {
       private:
        friend class SurakartaAgentInteractiveFactory;
        SurakartaAgentInteractiveFactory* volatile factory_;
        std::shared_ptr<std::mutex> mutex_;

       public:
        AgentProxy(std::shared_ptr<SurakartaBoard> board,
                   std::shared_ptr<SurakartaGameInfo> game_info,
                   std::shared_ptr<SurakartaRuleManager> rule_manager,
                   SurakartaAgentInteractiveFactory* factory,
                   std::shared_ptr<std::mutex> mutex)
            : SurakartaAgentBase(board, game_info, rule_manager), factory_(factory), mutex_(mutex) {}

        ~AgentProxy() {
            std::lock_guard lk(*mutex_);
            if (factory_ != nullptr) {
                factory_->agent_.reset();
                factory_->proxy_ = nullptr;
            }
        }

        virtual SurakartaMove CalculateMove() override {
            std::lock_guard lk(*mutex_);
            if (factory_ == nullptr) {
                throw std::runtime_error("SurakartaAgentInteractiveFactory that created this agent is already destroyed!");
            }
            if (!factory_->agent_.has_value())
                throw std::runtime_error("Agent does not exist, but its proxy is still alive!");
            return factory_->agent_.value()->CalculateMove();
        }
    };

    friend class SurakartaAgentInteractiveHandler;
    std::shared_ptr<std::mutex> mutex_ = std::make_shared<std::mutex>();
    std::optional<std::unique_ptr<SurakartaAgentInteractive>> agent_;
    AgentProxy* volatile proxy_;

   public:
    SurakartaAgentInteractiveFactory() {}

    ~SurakartaAgentInteractiveFactory() {
        std::lock_guard lk(*mutex_);
        if (proxy_ != nullptr) {
            proxy_->factory_ = nullptr;
        }
    }

    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor my_color) {
        std::lock_guard lk(*mutex_);
        if (agent_.has_value()) {
            throw std::runtime_error("SurakartaAgentInteractiveFactory can only create one agent!");
        }
        auto agent_ptr = new SurakartaAgentInteractive(daemon, my_color);
        agent_ = std::unique_ptr<SurakartaAgentInteractive>(agent_ptr);
        auto proxy_ptr = new AgentProxy(
            agent_.value()->board_,
            agent_.value()->game_info_,
            agent_.value()->rule_manager_,
            this,
            mutex_);
        return std::unique_ptr<AgentProxy>(proxy_ptr);
    }
};

SurakartaAgentInteractiveHandler::SurakartaAgentInteractiveHandler()
    : agent_factory_(std::make_shared<SurakartaAgentInteractiveFactory>()),
      mutex_(agent_factory_->mutex_) {}

SurakartaAgentInteractiveHandler::~SurakartaAgentInteractiveHandler() {}

std::shared_ptr<SurakartaDaemon::AgentFactory> SurakartaAgentInteractiveHandler::GetAgentFactory() {
    return agent_factory_;
}

bool SurakartaAgentInteractiveHandler::IsAgentCreated() {
    std::lock_guard lk(*mutex_);
    return agent_factory_->agent_.has_value();
}

bool SurakartaAgentInteractiveHandler::IsMyTurn() {
    std::lock_guard lk(*mutex_);
    return agent_factory_->agent_.has_value() && agent_factory_->agent_.value()->IsMyTurn();
}

PieceColor SurakartaAgentInteractiveHandler::MyColor() {
    std::lock_guard lk(*mutex_);
    return agent_factory_->agent_.has_value() ? agent_factory_->agent_.value()->MyColor() : PieceColor::UNKNOWN;
}

std::unique_ptr<std::vector<SurakartaPositionWithId>> SurakartaAgentInteractiveHandler::CopyMyPieces() {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return std::unique_ptr<std::vector<SurakartaPositionWithId>>();
    return std::make_unique<std::vector<SurakartaPositionWithId>>(*agent_factory_->agent_.value()->MyPieces());
}

std::unique_ptr<std::vector<SurakartaPositionWithId>> SurakartaAgentInteractiveHandler::CopyOpponentPieces() {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return std::unique_ptr<std::vector<SurakartaPositionWithId>>();
    return std::make_unique<std::vector<SurakartaPositionWithId>>(*agent_factory_->agent_.value()->OpponentPieces());
}

SurakartaPositionWithId SurakartaAgentInteractiveHandler::SelectedPiece() {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return SurakartaPositionWithId();
    return agent_factory_->agent_.value()->SelectedPiece();
}

bool SurakartaAgentInteractiveHandler::CanSelectPiece(SurakartaPosition position) {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CanSelectPiece(position);
}

bool SurakartaAgentInteractiveHandler::SelectPiece(SurakartaPosition position) {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->SelectPiece(position);
}

SurakartaPositionWithId SurakartaAgentInteractiveHandler::SelectedDestination() {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return SurakartaPositionWithId();
    return agent_factory_->agent_.value()->SelectedDestination();
}

bool SurakartaAgentInteractiveHandler::CanSelectDestination(SurakartaPosition position) {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CanSelectDestination(position);
}

bool SurakartaAgentInteractiveHandler::SelectDestination(SurakartaPosition position) {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->SelectDestination(position);
}

bool SurakartaAgentInteractiveHandler::CanCommitMove() {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CanCommitMove();
}

bool SurakartaAgentInteractiveHandler::CommitMove() {
    std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CommitMove();
}
