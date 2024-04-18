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
    std::optional<SurakartaMovePathFragment> PathFragment(std::pair<SurakartaPosition, SurakartaDirectionStraight> pair) const;
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

    std::unique_ptr<std::vector<SurakartaPosition>> GetAllLegalTargets(const SurakartaPiece& piece) const;
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

class SurakartaTemporarilyApplyMoveGuardUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    SurakartaApplyMoveUtil apply_move_util_;

   public:
    SurakartaTemporarilyApplyMoveGuardUtil(std::shared_ptr<const SurakartaBoard> board, const SurakartaMove& move)
        : board_(board), apply_move_util_(board) {
        apply_move_util_.ApplyMove(move);
    }

    ~SurakartaTemporarilyApplyMoveGuardUtil() {
        apply_move_util_.RevertMove();
    }
};

class SurakartaTemporarilyChangeColorGuardUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    bool is_used_ = false;
    SurakartaPosition position_;
    PieceColor origin_color_;

   public:
    SurakartaTemporarilyChangeColorGuardUtil() {}
    SurakartaTemporarilyChangeColorGuardUtil(std::shared_ptr<const SurakartaBoard> board, SurakartaPosition position, PieceColor color)
        : board_(board), is_used_(true), position_(position), origin_color_((*board_)[position.x][position.y]->GetColor()) {
        (*board_)[position_.x][position_.y]->SetColor(color);
    }

    ~SurakartaTemporarilyChangeColorGuardUtil() {
        if (is_used_)
            (*board_)[position_.x][position_.y]->SetColor(origin_color_);
    }
};

class SurakartaMovablityUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    const SurakartaPieceMoveUtil util_;

   public:
    SurakartaMovablityUtil(
        std::shared_ptr<const SurakartaBoard> board)
        : board_(board),
          util_(board->board_size_) {}

    bool IsMovableToNoneCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
    SurakartaDirection GetDirectionNoneCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
    bool IsMovableToCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
    SurakartaDirectionStraight GetDirectionCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
    bool IsMovableTo(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const;
};

class SurakartaTraceGenerateUtil {
   private:
    const std::shared_ptr<const SurakartaBoard> board_;
    const SurakartaPieceMoveUtil move_util_;
    std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_black_;
    std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_white_;

   public:
    SurakartaTraceGenerateUtil(
        std::shared_ptr<const SurakartaBoard> board,
        std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_black,
        std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_white)
        : board_(board),
          move_util_(board->board_size_),
          id_position_list_black_(id_position_list_black),
          id_position_list_white_(id_position_list_white) {}

    SurakartaMoveTrace GenerateNoneCaptured(SurakartaPosition from, SurakartaDirection start_direction) const;
    SurakartaMoveTrace GenerateCaptured(SurakartaPosition from, SurakartaDirectionStraight start_direction) const;
};

class SurakartaInitPositionListsUtil {
   private:
    const std::shared_ptr<SurakartaBoard> board_;

   public:
    SurakartaInitPositionListsUtil(std::shared_ptr<SurakartaBoard> board)
        : board_(board) {}

    struct PositionLists {
        std::shared_ptr<std::vector<SurakartaPositionWithId>> black_list;
        std::shared_ptr<std::vector<SurakartaPositionWithId>> white_list;
    };
    PositionLists InitPositionList() const;
};

class SurakartaOnBoardUpdateUtil {
   private:
    std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_black_;
    std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_white_;
    std::shared_ptr<SurakartaBoard> board_;
    const SurakartaPieceMoveUtil move_util_;
    const SurakartaMovablityUtil movability_util_;
    const SurakartaTraceGenerateUtil trace_generate_util_;

    std::optional<std::pair<SurakartaMove, bool>> ResolveMove() const;
    void Update(const SurakartaMove& move) const;
    SurakartaMoveTrace GetTrace(const SurakartaMove& move, bool is_capture_move) const;

   public:
    SurakartaOnBoardUpdateUtil(
        std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_black_,
        std::shared_ptr<std::vector<SurakartaPositionWithId>> id_position_list_white_,
        std::shared_ptr<SurakartaBoard> board)
        : id_position_list_black_(id_position_list_black_),
          id_position_list_white_(id_position_list_white_),
          board_(board),
          move_util_((int)board->size()),
          movability_util_(board),
          trace_generate_util_(board, id_position_list_black_, id_position_list_white_) {}

    std::optional<SurakartaMoveTrace> UpdateAndGetTrace() const {
        const auto move = ResolveMove();
        if (move.has_value() == false)
            return std::nullopt;
        const auto trace = GetTrace(move.value().first, move.value().second);
        Update(move.value().first);
        return trace;
    }
};
