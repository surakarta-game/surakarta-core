#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "surakarta_agent_base.h"
#include "surakarta_daemon.h"
#include "surakarta_piece.h"

typedef int PieceId;

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
            int start_angle;  // 0 = right, 1 = up, 2 = left, 3 = down
            int end_angle;    // 0 = right, 1 = up, 2 = left, 3 = down
            bool is_clockwise;
        } curve;
    } info;
};

struct SurakartaMoveTrace {
    bool is_capture;
    int moved_piece_id;
    int captured_piece_id;  // -1 if no piece is captured
    std::unique_ptr<std::vector<SurakartaMovePathFragment>> path;
};

class SurakartaAgentInteractive {
   public:
    SurakartaAgentInteractive(std::shared_ptr<SurakartaDaemon> daemon);
    std::unique_ptr<SurakartaAgentBase> CreateAgent();

    bool IsMyTurn();
    std::unique_ptr<std::vector<PieceId>> MyPieces();
    std::unique_ptr<std::vector<PieceId>> OpponentPieces();
    SurakartaPosition PiecePosition(PieceId piece_id);

    PieceId SelectedPieceId();
    bool CanSelectPiece(SurakartaPosition position);
    bool SelectPiece(SurakartaPosition position);

    PieceId SelectedDestinationId();
    bool CanSelectDestination(SurakartaPosition position);
    bool SelectDestination(SurakartaPosition position);

    bool CanCommitMove();
    SurakartaMoveTrace CommitMove(std::function<void(SurakartaMoveTrace)> callback_for_next_move);
};
