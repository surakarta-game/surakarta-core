#pragma once

#include <iostream>

using PieceColorMemoryType = int;
enum class PieceColor : PieceColorMemoryType { BLACK,
                                               WHITE,
                                               NONE,
                                               UNKNOWN
};

using SurakartaPlayer = PieceColor;

inline PieceColor ReverseColor(PieceColor color) {
    switch (color) {
        case PieceColor::BLACK:
            return PieceColor::WHITE;
        case PieceColor::WHITE:
            return PieceColor::BLACK;
        default:
            return color;
    }
}

inline std::ostream& operator<<(std::ostream& os, const PieceColor& color) {
    switch (color) {
        case PieceColor::NONE:
            os << ".";
            break;
        case PieceColor::BLACK:
            os << "B";
            break;
        case PieceColor::WHITE:
            os << "W";
            break;
        default:
            os << "?";
            break;
    }
    return os;
}

inline std::istream& operator>>(std::istream& is, PieceColor& color) {
    char ch;
    is >> ch;
    switch (ch) {
        case '.':
            color = PieceColor::NONE;
            break;
        case 'B':
            color = PieceColor::BLACK;
            break;
        case 'W':
            color = PieceColor::WHITE;
            break;
        default:
            color = PieceColor::UNKNOWN;
            break;
    }
    return is;
}

struct SurakartaPosition {
    int x;
    int y;
    SurakartaPosition(int x = 0, int y = 0, bool debug_check = false, int board_size = 6)
        : x(x), y(y) {
        if (debug_check && (x < 0 || x >= board_size || y < 0 || y >= board_size)) {
            throw std::runtime_error("SurakartaPosition out of range");
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const SurakartaPosition& pos) {
        os << "(" << pos.x << ", " << pos.y << ")";
        return os;
    }
    bool operator==(const SurakartaPosition& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const SurakartaPosition& rhs) const {
        return !(*this == rhs);
    }
};

class SurakartaPiece {
   public:
    SurakartaPiece()
        : position_({0, 0}), color_(PieceColor::NONE) {}

    SurakartaPiece(int x, int y, PieceColor color)
        : position_({x, y}), color_(color) {}

    SurakartaPiece(SurakartaPosition position, PieceColor color)
        : position_(position), color_(color) {}

    void SetPosition(SurakartaPosition position) { position_ = position; }
    void SetColor(PieceColor color) { color_ = color; }
    void Set(SurakartaPosition position, PieceColor color) {
        position_ = position;
        color_ = color;
    }
    SurakartaPosition GetPosition() const { return position_; }
    PieceColor GetColor() const { return color_; }

    friend std::ostream& operator<<(std::ostream& os, const SurakartaPiece& piece) {
        os << piece.color_;
        return os;
    }

    //    private:
    SurakartaPosition position_;
    PieceColor color_;
};

typedef int PieceId;

struct SurakartaPositionWithId : public SurakartaPosition {
    PieceId id;

    SurakartaPositionWithId()
        : SurakartaPosition(0, 0), id(-1) {}

    SurakartaPositionWithId(int x, int y, PieceId id)
        : SurakartaPosition(x, y), id(id) {}
};

struct SurakartaMovePathFragment {
    bool is_curve;
    union {
        struct
        {
            int start_x;
            int start_y;
            int end_x;
            int end_y;
        } straight;
        struct
        {
            int center_x;
            int center_y;
            int radius;
            int start_angle;
            int end_angle;
            bool is_clockwise;
        } curve;
    } info;

    SurakartaMovePathFragment(int start_x, int start_y, int end_x, int end_y)
        : is_curve(false) {
        info.straight.start_x = start_x;
        info.straight.start_y = start_y;
        info.straight.end_x = end_x;
        info.straight.end_y = end_y;
    }
    SurakartaMovePathFragment(int center_x, int center_y, int radius, int start_angle, int end_angle, bool is_clockwise)
        : is_curve(true) {
        info.curve.center_x = center_x;
        info.curve.center_y = center_y;
        info.curve.radius = radius;
        info.curve.start_angle = start_angle;
        info.curve.end_angle = end_angle;
        info.curve.is_clockwise = is_clockwise;
    }
    SurakartaPosition From() const {
        if (is_curve) {
            constexpr int dx[4] = {1, 0, -1, 0};
            constexpr int dy[4] = {0, 1, 0, -1};
            return SurakartaPosition(info.curve.center_x + dx[info.curve.start_angle] * info.curve.radius,
                                     info.curve.center_y + dy[info.curve.start_angle] * info.curve.radius, true);
        } else {
            return SurakartaPosition(info.straight.start_x, info.straight.start_y, true);
        }
    }
    SurakartaPosition To() const {
        if (is_curve) {
            constexpr int dx[4] = {1, 0, -1, 0};
            constexpr int dy[4] = {0, 1, 0, -1};
            return SurakartaPosition(info.curve.center_x + dx[info.curve.end_angle] * info.curve.radius,
                                     info.curve.center_y + dy[info.curve.end_angle] * info.curve.radius, true);
        } else {
            return SurakartaPosition(info.straight.end_x, info.straight.end_y, true);
        }
    }
};

struct SurakartaMoveTrace {
    bool is_capture;
    SurakartaPositionWithId moved_piece;
    SurakartaPositionWithId captured_piece;  // -1 if no piece is captured
    std::vector<SurakartaMovePathFragment> path;
};
