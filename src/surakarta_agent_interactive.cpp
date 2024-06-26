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

    SurakartaGameInfo CopyGameInfo() {
        return daemon_.CopyGameInfo();
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
                destination_selected_ = false;
                return true;
            }
        }
        throw std::runtime_error("piece can be selected, but not found in mine_pieces_");
    }

    std::optional<SurakartaPosition> SelectedDestination() {
        if (destination_selected_)
            return selected_destination_;
        else
            return std::nullopt;
    }

    bool CanSelectDestination(SurakartaPosition position) {
        if (!IsMyTurn()) {
            return false;
        }
        if (selected_piece_.id == -1) {
            return false;
        }
        const auto targets = get_all_legal_target_util_.GetAllLegalTargets(
            *(*board_)[selected_piece_.x][selected_piece_.y]);
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
        destination_selected_ = true;
        selected_destination_ = position;
        return true;
    }

    bool CanCommitMove() {
        return selected_piece_.id != -1 && destination_selected_ && IsMyTurn();
    }

    bool CommitMove() {
        if (!CanCommitMove()) {
            return false;
        }
        inner_agent_.CommitMove(SurakartaMove(selected_piece_, selected_destination_, my_color_));
        selected_piece_ = SurakartaPositionWithId(0, 0, -1);
        destination_selected_ = false;
        return true;
    }

    void CommitMoveRaw(SurakartaMove move) {
        inner_agent_.CommitMove(move);
    }

    SurakartaEvent<SurakartaMoveTrace> OnMoveCommitted;
    SurakartaEvent<> OnWaitingForMove;
    SurakartaEvent<SurakartaMoveResponse> OnGameEnded;

   private:
    friend class SurakartaAgentInteractiveFactory;

    SurakartaAgentInteractive(
        std::shared_ptr<SurakartaGameInfo> game_info,
        std::shared_ptr<SurakartaBoard> board,
        std::shared_ptr<SurakartaRuleManager> rule_manager,
        SurakartaDaemon& daemon,
        PieceColor my_color)
        : SurakartaAgentBase(board, game_info, rule_manager),
          inner_agent_(game_info, board, rule_manager),
          daemon_(daemon),
          my_color_(my_color),
          get_all_legal_target_util_(board) {
        const auto piece_lists = SurakartaInitPositionListsUtil(board).InitPositionList();
        if (my_color != PieceColor::BLACK && my_color != PieceColor::WHITE)
            throw std::runtime_error("Invalid my_color in SurakartaAgentInteractive!");
        mine_pieces_ = my_color == PieceColor::BLACK ? piece_lists.black_list : piece_lists.white_list;
        oppo_pieces_ = my_color == PieceColor::BLACK ? piece_lists.white_list : piece_lists.black_list;
        on_board_update_util_ = std::make_unique<SurakartaOnBoardUpdateUtil>(
            piece_lists.black_list,
            piece_lists.white_list,
            board);
        auto& util = *on_board_update_util_;
        daemon_.OnUpdateBoard.AddListener([this, util]() {
            const auto opt_trace = util.UpdateAndGetTrace();
            if (opt_trace.has_value())
                OnMoveCommitted.Invoke(opt_trace.value());
        });
        inner_agent_.OnWaitingForMove.AddListener([this]() {
            OnWaitingForMove.Invoke();
        });
        daemon.OnGameEnded.AddListener([this](SurakartaMoveResponse response) {
            OnGameEnded.Invoke(response);
        });
    }

    /// @note This class is thread safe.
    class SurakartaAgentPassive : public SurakartaAgentBase {
       public:
        bool IsWaitingForMove() const {
            std::lock_guard lk(move_mutex_);
            return is_waiting_for_move_;
        }
        SurakartaMove CalculateMove() {
            auto lk = std::unique_lock<std::mutex>(move_mutex_);
            if (is_waiting_for_move_)
                throw std::runtime_error("Cannot calculate move while already waiting for one!");
            is_waiting_for_move_ = true;
            OnWaitingForMove.Invoke();
            move_cv_.wait(lk, [this] { return !is_waiting_for_move_; });
            return SurakartaMove(
                SurakartaPosition(move_.from.x, move_.from.y),
                SurakartaPosition(move_.to.x, move_.to.y),
                move_.player);
        }
        void CommitMove(SurakartaMove move) {
            auto lk = std::lock_guard<std::mutex>(move_mutex_);
            if (!is_waiting_for_move_)
                throw std::runtime_error("Cannot commit move while not waiting for one!");
            move_.from.x = move.from.x;
            move_.from.y = move.from.y;
            move_.to.x = move.to.x;
            move_.to.y = move.to.y;
            move_.player = move.player;
            is_waiting_for_move_ = false;
            move_cv_.notify_one();
        }

        /// @brief This event will be invoked when the agent begins to wait for move.
        SurakartaEvent<> OnWaitingForMove;

       private:
        friend class SurakartaAgentInteractive;
        SurakartaAgentPassive(
            std::shared_ptr<SurakartaGameInfo> game_info,
            std::shared_ptr<SurakartaBoard> board,
            std::shared_ptr<SurakartaRuleManager> rule_manager)
            : SurakartaAgentBase(board, game_info, rule_manager) {}
        bool is_waiting_for_move_ = false;
        SurakartaMove move_;
        mutable std::mutex move_mutex_;
        std::condition_variable move_cv_;
    };

    SurakartaAgentPassive inner_agent_;
    SurakartaDaemon& daemon_;
    PieceColor my_color_;
    std::shared_ptr<std::vector<SurakartaPositionWithId>> mine_pieces_;
    std::shared_ptr<std::vector<SurakartaPositionWithId>> oppo_pieces_;
    std::unique_ptr<SurakartaOnBoardUpdateUtil> on_board_update_util_;
    SurakartaGetAllLegalTargetUtil get_all_legal_target_util_;
    SurakartaPositionWithId selected_piece_ = SurakartaPositionWithId(0, 0, -1);
    bool destination_selected_ = false;
    SurakartaPosition selected_destination_ = SurakartaPosition(0, 0);
};

