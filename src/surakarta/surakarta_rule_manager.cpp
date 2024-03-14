#include "surakarta_rule_manager.h"
#include <iostream>

void SurakartaRuleManager::OnUpdateBoard() {
    // TODO:
    // Every time the board and game_info is updated to the next round version, this function will be called.
    // You don't need to implement this function if you don't need it.
    // A more delicate way is to use Qt's signal and slot mechanism, but that's the advanced part.
}

SurakartaIllegalMoveReason SurakartaRuleManager::JudgeMove(const SurakartaMove& move) {
    // TODO: Implement this function.

    //* OUT_OF_BOARD,  // from or to position is out of board
    if (move.from.x < 0 || move.from.x >= board_size_ || move.from.y < 0 || move.from.y >= board_size_  //
        || move.to.x < 0 || move.to.x >= board_size_ || move.to.y < 0 || move.to.y >= board_size_)
        return SurakartaIllegalMoveReason::OUT_OF_BOARD;

    bool try_to_capture = false;

    SurakartaPlayer current_player = game_info_->current_player_;
    PieceColor aim_color = (*board_)[move.to.y][move.to.x]->GetColor();
    if (current_player == SurakartaPlayer::BLACK) {
        // black player's turn
        if (move.player != current_player)  //*NOT_PLAYER_PIECE,                    // move a piece that is not the player's
            return SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;

        if (aim_color == SurakartaPlayer::WHITE)
            try_to_capture = true;
    } else if (current_player == SurakartaPlayer::WHITE) {
        // white player's turn
        if (move.player != current_player)  //*NOT_PLAYER_PIECE,                    // move a piece that is not the player's
            return SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;

        if (aim_color == SurakartaPlayer::BLACK)
            try_to_capture = true;
    }

    for (unsigned int i = 0; i < board_size_; i++) {
        for (unsigned int j = 0; j < board_size_; j++) {
            PieceColor color = (*board_)[i][j]->GetColor();

            if (color == PieceColor::BLACK) {  // i,j is the position of a black piece

            } else if (color == PieceColor::WHITE) {  // i,j is the position of a white piece

            } else {  // i,j is an empty position

                if (move.to.x == j && move.to.y == i)                //*maybe a no_killing move
                    if ((move.to.x - j >= -1 && move.to.x - j <= 1)  //*LEGAL_NON_CAPTURE_MOVE,    // just as the name
                        && (move.to.y - i >= -1 && move.to.y - i <= 1))
                        return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
                    else  //*ILLIGAL_NON_CAPTURE_MOVE,  // otherwise
                        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;

                //*NOT_PIECE,  // move a position that is not a piece
                if (move.from.x == j && move.from.y == i)
                    return SurakartaIllegalMoveReason::NOT_PIECE;
            }
        }
        /*
            LEGAL_CAPTURE_MOVE,        // capture a opponent's piece, and the move consists at least one corner loop
            ILLIGAL_CAPTURE_MOVE,      // try to capture a opponent's piece, but the move can't consist any corner loop
        */
    }
    return SurakartaIllegalMoveReason::LEGAL;
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