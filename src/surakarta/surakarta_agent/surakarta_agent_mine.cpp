#include "surakarta/surakarta_agent/surakarta_agent_mine.h"
#include "surakarta_agent_mine.h"

#include <algorithm>
#include <valarray>

double SurakartaMoveWeightUtil::CalculateMoveWeight(SurakartaMove move,
                                                    unsigned int captured_cnt,
                                                    int depth) {
    if (move.player == curr_colour_) {
        const auto piece_to = (*board_)[move.to.x][move.to.y];
        const bool capture = piece_to->GetColor() == oppo_colour_;
        double weight = 0;
        if (capture) {
            weight += 1;
            captured_cnt += 1;
        }
        if (depth + 0.0 * captured_cnt > 1) {
            apply_move_util_.ApplyMove(move);
            const auto moves = get_all_legal_moves_util_.GetAllLegalMoves(oppo_colour_);
            if (moves->size() > 0) {
                auto move_weights = std::valarray<double>(moves->size());
                for (size_t i = 0; i < moves->size(); i++) {
                    move_weights[i] = CalculateMoveWeight(moves->at(i), captured_cnt, depth - 1);
                }
                const auto min_weight = move_weights.min();
                weight += min_weight * alpha_;
            }
            apply_move_util_.RevertMove();
        }
        return weight;
    } else {
        const auto piece_to = (*board_)[move.to.x][move.to.y];
        const bool capture = piece_to->GetColor() == curr_colour_;
        double weight = 0;
        if (capture) {
            weight -= 1;
            captured_cnt += 1;
        }
        if (depth + 0.0 * captured_cnt > 1) {
            apply_move_util_.ApplyMove(move);
            const auto moves = get_all_legal_moves_util_.GetAllLegalMoves(curr_colour_);
            if (moves->size() > 0) {
                auto move_weights = std::valarray<double>(moves->size());
                for (size_t i = 0; i < moves->size(); i++) {
                    move_weights[i] = CalculateMoveWeight(moves->at(i), captured_cnt, depth - 1);
                }
                const auto max_weight = move_weights.max();
                weight += max_weight * beta_;
            }
            apply_move_util_.RevertMove();
        }
        return weight;
    }
}

SurakartaMove SurakartaAgentMine::CalculateMove() {
    const auto moves = get_all_legal_moves_util_.GetAllLegalMoves(curr_colour_);
    auto move_weights = std::valarray<double>(moves->size());
    std::transform(moves->begin(), moves->end(), std::begin(move_weights), [&](SurakartaMove move) {
        return move_weight_util_->CalculateMoveWeight(move);
    });
    const auto max_weight = move_weights.max();
    auto max_weight_indexes = std::vector<size_t>();
    for (size_t i = 0; i < moves->size(); i++) {
        if (move_weights[i] == max_weight) {
            max_weight_indexes.push_back(i);
        }
    }
    const auto max_weight_index = max_weight_indexes[random_engine_() % max_weight_indexes.size()];
    return moves->at(max_weight_index);
}
