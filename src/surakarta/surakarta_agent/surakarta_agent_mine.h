#ifndef SURAKARTA_AGENT_MINE_H
#define SURAKARTA_AGENT_MINE_H

#include <memory>
#include "surakarta/surakarta_agent/surakarta_agent_base.h"
#include "surakarta/surakarta_utils.h"

constexpr unsigned int surakarta_weight_calculation_default_depth = 5;

class SurakartaMoveWeightUtil {
   private:
    const std::shared_ptr<SurakartaBoard> board_;
    const PieceColor curr_colour_;
    const PieceColor oppo_colour_;
    SurakartaApplyMoveUtil apply_move_util_;
    const SurakartaGetAllLegalMovesUtil get_all_legal_moves_util_;

   public:
    SurakartaMoveWeightUtil(std::shared_ptr<SurakartaBoard> board, PieceColor curr_colour)
        : board_(board), curr_colour_(curr_colour), oppo_colour_(ReverseColor(curr_colour)), apply_move_util_(board), get_all_legal_moves_util_(board) {}

    /// @param move The move to calculate the weight, must be valid.
    double CalculateMoveWeight(SurakartaMove move, unsigned int depth = surakarta_weight_calculation_default_depth);
};

class SurakartaAgentMine : public SurakartaAgentBase {
   protected:
    const PieceColor curr_colour_;
    const SurakartaGetAllLegalMovesUtil get_all_legal_moves_util_;
    SurakartaMoveWeightUtil move_weight_util_;

   public:
    SurakartaAgentMine(std::shared_ptr<SurakartaBoard> board,
                       std::shared_ptr<SurakartaGameInfo> game_info,
                       std::shared_ptr<SurakartaRuleManager> rule_manager,
                       PieceColor curr_colour)
        : SurakartaAgentBase(board, game_info, rule_manager),
          curr_colour_(curr_colour),
          get_all_legal_moves_util_(board),
          move_weight_util_(board, curr_colour_) {}
    SurakartaMove CalculateMove() override;
};

#endif  // SURAKARTA_AGENT_MINE_H