// This class is thread safe, and its instance can be owned both by daemon thread and other threads.
class SurakartaAgentInteractiveFactory : public SurakartaDaemon::AgentFactory {
   private:
    // Instance of this class should be owned by daemon thread.
    class AgentProxy : public SurakartaAgentBase {
       private:
        friend class SurakartaAgentInteractiveFactory;
        SurakartaAgentInteractiveFactory* factory_;
        // std::shared_ptr<std::mutex> mutex_;

       public:
        AgentProxy(std::shared_ptr<SurakartaBoard> board,
                   std::shared_ptr<SurakartaGameInfo> game_info,
                   std::shared_ptr<SurakartaRuleManager> rule_manager,
                   SurakartaAgentInteractiveFactory* factory)
            : SurakartaAgentBase(board, game_info, rule_manager), factory_(factory) {}

        ~AgentProxy() {
            if (factory_ != nullptr) {
                // factory_->agent_.reset();
                factory_->proxy_ = nullptr;
            }
        }

        virtual SurakartaMove CalculateMove() override {
            if (factory_ == nullptr) {
                throw std::runtime_error("SurakartaAgentInteractiveFactory that created this agent is already destroyed!");
            }
            if (!factory_->agent_.has_value())
                throw std::runtime_error("Agent does not exist, but its proxy is still alive!");
            return factory_->agent_.value()->CalculateMove();
        }
    };

    friend class SurakartaAgentInteractiveHandler;
    // std::shared_ptr<std::mutex> mutex_ = std::make_shared<std::mutex>();
    std::optional<std::unique_ptr<SurakartaAgentInteractive>> agent_;
    AgentProxy* volatile proxy_;
    SurakartaAgentInteractiveHandler* handler_;
    mutable std::mutex agent_creation_blocker_;

   public:
    SurakartaAgentInteractiveFactory(SurakartaAgentInteractiveHandler* handler)
        : handler_(handler) {}

    ~SurakartaAgentInteractiveFactory() {
        if (proxy_ != nullptr) {
            proxy_->factory_ = nullptr;
        }
    }

    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(
        std::shared_ptr<SurakartaGameInfo> game_info,
        std::shared_ptr<SurakartaBoard> board,
        std::shared_ptr<SurakartaRuleManager> rule_manager,
        SurakartaDaemon& daemon [[maybe_unused]],
        PieceColor my_color) override {
        // //std::lock_guard lk(*mutex_);
        std::lock_guard lk(agent_creation_blocker_);
        if (agent_.has_value()) {
            throw std::runtime_error("SurakartaAgentInteractiveFactory can only create one agent!");
        }
        auto agent_ptr = new SurakartaAgentInteractive(game_info, board, rule_manager, daemon, my_color);
        agent_ = std::unique_ptr<SurakartaAgentInteractive>(agent_ptr);
        auto proxy_ptr = new AgentProxy(
            agent_.value()->board_,
            agent_.value()->game_info_,
            agent_.value()->rule_manager_,
            this);
        if (handler_) {
            agent_.value()->OnMoveCommitted.AddListener([this](SurakartaMoveTrace trace) {
                handler_->OnMoveCommitted.Invoke(trace);
            });
            agent_.value()->OnWaitingForMove.AddListener([this]() {
                handler_->OnWaitingForMove.Invoke();
            });
            agent_.value()->OnGameEnded.AddListener([this](SurakartaMoveResponse response) {
                handler_->OnGameEnded.Invoke(response);
            });
            handler_->OnAgentCreated.Invoke();
        } else
            printf("Warning: SurakartaAgentInteractiveHandler has been destroyed, so OnMoveCommitted will not be invoked.\n");
        return std::unique_ptr<AgentProxy>(proxy_ptr);
    }

