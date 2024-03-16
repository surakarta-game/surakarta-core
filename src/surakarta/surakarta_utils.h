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

    std::optional<SurakartaPosition> Next(const SurakartaPosition position, const SurakartaDirection direction) {
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
            return SurakartaPosition(next.first, next.second);

        // Upper:
        if (next.first >= 1 && next.first < board_size_ / 2 && next.second == -1)
            return SurakartaPosition(0, next.first);
        if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == -1)
            return SurakartaPosition(board_size_ - 1, board_size_ - next.first - 1);

        // Lower:
        if (next.first >= 1 && next.first < board_size_ / 2 && next.second == board_size_)
            return SurakartaPosition(0, board_size_ - next.first - 1);
        if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == board_size_)
            return SurakartaPosition(board_size_ - 1, next.first);

        // Left:
        if (next.first == -1 && next.second >= 1 && next.second < board_size_ / 2)
            return SurakartaPosition(next.second, 0);
        if (next.first == -1 && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
            return SurakartaPosition(board_size_ - next.second - 1, board_size_ - 1);

        // Right:
        if (next.first == board_size_ && next.second >= 1 && next.second < board_size_ / 2)
            return SurakartaPosition(board_size_ - next.second - 1, 0);
        if (next.first == board_size_ && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
            return SurakartaPosition(next.second, board_size_ - 1);

        // Corner:
        return std::nullopt;
    }
};
