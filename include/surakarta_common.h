#pragma once

#include "surakarta_board.h"
#include "surakarta_piece.h"
#include "surakarta_reason.h"

#define MAX_NO_CAPTURE_ROUND 40

struct SurakartaMove {
    SurakartaPosition from;
    SurakartaPosition to;
    SurakartaPlayer player;
    SurakartaMove()
        : from(0, 0), to(0, 0), player(SurakartaPlayer::NONE) {}
    SurakartaMove(int from_x, int from_y, int to_x, int to_y, SurakartaPlayer player)
        : from(from_x, from_y), to(to_x, to_y), player(player) {}
    SurakartaMove(SurakartaPosition from, SurakartaPosition to, SurakartaPlayer player)
        : from(from), to(to), player(player) {}
    friend std::ostream& operator<<(std::ostream& os, const SurakartaMove& move) {
        os << move.from << " -> " << move.to << " (" << move.player << ")";
        return os;
    }
    bool operator==(const SurakartaMove& other) const {
        return from == other.from && to == other.to && player == other.player;
    }
};

struct SurakartaGameInfo {
    SurakartaPlayer current_player_;
    int num_round_;
    int last_captured_round_;
    SurakartaEndReason end_reason_;
    SurakartaPlayer winner_;
    int max_no_capture_round_;

    SurakartaGameInfo(int max_no_capture_round = MAX_NO_CAPTURE_ROUND)
        : current_player_(SurakartaPlayer::BLACK),
          num_round_(1),
          last_captured_round_(0),
          end_reason_(SurakartaEndReason::NONE),
          winner_(SurakartaPlayer::NONE),
          max_no_capture_round_(max_no_capture_round) {}

    void Reset() {
        current_player_ = SurakartaPlayer::BLACK;
        num_round_ = 1;
        last_captured_round_ = 0;
        end_reason_ = SurakartaEndReason::NONE;
        winner_ = SurakartaPlayer::NONE;
    }

    bool IsEnd() const { return IsEndReason(end_reason_); }
    SurakartaPlayer Winner() const { return winner_; }

    friend std::ostream& operator<<(std::ostream& os, const SurakartaGameInfo& game_info) {
        os << "current_player: " << game_info.current_player_ << std::endl;
        os << "num_round: " << game_info.num_round_ << std::endl;
        os << "last_captured_round: " << game_info.last_captured_round_ << std::endl;
        os << "end_reason: " << game_info.end_reason_ << std::endl;
        os << "winner: " << game_info.winner_ << std::endl;
        os << "max_no_capture_round: " << game_info.max_no_capture_round_ << std::endl;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, SurakartaGameInfo& game_info) {
        std::string str;
        is >> str >> game_info.current_player_;
        is >> str >> game_info.num_round_;
        is >> str >> game_info.last_captured_round_;
        is >> str >> game_info.end_reason_;
        is >> str >> game_info.winner_;
        is >> str >> game_info.max_no_capture_round_;
        return is;
    }
};

class SurakartaException : public std::exception {
   public:
    SurakartaException(const std::string& message)
        : message_(message) {}
    virtual const char* what() const throw() { return message_.c_str(); }

   private:
    std::string message_;
};
