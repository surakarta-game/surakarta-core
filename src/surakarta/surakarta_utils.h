#include <optional>
#include "surakarta_common.h"

enum class SurakartaDirection {
    UP = 1,
    DOWN = 2,
    RIGHT = 3,
    LEFT = 4,
};

class SurakartaPieceMoveUtil {
   private:
    const int board_size_;

   public:
    SurakartaPieceMoveUtil(const int board_size)
        : board_size_(board_size){};

    std::optional<std::pair<SurakartaPosition, SurakartaDirection>> Next(const SurakartaPosition position, const SurakartaDirection direction) {
        const std::pair<int, int> next =
            direction == SurakartaDirection::UP
                ? std::pair(position.x, position.y - 1)
            : direction == SurakartaDirection::DOWN
                ? std::pair(position.x, position.y + 1)
            : direction == SurakartaDirection::LEFT
                ? std::pair(position.x - 1, position.y)
            : direction == SurakartaDirection::RIGHT
                ? std::pair(position.x + 1, position.y)
                : throw new std::logic_error("Invalid direction");
        // Inside:
        if (next.first >= 0 && next.first < board_size_ && next.second >= 0 && next.second < board_size_)
            return std::pair(SurakartaPosition(next.first, next.second), direction);

        // Upper:
        if (next.first >= 1 && next.first < board_size_ / 2 && next.second == -1)
            return std::pair(SurakartaPosition(0, next.first), SurakartaDirection::RIGHT);
        if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == -1)
            return std::pair(SurakartaPosition(board_size_ - 1, board_size_ - next.first - 1), SurakartaDirection::LEFT);

        // Lower:
        if (next.first >= 1 && next.first < board_size_ / 2 && next.second == board_size_)
            return std::pair(SurakartaPosition(0, board_size_ - next.first - 1), SurakartaDirection::RIGHT);
        if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == board_size_)
            return std::pair(SurakartaPosition(board_size_ - 1, next.first), SurakartaDirection::LEFT);

        // Left:
        if (next.first == -1 && next.second >= 1 && next.second < board_size_ / 2)
            return std::pair(SurakartaPosition(next.second, 0), SurakartaDirection::DOWN);
        if (next.first == -1 && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
            return std::pair(SurakartaPosition(board_size_ - next.second - 1, board_size_ - 1), SurakartaDirection::UP);

        // Right:
        if (next.first == board_size_ && next.second >= 1 && next.second < board_size_ / 2)
            return std::pair(SurakartaPosition(board_size_ - next.second - 1, 0), SurakartaDirection::DOWN);
        if (next.first == board_size_ && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
            return std::pair(SurakartaPosition(next.second, board_size_ - 1), SurakartaDirection::UP);

        // Corner:
        return std::nullopt;
    }
};

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
    }
    return os;
}
