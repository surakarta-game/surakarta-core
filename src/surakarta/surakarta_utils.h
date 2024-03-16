#include <optional>
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

const SurakartaDirectionStraight SurakartaDirectionStraightList[] = {
    SurakartaDirectionStraight::UP,
    SurakartaDirectionStraight::DOWN,
    SurakartaDirectionStraight::LEFT,
    SurakartaDirectionStraight::RIGHT,
};

const SurakartaDirection SurakartaDirectionList[] = {
    SurakartaDirection::UP,
    SurakartaDirection::DOWN,
    SurakartaDirection::LEFT,
    SurakartaDirection::RIGHT,
    SurakartaDirection::UP_RIGHT,
    SurakartaDirection::UP_LEFT,
    SurakartaDirection::DOWN_RIGHT,
    SurakartaDirection::DOWN_LEFT,
};

const SurakartaDirection SurakartaDirectionNotStraightList[] = {
    SurakartaDirection::UP_RIGHT,
    SurakartaDirection::UP_LEFT,
    SurakartaDirection::DOWN_RIGHT,
    SurakartaDirection::DOWN_LEFT,
};

std::pair<int, int> operator+(SurakartaPosition position, SurakartaDirectionStraight direction) {
    return direction == SurakartaDirectionStraight::UP
               ? std::pair(position.x, position.y - 1)
           : direction == SurakartaDirectionStraight::DOWN
               ? std::pair(position.x, position.y + 1)
           : direction == SurakartaDirectionStraight::LEFT
               ? std::pair(position.x - 1, position.y)
           : direction == SurakartaDirectionStraight::RIGHT
               ? std::pair(position.x + 1, position.y)
               : throw new std::logic_error("Invalid direction");
}

std::pair<int, int> operator+(SurakartaPosition position, SurakartaDirection direction) {
    return direction == SurakartaDirection::UP
               ? std::pair(position.x, position.y - 1)
           : direction == SurakartaDirection::DOWN
               ? std::pair(position.x, position.y + 1)
           : direction == SurakartaDirection::LEFT
               ? std::pair(position.x - 1, position.y)
           : direction == SurakartaDirection::RIGHT
               ? std::pair(position.x + 1, position.y)
           : direction == SurakartaDirection::UP_RIGHT
               ? std::pair(position.x + 1, position.y - 1)
           : direction == SurakartaDirection::UP_LEFT
               ? std::pair(position.x - 1, position.y - 1)
           : direction == SurakartaDirection::DOWN_RIGHT
               ? std::pair(position.x + 1, position.y + 1)
           : direction == SurakartaDirection::DOWN_LEFT
               ? std::pair(position.x - 1, position.y + 1)
               : throw new std::logic_error("Invalid direction");
}

class SurakartaPieceMoveUtil {
   private:
    const int board_size_;

   public:
    SurakartaPieceMoveUtil(const int board_size)
        : board_size_(board_size){};

    using PositionDirectionPair = std::pair<SurakartaPosition, SurakartaDirectionStraight>;
    std::optional<PositionDirectionPair> Next(PositionDirectionPair pair) const {
        const auto position = pair.first;
        const auto direction = pair.second;
        const std::pair<int, int> next = position + direction;
        // Inside:
        if (next.first >= 0 && next.first < board_size_ && next.second >= 0 && next.second < board_size_)
            return std::pair(SurakartaPosition(next.first, next.second), direction);

        // Upper:
        if (next.first >= 1 && next.first < board_size_ / 2 && next.second == -1)
            return std::pair(SurakartaPosition(0, next.first), SurakartaDirectionStraight::RIGHT);
        if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == -1)
            return std::pair(SurakartaPosition(board_size_ - 1, board_size_ - next.first - 1), SurakartaDirectionStraight::LEFT);

        // Lower:
        if (next.first >= 1 && next.first < board_size_ / 2 && next.second == board_size_)
            return std::pair(SurakartaPosition(0, board_size_ - next.first - 1), SurakartaDirectionStraight::RIGHT);
        if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == board_size_)
            return std::pair(SurakartaPosition(board_size_ - 1, next.first), SurakartaDirectionStraight::LEFT);

        // Left:
        if (next.first == -1 && next.second >= 1 && next.second < board_size_ / 2)
            return std::pair(SurakartaPosition(next.second, 0), SurakartaDirectionStraight::DOWN);
        if (next.first == -1 && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
            return std::pair(SurakartaPosition(board_size_ - next.second - 1, board_size_ - 1), SurakartaDirectionStraight::UP);

        // Right:
        if (next.first == board_size_ && next.second >= 1 && next.second < board_size_ / 2)
            return std::pair(SurakartaPosition(board_size_ - next.second - 1, 0), SurakartaDirectionStraight::DOWN);
        if (next.first == board_size_ && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
            return std::pair(SurakartaPosition(next.second, board_size_ - 1), SurakartaDirectionStraight::UP);

        // Corner:
        return std::nullopt;
    }
};

