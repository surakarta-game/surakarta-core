#pragma once

#include <memory>
#include <vector>
#include "surakarta_rule_manager.h"

class SurakartaRuleManagerImpl : public SurakartaRuleManager {
   public:
    SurakartaRuleManagerImpl() = default;

    SurakartaRuleManagerImpl(std::shared_ptr<SurakartaBoard> board,
                             std::shared_ptr<SurakartaGameInfo> game_info)
        : SurakartaRuleManager(board, game_info) {}

    virtual ~SurakartaRuleManagerImpl() = default;

    virtual void OnUpdateBoard();

    /**
     * @brief Judge whether a move is legal.
     * @param move The move to be judged.
     */
    virtual SurakartaIllegalMoveReason JudgeMove(const SurakartaMove& move);

    /**
     * @brief Judge whether the game is end.
     * @param reason SurakartaIllegalMoveReason of the last move.
     */
    virtual std::pair<SurakartaEndReason, SurakartaPlayer> JudgeEnd(const SurakartaIllegalMoveReason reason);

    /**
     * @brief Get all legal target positions of a piece.
     * @param postion The position of the piece.
     */
    virtual std::unique_ptr<std::vector<SurakartaPosition>> GetAllLegalTarget(const SurakartaPosition postion);
};
