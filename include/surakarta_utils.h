#pragma once

#include <list>
#include <optional>
#include <tuple>
#include "surakarta_common.h"

enum class SurakartaDirectionStraight {
    UP = 1,
    DOWN = 2,
    RIGHT = 3,
    LEFT = 4,
};

enum class SurakartaDirection {
    UP = 1,
    DOWN = 2,
    RIGHT = 3,
    LEFT = 4,
    UP_RIGHT = 5,
    UP_LEFT = 6,
    DOWN_RIGHT = 7,
    DOWN_LEFT = 8,
};

inline const SurakartaDirectionStraight SurakartaDirectionStraightList[] = {
    SurakartaDirectionStraight::UP,
    SurakartaDirectionStraight::DOWN,
    SurakartaDirectionStraight::LEFT,
    SurakartaDirectionStraight::RIGHT,
};

inline const SurakartaDirection SurakartaDirectionList[] = {
    SurakartaDirection::UP,
    SurakartaDirection::DOWN,
    SurakartaDirection::LEFT,
    SurakartaDirection::RIGHT,
    SurakartaDirection::UP_RIGHT,
    SurakartaDirection::UP_LEFT,
    SurakartaDirection::DOWN_RIGHT,
    SurakartaDirection::DOWN_LEFT,
};

inline const SurakartaDirection SurakartaDirectionNotStraightList[] = {
    SurakartaDirection::UP_RIGHT,
    SurakartaDirection::UP_LEFT,
    SurakartaDirection::DOWN_RIGHT,
    SurakartaDirection::DOWN_LEFT,
};

std::pair<int, int> operator+(SurakartaPosition position, SurakartaDirectionStraight direction);
std::pair<int, int> operator+(SurakartaPosition position, SurakartaDirection direction);
std::ostream& operator<<(std::ostream& os, const SurakartaDirectionStraight& dir);
std::ostream& operator<<(std::ostream& os, const SurakartaDirection& dir);

class SurakartaPieceMoveUtil {
   private:
    const int board_size_;

   public:
    SurakartaPieceMoveUtil(const int board_size)
        : board_size_(board_size){};

    std::optional<std::pair<SurakartaPosition, SurakartaDirectionStraight>> Next(std::pair<SurakartaPosition, SurakartaDirectionStraight> pair) const;
};

class SurakartaPieceMovableUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    const SurakartaPieceMoveUtil util_;

   public:
    SurakartaPieceMovableUtil(std::shared_ptr<const SurakartaBoard> board)
        : board_(board), util_(board->board_size_) {}

    bool IsMovable(const SurakartaPiece& piece) const;
};

class SurakartaPieceCanCaptureUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    const SurakartaPieceMoveUtil util_;

   public:
    SurakartaPieceCanCaptureUtil(std::shared_ptr<const SurakartaBoard> board)
        : board_(board), util_(board->board_size_) {}

    bool CanCaptureOpponentPiece(const SurakartaPiece& piece) const;

    bool CanCaptureOpponentPiece(PieceColor colour) const;
};

class SurakartaGetAllLegalTargetUtil {
   private:
    const int board_size_;
    const std::shared_ptr<const SurakartaBoard> board_;
    const SurakartaPieceMoveUtil util_;

   public:
    SurakartaGetAllLegalTargetUtil(std::shared_ptr<const SurakartaBoard> board)
        : board_size_(board->board_size_), board_(board), util_(board->board_size_) {}

    std::unique_ptr<std::vector<SurakartaPosition>> GetAllLegalTarget(const SurakartaPiece& piece) const;
};

class SurakartaGetAllLegalMovesUtil {
   private:
    const int board_size_;
    const std::shared_ptr<const SurakartaBoard> board_;
    const SurakartaGetAllLegalTargetUtil util_;

   public:
    SurakartaGetAllLegalMovesUtil(std::shared_ptr<const SurakartaBoard> board)
        : board_size_(board->board_size_), board_(board), util_(board) {}

    std::unique_ptr<std::vector<SurakartaMove>> GetAllLegalMoves(PieceColor colour) const;
};

class SurakartaApplyMoveUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    std::list<std::tuple<int, int, PieceColor>> revert_list_;

    void Pop();

   public:
    SurakartaApplyMoveUtil(std::shared_ptr<const SurakartaBoard> board)
        : board_(board) {}

    void ApplyMove(const SurakartaMove& move);
    void RevertMove();
};

class SurakartaMovablityUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    const SurakartaPieceMoveUtil util_;

   public:
    SurakartaMovablityUtil(std::shared_ptr<const SurakartaBoard> board)
        : board_(board), util_(board->board_size_) {}

    bool IsMovableToNoneCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
    bool IsMovableToCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
    bool IsMovableTo(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
};