inline std::ostream& operator<<(std::ostream& os, const SurakartaDirectionStraight& dir) {
    switch (dir) {
        case SurakartaDirectionStraight::UP:
            os << "UP";
            break;
        case SurakartaDirectionStraight::DOWN:
            os << "DOWN";
            break;
        case SurakartaDirectionStraight::LEFT:
            os << "LEFT";
            break;
        case SurakartaDirectionStraight::RIGHT:
            os << "RIGHT";
            break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const SurakartaDirection& dir) {
    switch (dir) {
        case SurakartaDirection::UP:
            os << "UP";
            break;
        case SurakartaDirection::DOWN:
            os << "DOWN";
            break;
        case SurakartaDirection::LEFT:
            os << "LEFT";
            break;
        case SurakartaDirection::RIGHT:
            os << "RIGHT";
            break;
        case SurakartaDirection::UP_RIGHT:
            os << "UP_RIGHT";
            break;
        case SurakartaDirection::UP_LEFT:
            os << "UP_LEFT";
            break;
        case SurakartaDirection::DOWN_RIGHT:
            os << "DOWN_RIGHT";
            break;
        case SurakartaDirection::DOWN_LEFT:
            os << "DOWN_LEFT";
            break;
    }
    return os;
}

// class SurakartaPieceMovableUtil {
//    private:
//     const std::shared_ptr<const SurakartaBoard> board_;
//     const SurakartaPieceMoveUtil util_;

//    public:
//     SurakartaPieceMovableUtil(std::shared_ptr<const SurakartaBoard> board)
//         : board_(board), util_(board->board_size_) {}

//     bool IsMovable(const SurakartaPiece& piece) const {
//         const auto position = piece.GetPosition();
//         for (const auto direction : SurakartaDirectionStraightList) {
//             const auto pair_opt = util_.Next(std::pair(position, direction));
//             if (pair_opt.has_value()) {
//                 const auto position = pair_opt.value().first;
//                 if ((*board_)[position.x][position.y]->GetColor() == PieceColor::NONE)
//                     return true;
//             }
//         }
//         for (const auto direction : SurakartaDirectionNotStraightList) {
//             const auto next_position = position + direction;
//             if (board_->IsInside(SurakartaPosition(next_position.first, next_position.second)))
//                 if ((*board_)[next_position.first][next_position.second]->GetColor() == PieceColor::NONE)
//                     return true;
//         }
//         return false;
//     }
// };

// class SurakartaPieceCanCaptureUtil {
//    private:
//     const std::shared_ptr<const SurakartaBoard> board_;
//     const SurakartaPieceMoveUtil util_;

//    public:
//     SurakartaPieceCanCaptureUtil(std::shared_ptr<const SurakartaBoard> board)
//         : board_(board), util_(board->board_size_) {}

//     bool CanCaptureOpponentPiece(const SurakartaPiece& piece) const {
//         const auto curr_colour = piece.GetColor();
//         const auto oppo_colour = ReverseColor(curr_colour);
//         const auto position = piece.GetPosition();
//         for (const auto start_direction : SurakartaDirectionStraightList) {
//             auto curr_position = position;
//             auto curr_direction = start_direction;
//             int passed_corner_cnt = 0;
//             while (true) {
//                 const auto next_pair_opt = util_.Next(std::pair(curr_position, curr_direction));  // Try move a step
//                 if (next_pair_opt.has_value() == false)                                           // At corner
//                     break;
//                 auto [next_position, next_direction] = next_pair_opt.value();
//                 curr_position = next_position;
//                 if (next_direction != curr_direction)  // Has gone through a quarter
//                     passed_corner_cnt++;
//                 curr_direction = next_direction;
//                 if ((*board_)[curr_position.x][curr_position.y]->GetColor() == oppo_colour) {  // Reached
//                     if (passed_corner_cnt > 0)
//                         return true;
//                     else
//                         break;  // Must pass a corner before capture
//                 }
//                 // Encountered another piece (not self):
//                 if (curr_position != position && (*board_)[curr_position.x][curr_position.y]->GetColor() != PieceColor::NONE)
//                     break;
//                 if (curr_position == position && passed_corner_cnt == 4)  // Has gone back
//                     break;
//             }
//         }
//         return false;
//     }
// };
