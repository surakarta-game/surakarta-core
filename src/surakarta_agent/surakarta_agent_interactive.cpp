#include "surakarta_agent_interactive.h"

SurakartaAgentInteractive::SurakartaAgentInteractive(SurakartaDaemon& daemon, PieceColor my_color)
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

bool SurakartaAgentInteractive::IsMyTurn() {
    return inner_agent_.IsWaitingForMove();
}

bool SurakartaAgentInteractive::CanSelectPiece(SurakartaPosition position) {
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

bool SurakartaAgentInteractive::SelectPiece(SurakartaPosition position) {
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

bool SurakartaAgentInteractive::CanSelectDestination(SurakartaPosition position) {
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

bool SurakartaAgentInteractive::SelectDestination(SurakartaPosition position) {
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

bool SurakartaAgentInteractive::CanCommitMove() {
    return selected_piece_.id != -1 && selected_destination_.id != -1 && IsMyTurn();
}

bool SurakartaAgentInteractive::CommitMove() {
    if (!CanCommitMove()) {
        return false;
    }
    inner_agent_.CommitMove(SurakartaMove(selected_piece_, selected_destination_, my_color_));
    return true;
}

SurakartaMove SurakartaAgentInteractive::SurakartaAgentPassive::CalculateMove() {
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

void SurakartaAgentInteractive::SurakartaAgentPassive::CommitMove(SurakartaMove move) {
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
