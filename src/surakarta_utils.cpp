#include "surakarta_utils.h"

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

std::ostream& operator<<(std::ostream& os, const SurakartaDirectionStraight& dir) {
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

std::ostream& operator<<(std::ostream& os, const SurakartaDirection& dir) {
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

std::optional<std::pair<SurakartaPosition, SurakartaDirectionStraight>> SurakartaPieceMoveUtil::Next(std::pair<SurakartaPosition, SurakartaDirectionStraight> pair) const {
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

std::optional<SurakartaMovePathFragment> SurakartaPieceMoveUtil::PathFragment(std::pair<SurakartaPosition, SurakartaDirectionStraight> pair) const {
    const auto position = pair.first;
    const auto direction = pair.second;
    const std::pair<int, int> next = position + direction;
    // Inside:
    if (next.first >= 0 && next.first < board_size_ && next.second >= 0 && next.second < board_size_)
        return SurakartaMovePathFragment(position.x, position.y, next.first, next.second);
    // Upper:
    if (next.first >= 1 && next.first < board_size_ / 2 && next.second == -1)
        return SurakartaMovePathFragment(0, 0, next.first, 1, 0, false);
    if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == -1)
        return SurakartaMovePathFragment(board_size_ - 1, 0, board_size_ - next.first - 1, 3, 0, true);
    // Lower:
    if (next.first >= 1 && next.first < board_size_ / 2 && next.second == board_size_)
        return SurakartaMovePathFragment(0, board_size_ - 1, next.first, 1, 2, true);
    if (next.first >= board_size_ / 2 && next.first < board_size_ - 1 && next.second == board_size_)
        return SurakartaMovePathFragment(board_size_ - 1, board_size_ - 1, board_size_ - next.first - 1, 3, 2, false);
    // Left:
    if (next.first == -1 && next.second >= 1 && next.second < board_size_ / 2)
        return SurakartaMovePathFragment(0, 0, next.second, 0, 1, true);
    if (next.first == -1 && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
        return SurakartaMovePathFragment(0, board_size_ - 1, board_size_ - next.second - 1, 2, 1, false);

    // Right:
    if (next.first == board_size_ && next.second >= 1 && next.second < board_size_ / 2)
        return SurakartaMovePathFragment(board_size_ - 1, 0, next.second, 0, 3, false);
    if (next.first == board_size_ && next.second >= board_size_ / 2 && next.second < board_size_ - 1)
        return SurakartaMovePathFragment(board_size_ - 1, board_size_ - 1, board_size_ - next.second - 1, 2, 3, true);

    // Corner:
    return std::nullopt;
}

bool SurakartaPieceMovableUtil::IsMovable(const SurakartaPiece& piece) const {
    const auto position = piece.GetPosition();
    for (const auto direction : SurakartaDirectionStraightList) {
        const auto pair_opt = util_.Next(std::pair(position, direction));
        if (pair_opt.has_value()) {
            const auto position_next = pair_opt.value().first;
            if ((*board_)[position_next.x][position_next.y]->GetColor() == PieceColor::NONE)
                return true;
        }
    }
    for (const auto direction : SurakartaDirectionNotStraightList) {
        const auto next_position = position + direction;
        if (board_->IsInside(SurakartaPosition(next_position.first, next_position.second)))
            if ((*board_)[next_position.first][next_position.second]->GetColor() == PieceColor::NONE)
                return true;
    }
    return false;
}

bool SurakartaPieceCanCaptureUtil::CanCaptureOpponentPiece(const SurakartaPiece& piece) const {
    const auto curr_colour = piece.GetColor();
    const auto oppo_colour = ReverseColor(curr_colour);
    const auto position = piece.GetPosition();
    for (const auto start_direction : SurakartaDirectionStraightList) {
        auto curr_position = position;
        auto curr_direction = start_direction;
        int passed_corner_cnt = 0;
        while (true) {
            const auto next_pair_opt = util_.Next(std::pair(curr_position, curr_direction));  // Try move a step
            if (next_pair_opt.has_value() == false)                                           // At corner
                break;
            auto [next_position, next_direction] = next_pair_opt.value();
            curr_position = next_position;
            if (next_direction != curr_direction)  // Has gone through a quarter
                passed_corner_cnt++;
            curr_direction = next_direction;
            if ((*board_)[curr_position.x][curr_position.y]->GetColor() == oppo_colour) {  // Reached
                if (passed_corner_cnt > 0)
                    return true;
                else
                    break;  // Must pass a corner before capture
            }
            // Encountered another piece (not self):
            if (curr_position != position && (*board_)[curr_position.x][curr_position.y]->GetColor() != PieceColor::NONE)
                break;
            if (curr_position == position && passed_corner_cnt == 4)  // Has gone back
                break;
        }
    }
    return false;
}

bool SurakartaPieceCanCaptureUtil::CanCaptureOpponentPiece(PieceColor colour) const {
    for (const auto& column : *board_) {
        for (const auto& piece : column) {
            if (piece->GetColor() == colour && CanCaptureOpponentPiece(*piece))
                return true;
        }
    }
    return false;
}

std::unique_ptr<std::vector<SurakartaPosition>> SurakartaGetAllLegalTargetUtil::GetAllLegalTargets(const SurakartaPiece& piece) const {
    const auto position = piece.GetPosition();
    const auto curr_colour = piece.GetColor();
    const auto oppo_colour = ReverseColor(curr_colour);
    const auto result_map = std::make_unique<std::vector<bool>>(board_size_ * board_size_, false);
    for (const auto direction : SurakartaDirectionList) {
        const auto next_position = position + direction;
        if (board_->IsInside(SurakartaPosition(next_position.first, next_position.second)))
            if ((*board_)[next_position.first][next_position.second]->GetColor() == PieceColor::NONE)
                result_map->at(next_position.first * board_size_ + next_position.second) = true;
    }
    for (const auto start_direction : SurakartaDirectionStraightList) {
        auto curr_position = position;
        auto curr_direction = start_direction;
        int passed_corner_cnt = 0;
        while (true) {
            const auto next_pair_opt = util_.Next(std::pair(curr_position, curr_direction));  // Try move a step
            if (next_pair_opt.has_value() == false)                                           // At corner
                break;
            auto [next_position, next_direction] = next_pair_opt.value();
            curr_position = next_position;
            if (next_direction != curr_direction)  // Has gone through a quarter
                passed_corner_cnt++;
            curr_direction = next_direction;
            const auto curr_position_colour = (*board_)[curr_position.x][curr_position.y]->GetColor();
            // Encountered another piece (not self):
            if (curr_position != position && curr_position_colour != PieceColor::NONE) {
                if (curr_position_colour == oppo_colour && passed_corner_cnt > 0) {
                    result_map->at(curr_position.x * board_size_ + curr_position.y) = true;
                }
                break;
            }
            if (curr_position == position && passed_corner_cnt == 4)  // Has gone back
                break;
        }
    }
    auto result = std::make_unique<std::vector<SurakartaPosition>>();
    for (int i = 0; i < board_size_; i++) {
        for (int j = 0; j < board_size_; j++) {
            if (result_map->at(i * board_size_ + j))
                result->push_back(SurakartaPosition(i, j));
        }
    }
    return result;
}

std::unique_ptr<std::vector<SurakartaMove>> SurakartaGetAllLegalMovesUtil::GetAllLegalMoves(PieceColor colour) const {
    auto result = std::make_unique<std::vector<SurakartaMove>>();
    for (int i = 0; i < board_size_; i++) {
        for (int j = 0; j < board_size_; j++) {
            const auto piece = (*board_)[i][j];
            if (piece->GetColor() == colour) {
                const auto targets = util_.GetAllLegalTargets(*piece);
                for (const auto pos : *targets) {
                    result->push_back(SurakartaMove(SurakartaPosition(i, j), pos, colour));
                }
            }
        }
    }
    return result;
}

void SurakartaApplyMoveUtil::Pop() {
    auto tuple = revert_list_.back();
    (*board_)[std::get<0>(tuple)][std::get<1>(tuple)]->SetColor(std::get<2>(tuple));
    revert_list_.pop_back();
}

void SurakartaApplyMoveUtil::ApplyMove(const SurakartaMove& move) {
    const auto from = move.from;
    const auto to = move.to;
    const auto piece_from = (*board_)[from.x][from.y];
    const auto pitce_to = (*board_)[to.x][to.y];
    revert_list_.push_back(std::tuple(from.x, from.y, piece_from->GetColor()));
    revert_list_.push_back(std::tuple(to.x, to.y, pitce_to->GetColor()));
    piece_from->SetColor(PieceColor::NONE);
    pitce_to->SetColor(move.player);
}

void SurakartaApplyMoveUtil::RevertMove() {
    Pop();
    Pop();
}

bool SurakartaMovablityUtil::IsMovableToNoneCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const {
    if (piece.GetColor() == PieceColor::NONE || piece_to.GetColor() != PieceColor::NONE)
        return false;
    const auto position = piece.GetPosition();
    const auto position_to = piece_to.GetPosition();
    for (const auto direction : SurakartaDirectionList) {
        const auto next = position + direction;
        const auto next_pos = SurakartaPosition(next.first, next.second);
        if (board_->IsInside(next_pos) == false)
            continue;
        if ((*board_)[next_pos.x][next_pos.y]->GetColor() != PieceColor::NONE)
            continue;
        if (next_pos == position_to)
            return true;
    }
    return false;
}

SurakartaDirection SurakartaMovablityUtil::GetDirectionNoneCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const {
    const auto position = piece.GetPosition();
    const auto position_to = piece_to.GetPosition();
    for (const auto direction : SurakartaDirectionList) {
        const auto next = position + direction;
        const auto next_pos = SurakartaPosition(next.first, next.second);
        if (board_->IsInside(next_pos) == false)
            continue;
        if ((*board_)[next_pos.x][next_pos.y]->GetColor() != PieceColor::NONE)
            continue;
        if (next_pos == position_to)
            return direction;
    }
    throw std::runtime_error("Invalid move");
}

bool SurakartaMovablityUtil::IsMovableToCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const {
    if (piece.GetColor() == PieceColor::NONE || piece_to.GetColor() == PieceColor::NONE)
        return false;
    if (piece.GetColor() == piece_to.GetColor())
        return false;
    const auto position = piece.GetPosition();
    const auto position_to = piece_to.GetPosition();
    for (const auto start_direction : SurakartaDirectionStraightList) {
        auto curr_position = position;
        auto curr_direction = start_direction;
        int passed_corner_cnt = 0;
        while (true) {
            const auto next_pair_opt = util_.Next(std::pair(curr_position, curr_direction));  // Try move a step
            if (next_pair_opt.has_value() == false)                                           // At corner
                break;
            auto [next_position, next_direction] = next_pair_opt.value();
            curr_position = next_position;
            if (next_direction != curr_direction)  // Has gone through a quarter
                passed_corner_cnt++;
            curr_direction = next_direction;
            if (curr_position == position_to) {  // Reached
                if (passed_corner_cnt > 0)
                    return true;
                else
                    break;  // Must pass a corner before capture
            }
            // Encountered another piece (not self):
            if (curr_position != position && (*board_)[curr_position.x][curr_position.y]->GetColor() != PieceColor::NONE)
                break;
            if (curr_position == position && passed_corner_cnt == 4)  // Has gone back
                break;
        }
    }
    return false;
}

SurakartaDirectionStraight SurakartaMovablityUtil::GetDirectionCapture(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const {
    const auto position = piece.GetPosition();
    const auto position_to = piece_to.GetPosition();
    for (const auto start_direction : SurakartaDirectionStraightList) {
        auto curr_position = position;
        auto curr_direction = start_direction;
        int passed_corner_cnt = 0;
        while (true) {
            const auto next_pair_opt = util_.Next(std::pair(curr_position, curr_direction));  // Try move a step
            if (next_pair_opt.has_value() == false)                                           // At corner
                break;
            auto [next_position, next_direction] = next_pair_opt.value();
            curr_position = next_position;
            if (next_direction != curr_direction)  // Has gone through a quarter
                passed_corner_cnt++;
            curr_direction = next_direction;
            if (curr_position == position_to) {  // Reached
                if (passed_corner_cnt > 0)
                    return start_direction;
                else
                    break;  // Must pass a corner before capture
            }
            // Encountered another piece (not self):
            if (curr_position != position && (*board_)[curr_position.x][curr_position.y]->GetColor() != PieceColor::NONE)
                break;
            if (curr_position == position && passed_corner_cnt == 4)  // Has gone back
                break;
        }
    }
    throw std::runtime_error("Invalid move");
}

bool SurakartaMovablityUtil::IsMovableTo(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const {
    return IsMovableToNoneCapture(piece, piece_to) || IsMovableToCapture(piece, piece_to);
}

std::optional<std::pair<SurakartaMove, bool>> SurakartaOnBoardUpdateUtil::ResolveMove() const {
    auto black_disappeared = std::vector<std::pair<int, SurakartaPositionWithId>>();
    for (unsigned i = 0; i < id_position_list_black_->size(); i++) {
        const auto pos = (*id_position_list_black_)[i];
        if ((*board_)[pos.x][pos.y]->GetColor() != PieceColor::BLACK) {
            black_disappeared.push_back(std::make_pair(i, pos));
        }
    }
    auto black_appeared = std::vector<SurakartaPosition>();
    for (int i = 0; i < board_->board_size_; i++) {
        for (int j = 0; j < board_->board_size_; j++) {
            if ((*board_)[i][j]->GetColor() == PieceColor::BLACK) {
                bool exist_before = false;
                for (auto& pos : *id_position_list_black_) {
                    if (pos.x == i && pos.y == j) {
                        exist_before = true;
                        break;
                    }
                }
                if (!exist_before) {
                    black_appeared.push_back(SurakartaPosition{i, j});
                }
            }
        }
    }
    auto white_disappeared = std::vector<std::pair<int, SurakartaPositionWithId>>();
    for (unsigned i = 0; i < id_position_list_white_->size(); i++) {
        const auto pos = (*id_position_list_white_)[i];
        if ((*board_)[pos.x][pos.y]->GetColor() != PieceColor::WHITE) {
            white_disappeared.push_back(std::make_pair(i, pos));
        }
    }
    auto white_appeared = std::vector<SurakartaPosition>();
    for (int i = 0; i < board_->board_size_; i++) {
        for (int j = 0; j < board_->board_size_; j++) {
            if ((*board_)[i][j]->GetColor() == PieceColor::WHITE) {
                bool exist_before = false;
                for (auto& pos : *id_position_list_white_) {
                    if (pos.x == i && pos.y == j) {
                        exist_before = true;
                        break;
                    }
                }
                if (!exist_before) {
                    white_appeared.push_back(SurakartaPosition{i, j});
                }
            }
        }
    }
    if (black_appeared.size() > black_disappeared.size())
        throw std::runtime_error("black_appeared.size() > black_disappeared.size()");
    if (black_disappeared.size() > 1)
        throw std::runtime_error("black moved more than 1 piece");
    if (white_appeared.size() > white_disappeared.size())
        throw std::runtime_error("white_appeared.size() > white_disappeared.size()");
    if (white_disappeared.size() > 1)
        throw std::runtime_error("white moved more than 1 piece");
    // Now:
    // black_appeared.size() <= black_disappeared.size() <= 1
    // white_appeared.size() <= white_disappeared.size() <= 1
    if (black_appeared.size() == 1 && white_appeared.size() == 1)
        throw std::runtime_error("both black and white moved");
    if (black_appeared.size() == 1) {
        if (white_disappeared.size() == 0)  // black non-capture
            return std::make_pair(SurakartaMove(black_disappeared[0].second, black_appeared[0], PieceColor::BLACK), false);
        else  // black capture
            return std::make_pair(SurakartaMove(black_disappeared[0].second, black_appeared[0], PieceColor::BLACK), true);
    }
    if (white_appeared.size() == 1) {
        if (black_disappeared.size() == 0)  // white non-capture
            return std::make_pair(SurakartaMove(white_disappeared[0].second, white_appeared[0], PieceColor::WHITE), false);
        else  // white capture
            return std::make_pair(SurakartaMove(white_disappeared[0].second, white_appeared[0], PieceColor::WHITE), true);
    }
    // Now: black_appeared.size() == 0 && white_appeared.size() == 0
    if (black_disappeared.size() == 0 && white_disappeared.size() == 0)
        return std::nullopt;  // no move
    throw std::runtime_error("piece disappeared for no reason");
}

void SurakartaOnBoardUpdateUtil::Update(const SurakartaMove& move) const {
    const auto curr_list = move.player == PieceColor::BLACK ? id_position_list_black_ : id_position_list_white_;
    const auto oppo_list = move.player == PieceColor::BLACK ? id_position_list_white_ : id_position_list_black_;
    bool updated = false;
    for (auto& pos : *curr_list) {
        if (pos.x == move.from.x && pos.y == move.from.y) {
            pos.x = move.to.x;
            pos.y = move.to.y;
            updated = true;
            break;
        }
    }
    if (updated == false)
        throw std::runtime_error("Cannot find piece to update!");
    for (unsigned i = 0; i < oppo_list->size(); i++) {
        if (oppo_list->at(i).x == move.to.x && oppo_list->at(i).y == move.to.y) {
            oppo_list->erase(oppo_list->begin() + i);
            break;
        }
    }
}

SurakartaMoveTrace SurakartaOnBoardUpdateUtil::GetTrace(const SurakartaMove& move, bool is_capture_move) const {
    const auto id_position_list = move.player == PieceColor::BLACK ? id_position_list_black_ : id_position_list_white_;
    const auto guard1 = SurakartaTemporarilyChangeColorGuardUtil(board_, move.from, move.player);
    const auto guard2 = is_capture_move
                            ? SurakartaTemporarilyChangeColorGuardUtil(board_, move.to, ReverseColor(move.player))
                            : SurakartaTemporarilyChangeColorGuardUtil(board_, move.to, PieceColor::NONE);
    const auto position = move.from;
    const auto position_to = move.to;
    if ((*board_)[move.to.x][move.to.y]->GetColor() == PieceColor::NONE) {
        const auto direction = movability_util_.GetDirectionNoneCapture(*(*board_)[position.x][position.y], *(*board_)[position_to.x][position_to.y]);
        return trace_generate_util_.GenerateNoneCaptured(position, direction);
    } else {
        // good direction
        const auto start_direction = movability_util_.GetDirectionCapture(*(*board_)[position.x][position.y], *(*board_)[position_to.x][position_to.y]);
        return trace_generate_util_.GenerateCaptured(position, start_direction);
    }
}

SurakartaMoveTrace SurakartaTraceGenerateUtil::GenerateNoneCaptured(SurakartaPosition from, SurakartaDirection start_direction) const {
    const auto id_position_list = (*board_)[from.x][from.y]->GetColor() == PieceColor::BLACK ? id_position_list_black_ : id_position_list_white_;
    auto trace = SurakartaMoveTrace();
    trace.is_capture = false;
    trace.moved_piece.id = -1;
    for (unsigned i = 0; i < id_position_list->size(); i++) {
        if ((*id_position_list)[i].x == from.x && (*id_position_list)[i].y == from.y) {
            trace.moved_piece.id = (*id_position_list)[i].id;
            trace.moved_piece.x = from.x;
            trace.moved_piece.y = from.y;
            break;
        }
    }
    if (trace.moved_piece.id == -1)
        throw std::runtime_error("piece not found within id_position_list");
    trace.captured_piece.id = -1;
    trace.path = std::make_shared<std::vector<SurakartaMovePathFragment>>();
    const auto position_to = from + start_direction;
    trace.path->push_back(SurakartaMovePathFragment(from.x, from.y, position_to.first, position_to.second));
    return trace;
}

SurakartaMoveTrace SurakartaTraceGenerateUtil::GenerateCaptured(SurakartaPosition from, SurakartaDirectionStraight start_direction) const {
    const auto curr_list = (*board_)[from.x][from.y]->GetColor() == PieceColor::BLACK ? id_position_list_black_ : id_position_list_white_;
    const auto oppo_list = (*board_)[from.x][from.y]->GetColor() == PieceColor::BLACK ? id_position_list_white_ : id_position_list_black_;
    auto trace = SurakartaMoveTrace();
    trace.is_capture = true;
    trace.moved_piece.id = -1;
    for (unsigned i = 0; i < curr_list->size(); i++) {
        if ((*curr_list)[i].x == from.x && (*curr_list)[i].y == from.y) {
            trace.moved_piece.id = (*curr_list)[i].id;
            trace.moved_piece.x = from.x;
            trace.moved_piece.y = from.y;
            break;
        }
    }
    if (trace.moved_piece.id == -1)
        throw std::runtime_error("piece not found within id_position_list");
    trace.path = std::make_shared<std::vector<SurakartaMovePathFragment>>();
    auto curr_position = from;
    auto curr_direction = start_direction;
    int passed_corner_cnt = 0;
    while (true) {
        const auto fragment = move_util_.PathFragment(std::pair(curr_position, curr_direction));
        if (fragment.has_value() == false)
            throw std::runtime_error("not a valid capture move");
        trace.path->push_back(fragment.value());
        const auto next_pair_opt = move_util_.Next(std::pair(curr_position, curr_direction));  // Try move a step
        if (next_pair_opt.has_value() == false)                                                // At corner
            throw std::logic_error("impossible");
        auto [next_position, next_direction] = next_pair_opt.value();
        curr_position = next_position;
        if (next_direction != curr_direction)  // Has gone through a quarter
            passed_corner_cnt++;
        curr_direction = next_direction;
        if (curr_position != from && (*board_)[curr_position.x][curr_position.y]->GetColor() != PieceColor::NONE) {  // Reached
            if (passed_corner_cnt > 0)
                break;
            else
                throw std::runtime_error("not a valid capture move");  // Must pass a corner before capture
        }
        // Encountered another piece (not self):
        if (curr_position == from && passed_corner_cnt == 4)  // Has gone back
            throw std::runtime_error("not a valid capture move");
    }
    const auto position_to = curr_position;
    trace.captured_piece.id = -1;
    for (unsigned i = 0; i < oppo_list->size(); i++) {
        if ((*oppo_list)[i].x == position_to.x && (*oppo_list)[i].y == position_to.y) {
            trace.captured_piece.id = (*oppo_list)[i].id;
            trace.captured_piece.x = position_to.x;
            trace.captured_piece.y = position_to.y;
            break;
        }
    }
    if (trace.captured_piece.id == -1)
        throw std::runtime_error("piece not found within id_position_list");
    return trace;
}

SurakartaInitPositionListsUtil::PositionLists SurakartaInitPositionListsUtil::InitPositionList() const {
    int id = 0;
    auto black = std::make_unique<std::vector<SurakartaPositionWithId>>();
    for (int x = 0; x < board_->board_size_; x++) {
        for (int y = 0; y < board_->board_size_; y++) {
            if ((*board_)[x][y]->GetColor() == PieceColor::BLACK) {
                black->push_back(SurakartaPositionWithId(x, y, id++));
            }
        }
    }
    auto white = std::make_unique<std::vector<SurakartaPositionWithId>>();
    for (int x = 0; x < board_->board_size_; x++) {
        for (int y = 0; y < board_->board_size_; y++) {
            if ((*board_)[x][y]->GetColor() == PieceColor::WHITE) {
                white->push_back(SurakartaPositionWithId(x, y, id++));
            }
        }
    }
    return PositionLists{.black_list = std::move(black), .white_list = std::move(white)};
}
