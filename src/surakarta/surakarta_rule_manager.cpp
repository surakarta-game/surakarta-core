#include "surakarta_rule_manager.h"
#include <iostream>
#include "surakarta_utils.h"

// #define DEBUG_JUDGE_MOVE // Enable this macro to print debug information when judging move.

void SurakartaRuleManager::OnUpdateBoard() {
    // TODO:
    // Every time the board and game_info is updated to the next round version, this function will be called.
    // You don't need to implement this function if you don't need it.
    // A more delicate way is to use Qt's signal and slot mechanism, but that's the advanced part.
}

SurakartaIllegalMoveReason SurakartaRuleManager::JudgeMove(const SurakartaMove& move) {
    if (board_->IsInside(move.from) == false || board_->IsInside(move.to) == false)
        return SurakartaIllegalMoveReason::OUT_OF_BOARD;

    const auto piece_from = (*board_)[move.from.x][move.from.y];
    const auto color_from = piece_from->GetColor();
    const auto curr_player = game_info_->current_player_;
    const auto oppo_player = ReverseColor(curr_player);
    if (color_from == PieceColor::NONE)
        return SurakartaIllegalMoveReason::NOT_PIECE;
    if (color_from != curr_player)
        return SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;

    if (move.from.x == move.to.x && move.from.y == move.to.y)
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;

    const auto piece_to = (*board_)[move.to.x][move.to.y];
    const auto piece_to_color = piece_to->GetColor();
    if (piece_to_color != oppo_player && piece_to_color != PieceColor::NONE) {
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
    }
    if (piece_to->GetColor() == PieceColor::NONE) {  // non-capture
        if (move.from.x == move.to.x) {
            if (move.from.y < move.to.y) {
                for (unsigned int y = move.from.y + 1; y < move.to.y; y++) {
                    if ((*board_)[move.from.x][y]->GetColor() != PieceColor::NONE)
                        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
                }
                return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
            } else if (move.from.y > move.to.y) {
                for (unsigned int y = move.from.y - 1; y > move.to.y; y--) {
                    if ((*board_)[move.from.x][y]->GetColor() != PieceColor::NONE)
                        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
                }
                return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
            } else
                return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
        } else if (move.from.y == move.to.y) {
            if (move.from.x < move.to.x) {
                for (unsigned int x = move.from.x + 1; x < move.to.x; x++) {
                    if ((*board_)[x][move.from.y]->GetColor() != PieceColor::NONE)
                        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
                }
                return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
            } else if (move.from.x > move.to.x) {
                for (unsigned int x = move.from.x - 1; x > move.to.x; x--) {
                    if ((*board_)[x][move.from.y]->GetColor() != PieceColor::NONE)
                        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
                }
                return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
            } else
                return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
        } else {
            return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
        }
    } else {  // capture
        const SurakartaDirection directions[] = {
            SurakartaDirection::UP,
            SurakartaDirection::DOWN,
            SurakartaDirection::LEFT,
            SurakartaDirection::RIGHT,
        };
        for (const auto start_direction : directions) {
            auto curr_position = piece_from->GetPosition();
            auto curr_direction = start_direction;
            auto util = SurakartaPieceMoveUtil(board_size_);
            int passed_corner_cnt = 0;
            while (true) {
#ifdef DEBUG_JUDGE_MOVE
                std::cout << "(" << curr_position << ", " << curr_direction << ")" << std::endl;
#endif
                const auto next_pair_opt = util.Next(curr_position, curr_direction);
                if (next_pair_opt.has_value() == false)
                    break;
                if (next_pair_opt.value().second != curr_direction)
                    passed_corner_cnt++;
                curr_position = next_pair_opt.value().first;
                curr_direction = next_pair_opt.value().second;
                if (curr_position == move.to) {
                    if (passed_corner_cnt > 0) {
#ifdef DEBUG_JUDGE_MOVE
                        std::cout << "RET" << std::endl;
#endif
                        return SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                    } else {
                        break;
                    }
                }
                if (curr_position != move.from && (*board_)[curr_position.x][curr_position.y]->GetColor() != PieceColor::NONE)
                    break;
                if (curr_position == move.from && passed_corner_cnt == 4)
                    break;
            }
        }
#ifdef DEBUG_JUDGE_MOVE
        std::cout << "RET" << std::endl;
#endif
        return SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
    }
}

std::pair<SurakartaEndReason, SurakartaPlayer> SurakartaRuleManager::JudgeEnd(const SurakartaIllegalMoveReason reason) {
    // TODO: Implement this function.
    // Note that at this point, the board and game_info have not been updated yet.

    return std::make_pair(SurakartaEndReason::NONE, SurakartaPlayer::NONE);
}

std::unique_ptr<std::vector<SurakartaPosition>> SurakartaRuleManager::GetAllLegalTarget(const SurakartaPosition postion) {
    // TODO:
    // We don't test this function, you don't need to implement this function if you don't need it.
    return std::make_unique<std::vector<SurakartaPosition>>();
}

void SurakartaRuleManager::HelloWorld() {
    std::cout << "Hello World!" << std::endl;
}