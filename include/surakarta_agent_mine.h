#pragma once

#include <memory>
#include "surakarta_agent_base.h"
#include "surakarta_utils.h"

class SurakartaMoveWeightUtilBase {
   protected:
    const std::shared_ptr<SurakartaBoard> board_;
    const PieceColor curr_colour_;
    const PieceColor oppo_colour_;

   public:
    SurakartaMoveWeightUtilBase(std::shared_ptr<SurakartaBoard> board, PieceColor curr_colour)
        : board_(board), curr_colour_(curr_colour), oppo_colour_(ReverseColor(curr_colour)) {}

    /// @param move The move to calculate the weight, must be valid.
    virtual double CalculateMoveWeight(SurakartaMove move) = 0;
};

class SurakartaMoveWeightUtil final : public SurakartaMoveWeightUtilBase {
   private:
    SurakartaApplyMoveUtil apply_move_util_;
    const SurakartaGetAllLegalMovesUtil get_all_legal_moves_util_;
    const int depth_;
    const double alpha_;
    const double beta_;

   public:
    static constexpr int DefaultDepth = 4;
    static constexpr double DefaultAlpha = 1.1;
    static constexpr double DefaultBeta = 0.9;
    SurakartaMoveWeightUtil(std::shared_ptr<SurakartaBoard> board,
                            PieceColor curr_colour,
                            int depth = DefaultDepth,
                            double alpha = DefaultAlpha,
                            double beta = DefaultBeta)
        : SurakartaMoveWeightUtilBase(board, curr_colour),
          apply_move_util_(board),
          get_all_legal_moves_util_(board),
          depth_(depth),
          alpha_(alpha),
          beta_(beta) {}

    /// @param move The move to calculate the weight, must be valid.
    virtual double CalculateMoveWeight(SurakartaMove move) override {
        return CalculateMoveWeight(move, 0, depth_);
    }

    /// @param move The move to calculate the weight, must be valid.
    double CalculateMoveWeight(SurakartaMove move, int captured_cnt = 0, int depth = DefaultDepth);
};

class SurakartaAgentMine : public SurakartaAgentBase {
   protected:
    const PieceColor curr_colour_;
    const SurakartaGetAllLegalMovesUtil get_all_legal_moves_util_;
    const std::shared_ptr<SurakartaMoveWeightUtilBase> move_weight_util_;
    std::mt19937 random_engine_;

   public:
    SurakartaAgentMine(std::shared_ptr<SurakartaBoard> board,
                       std::shared_ptr<SurakartaGameInfo> game_info,
                       std::shared_ptr<SurakartaRuleManager> rule_manager,
                       PieceColor curr_colour,
                       std::shared_ptr<SurakartaMoveWeightUtilBase> move_weight_util)
        : SurakartaAgentBase(board, game_info, rule_manager),
          curr_colour_(curr_colour),
          get_all_legal_moves_util_(board),
          move_weight_util_(move_weight_util),
          random_engine_(GlobalRandomGenerator::getInstance()) {}
    SurakartaMove CalculateMove() override;
};
