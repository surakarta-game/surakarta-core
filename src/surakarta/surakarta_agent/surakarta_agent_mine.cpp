#include "surakarta/surakarta_agent/surakarta_agent_mine.h"
#include "surakarta_agent_mine.h"

#include <algorithm>
#include <valarray>

double SurakartaMoveWeightUtil::CalculateMoveWeight(SurakartaMove move, unsigned int depth) {
    if (move.player == curr_colour_) {
        const auto piece_to = (*board_)[move.to.x][move.to.y];
        const bool capture = piece_to->GetColor() == oppo_colour_;
        double weight = 0;
        if (capture)
            weight += 1;
        if (depth > 1) {
            apply_move_util_.ApplyMove(move);
            const auto moves = get_all_legal_moves_util_.GetAllLegalMoves(oppo_colour_);

            // // Cutting:
            // bool is_capture_within = false;
            // for (const auto move : *moves) {
            //     if ((*board_)[move.to.x][move.to.y]->GetColor() == curr_colour_) {
            //         is_capture_within = true;
            //         break;
            //     }
            // }
            // if (is_capture_within) {
            //     const auto itor_new_end = std::remove_if(moves->begin(), moves->end(), [this](SurakartaMove move) {
            //         return (*board_)[move.to.x][move.to.y]->GetColor() != curr_colour_;
            //     });
            //     moves->erase(itor_new_end, moves->end());
            // }

            // See: https://stackoverflow.com/questions/51074327/any-similar-function-like-js-array-prototype-map-in-c
            auto move_weights = std::valarray<double>(moves->size());
            std::transform(moves->begin(), moves->end(), std::begin(move_weights), [this, depth](SurakartaMove move) {
                return CalculateMoveWeight(move, depth - 1);
            });
            const auto min_weight = move_weights.min();
            weight += min_weight * 1.1;
            apply_move_util_.RevertMove();
        }
        return weight;
    } else {
        const auto piece_to = (*board_)[move.to.x][move.to.y];
        const bool capture = piece_to->GetColor() == curr_colour_;
        double weight = 0;
        if (capture)
            weight -= 1;
        if (depth > 1) {
            apply_move_util_.ApplyMove(move);
            const auto moves = get_all_legal_moves_util_.GetAllLegalMoves(curr_colour_);

            // // Cutting:
            // bool is_capture_within = false;
            // for (const auto move : *moves) {
            //     if ((*board_)[move.to.x][move.to.y]->GetColor() == oppo_colour_) {
            //         is_capture_within = true;
            //         break;
            //     }
            // }
            // if (is_capture_within) {
            //     const auto itor_new_end = std::remove_if(moves->begin(), moves->end(), [this](SurakartaMove move) {
            //         return (*board_)[move.to.x][move.to.y]->GetColor() != oppo_colour_;
            //     });
            //     moves->erase(itor_new_end, moves->end());
            // }

            // See: https://stackoverflow.com/questions/51074327/any-similar-function-like-js-array-prototype-map-in-c
            auto move_weights = std::valarray<double>(moves->size());
            std::transform(moves->begin(), moves->end(), std::begin(move_weights), [this, depth](SurakartaMove move) {
                return CalculateMoveWeight(move, depth - 1);
            });
            const auto max_weight = move_weights.max();
            weight += max_weight * 0.9;
            apply_move_util_.RevertMove();
        }
        return weight;
    }
}

SurakartaMove SurakartaAgentMine::CalculateMove() {
    const auto moves = get_all_legal_moves_util_.GetAllLegalMoves(curr_colour_);
    auto move_weights = std::valarray<double>(moves->size());
    std::transform(moves->begin(), moves->end(), std::begin(move_weights), [&](SurakartaMove move) {
        return move_weight_util_.CalculateMoveWeight(move);
    });
    const auto index = std::max_element(std::begin(move_weights), std::end(move_weights)) - std::begin(move_weights);
    return moves->at(index);
}
