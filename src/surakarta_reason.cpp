#include "surakarta_reason.h"

std::string SurakartaToString(SurakartaIllegalMoveReason reason) {
    switch (reason) {
        case SurakartaIllegalMoveReason::LEGAL:
            return "LEGAL";
        case SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE:
            return "LEGAL_CAPTURE_MOVE";
        case SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE:
            return "LEGAL_NON_CAPTURE_MOVE";
        case SurakartaIllegalMoveReason::ILLIGAL:
            return "ILLIGAL";
        case SurakartaIllegalMoveReason::NOT_PLAYER_TURN:
            return "NOT_PLAYER_TURN";
        case SurakartaIllegalMoveReason::OUT_OF_BOARD:
            return "OUT_OF_BOARD";
        case SurakartaIllegalMoveReason::NOT_PIECE:
            return "NOT_PIECE";
        case SurakartaIllegalMoveReason::NOT_PLAYER_PIECE:
            return "NOT_PLAYER_PIECE";
        case SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE:
            return "ILLIGAL_CAPTURE_MOVE";
        case SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE:
            return "ILLIGAL_NON_CAPTURE_MOVE";
        case SurakartaIllegalMoveReason::GAME_ALREADY_END:
            return "GAME_ALREADY_END";
        case SurakartaIllegalMoveReason::GAME_NOT_START:
            return "GAME_NOT_START";
        default:
            return "UNKNOWN";
    }
}

std::string SurakartaToString(SurakartaEndReason reason) {
    switch (reason) {
        case SurakartaEndReason::NONE:
            return "NONE";
        case SurakartaEndReason::STALEMATE:
            return "STALEMATE";
        case SurakartaEndReason::CHECKMATE:
            return "CHECKMATE";
        case SurakartaEndReason::TRAPPED:
            return "TRAPPED";
        case SurakartaEndReason::RESIGN:
            return "RESIGN";
        case SurakartaEndReason::TIMEOUT:
            return "TIMEOUT";
        case SurakartaEndReason::ILLIGAL_MOVE:
            return "ILLIGAL_MOVE";
        default:
            return "UNKNOWN";
    }
}

bool IsLegalMoveReason(SurakartaIllegalMoveReason reason) {
    switch (reason) {
        case SurakartaIllegalMoveReason::LEGAL:
        case SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE:
        case SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE:
            return true;
        default:
            return false;
    }
}

bool IsEndReason(SurakartaEndReason reason) {
    switch (reason) {
        case SurakartaEndReason::NONE:
            return false;
        default:
            return true;
    }
}

std::ostream& operator<<(std::ostream& os, const SurakartaIllegalMoveReason& reason) {
    os << SurakartaToString(reason);
    return os;
}

std::istream& operator>>(std::istream& is, SurakartaIllegalMoveReason& reason) {
    std::string str;
    is >> str;
    if (str == "LEGAL") {
        reason = SurakartaIllegalMoveReason::LEGAL;
    } else if (str == "LEGAL_CAPTURE_MOVE") {
        reason = SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
    } else if (str == "LEGAL_NON_CAPTURE_MOVE") {
        reason = SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;
    } else if (str == "ILLIGAL") {
        reason = SurakartaIllegalMoveReason::ILLIGAL;
    } else if (str == "NOT_PLAYER_TURN") {
        reason = SurakartaIllegalMoveReason::NOT_PLAYER_TURN;
    } else if (str == "OUT_OF_BOARD") {
        reason = SurakartaIllegalMoveReason::OUT_OF_BOARD;
    } else if (str == "NOT_PIECE") {
        reason = SurakartaIllegalMoveReason::NOT_PIECE;
    } else if (str == "NOT_PLAYER_PIECE") {
        reason = SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;
    } else if (str == "ILLIGAL_CAPTURE_MOVE") {
        reason = SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
    } else if (str == "ILLIGAL_NON_CAPTURE_MOVE") {
        reason = SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
    } else if (str == "GAME_ALREADY_END") {
        reason = SurakartaIllegalMoveReason::GAME_ALREADY_END;
    } else if (str == "GAME_NOT_START") {
        reason = SurakartaIllegalMoveReason::GAME_NOT_START;
    } else {
        reason = SurakartaIllegalMoveReason::ILLIGAL;
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const SurakartaEndReason& reason) {
    os << SurakartaToString(reason);
    return os;
}

std::istream& operator>>(std::istream& is, SurakartaEndReason& reason) {
    std::string str;
    is >> str;
    if (str == "NONE") {
        reason = SurakartaEndReason::NONE;
    } else if (str == "STALEMATE") {
        reason = SurakartaEndReason::STALEMATE;
    } else if (str == "CHECKMATE") {
        reason = SurakartaEndReason::CHECKMATE;
    } else if (str == "TRAPPED") {
        reason = SurakartaEndReason::TRAPPED;
    } else if (str == "RESIGN") {
        reason = SurakartaEndReason::RESIGN;
    } else if (str == "TIMEOUT") {
        reason = SurakartaEndReason::TIMEOUT;
    } else if (str == "ILLIGAL_MOVE") {
        reason = SurakartaEndReason::ILLIGAL_MOVE;
    } else {
        reason = SurakartaEndReason::NONE;
    }
    return is;
}