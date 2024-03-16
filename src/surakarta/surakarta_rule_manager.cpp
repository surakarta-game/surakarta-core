#include "surakarta_rule_manager.h"
#include <iostream>
#include "surakarta_utils.h"

void SurakartaRuleManager::OnUpdateBoard() {
    // TODO:
    // Every time the board and game_info is updated to the next round version, this function will be called.
    // You don't need to implement this function if you don't need it.
    // A more delicate way is to use Qt's signal and slot mechanism, but that's the advanced part.
}

SurakartaIllegalMoveReason SurakartaRuleManager::JudgeMove(const SurakartaMove& move) {
    // Range check:
    if (board_->IsInside(move.from) == false || board_->IsInside(move.to) == false)
        return SurakartaIllegalMoveReason::OUT_OF_BOARD;

    // Some useful values:
    const auto piece_from = (*board_)[move.from.x][move.from.y];
    const auto piece_to = (*board_)[move.to.x][move.to.y];
    const auto color_from = piece_from->GetColor();
    const auto color_to = piece_to->GetColor();
    const auto curr_player = game_info_->current_player_;
    const auto oppo_player = ReverseColor(curr_player);

    // Ensure color is correct:
    if (color_from == PieceColor::NONE)
        return SurakartaIllegalMoveReason::NOT_PIECE;
    if (color_from != curr_player)
        return SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;
    if (color_to != oppo_player && color_to != PieceColor::NONE) {
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
    }

    if (move.from.x == move.to.x && move.from.y == move.to.y)
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
    if (move.player != game_info_->current_player_)
        return SurakartaIllegalMoveReason::NOT_PLAYER_TURN;

    if (piece_to->GetColor() == PieceColor::NONE) {  // Non-capture case:
        // Try to reach move.to from eight directions:
        for (const auto direction : SurakartaDirectionList) {
            const auto next = move.from + direction;
            const auto next_pos = SurakartaPosition(next.first, next.second);
            if (board_->IsInside(next_pos) == false)
                continue;
            if ((*board_)[next_pos.x][next_pos.y]->GetColor() != PieceColor::NONE)
                continue;
            if (next_pos == move.to)
                return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
        }
        // Can't reach move.to from eight directions:
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
    } else {  // Capture case:
        const auto util = SurakartaPieceMoveUtil(board_size_);
        for (const auto start_direction : SurakartaDirectionStraightList) {
            auto curr_position = move.from;
            auto curr_direction = start_direction;
            int passed_corner_cnt = 0;
            while (true) {
                const auto next_pair_opt = util.Next(std::pair(curr_position, curr_direction));  // Try move a step
                if (next_pair_opt.has_value() == false)                                          // At corner
                    break;
                auto [next_position, next_direction] = next_pair_opt.value();
                curr_position = next_position;
                if (next_direction != curr_direction)  // Has gone through a quarter
                    passed_corner_cnt++;
                curr_direction = next_direction;
                if (curr_position == move.to) {  // Reached
                    if (passed_corner_cnt > 0)
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                    else
                        break;  // Must pass a corner before capture
                }
                // Encountered another piece (not self):
                if (curr_position != move.from && (*board_)[curr_position.x][curr_position.y]->GetColor() != PieceColor::NONE)
                    break;
                if (curr_position == move.from && passed_corner_cnt == 4)  // Has gone back
                    break;
            }
        }
        return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
    }
}

std::pair<SurakartaEndReason, SurakartaPlayer> SurakartaRuleManager::JudgeEnd(const SurakartaIllegalMoveReason reason) {
    const auto curr_colour = game_info_->current_player_;
    const auto oppo_colour = ReverseColor(curr_colour);

    if (IsLegalMoveReason(reason) == false)
        return std::pair(SurakartaEndReason::ILLIGAL_MOVE, oppo_colour);

    int curr_remain = 0;
    int oppo_remain = 0;
    for (const auto& column : *board_) {
        for (const auto& piece : column) {
            if (piece->GetColor() == curr_colour)
                curr_remain++;
            if (piece->GetColor() == oppo_colour)
                oppo_remain++;
        }
    }
    if (reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE)
        oppo_remain -= 1;

    int last_capture_round = reason != SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE
                                 ? game_info_->last_captured_round_
                                 : game_info_->num_round_;

    // Current player has already made a move, so we only need to check whether the opponent has any piece or is movable.
    if (oppo_remain == 0)
        return std::pair(SurakartaEndReason::CHECKMATE, curr_colour);

    if (game_info_->num_round_ - last_capture_round >= game_info_->max_no_capture_round_) {
        if (curr_remain > oppo_remain)
            return std::pair(SurakartaEndReason::STALEMATE, curr_colour);
        else if (curr_remain < oppo_remain)
            return std::pair(SurakartaEndReason::STALEMATE, oppo_colour);
        else
            return std::pair(SurakartaEndReason::STALEMATE, SurakartaPlayer::NONE);
    }

    return std::pair(SurakartaEndReason::NONE, PieceColor::NONE);
}

std::unique_ptr<std::vector<SurakartaPosition>> SurakartaRuleManager::GetAllLegalTarget(const SurakartaPosition postion) {
    // TODO:
    // We don't test this function, you don't need to implement this function if you don't need it.
    return std::make_unique<std::vector<SurakartaPosition>>();
}

void SurakartaRuleManager::HelloWorld() {
    std::cout << "Hello World!" << std::endl;
}