    void BlockAgentCreation() const {
        agent_creation_blocker_.lock();
    }

    void UnblockAgentCreation() const {
        auto _ [[maybe_unused]] = agent_creation_blocker_.try_lock();  // avoid unlocking an unlocked mutex
        agent_creation_blocker_.unlock();
    }
};

SurakartaAgentInteractiveHandler::SurakartaAgentInteractiveHandler()
    : agent_factory_(std::make_shared<SurakartaAgentInteractiveFactory>(this)) {}

SurakartaAgentInteractiveHandler::~SurakartaAgentInteractiveHandler() {
    agent_factory_->handler_ = nullptr;
}

std::shared_ptr<SurakartaDaemon::AgentFactory> SurakartaAgentInteractiveHandler::GetAgentFactory() {
    return agent_factory_;
}

bool SurakartaAgentInteractiveHandler::IsAgentCreated() {
    // std::lock_guard lk(*mutex_);
    return agent_factory_->agent_.has_value();
}

void SurakartaAgentInteractiveHandler::BlockAgentCreation() {
    agent_factory_->BlockAgentCreation();
}

void SurakartaAgentInteractiveHandler::UnblockAgentCreation() {
    agent_factory_->UnblockAgentCreation();
}

bool SurakartaAgentInteractiveHandler::IsMyTurn() {
    // std::lock_guard lk(*mutex_);
    return agent_factory_->agent_.has_value() && agent_factory_->agent_.value()->IsMyTurn();
}

PieceColor SurakartaAgentInteractiveHandler::MyColor() {
    // std::lock_guard lk(*mutex_);
    return agent_factory_->agent_.has_value() ? agent_factory_->agent_.value()->MyColor() : PieceColor::UNKNOWN;
}

std::unique_ptr<std::vector<SurakartaPositionWithId>> SurakartaAgentInteractiveHandler::CopyMyPieces() {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return std::make_unique<std::vector<SurakartaPositionWithId>>();
    return std::make_unique<std::vector<SurakartaPositionWithId>>(*agent_factory_->agent_.value()->MyPieces());
}

std::unique_ptr<std::vector<SurakartaPositionWithId>> SurakartaAgentInteractiveHandler::CopyOpponentPieces() {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return std::make_unique<std::vector<SurakartaPositionWithId>>();
    return std::make_unique<std::vector<SurakartaPositionWithId>>(*agent_factory_->agent_.value()->OpponentPieces());
}

SurakartaPositionWithId SurakartaAgentInteractiveHandler::SelectedPiece() {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return SurakartaPositionWithId();
    return agent_factory_->agent_.value()->SelectedPiece();
}

bool SurakartaAgentInteractiveHandler::CanSelectPiece(SurakartaPosition position) {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CanSelectPiece(position);
}

bool SurakartaAgentInteractiveHandler::SelectPiece(SurakartaPosition position) {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->SelectPiece(position);
}

std::optional<SurakartaPosition> SurakartaAgentInteractiveHandler::SelectedDestination() {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return SurakartaPositionWithId();
    return agent_factory_->agent_.value()->SelectedDestination();
}

bool SurakartaAgentInteractiveHandler::CanSelectDestination(SurakartaPosition position) {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CanSelectDestination(position);
}

bool SurakartaAgentInteractiveHandler::SelectDestination(SurakartaPosition position) {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->SelectDestination(position);
}

bool SurakartaAgentInteractiveHandler::CanCommitMove() {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CanCommitMove();
}

bool SurakartaAgentInteractiveHandler::CommitMove() {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return false;
    return agent_factory_->agent_.value()->CommitMove();
}

void SurakartaAgentInteractiveHandler::CommitMoveRaw(SurakartaMove move) {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return;
    agent_factory_->agent_.value()->CommitMoveRaw(move);
}

SurakartaGameInfo SurakartaAgentInteractiveHandler::CopyGameInfo() {
    // std::lock_guard lk(*mutex_);
    if (!agent_factory_->agent_.has_value())
        return SurakartaGameInfo();
    return agent_factory_->agent_.value()->CopyGameInfo();
}
