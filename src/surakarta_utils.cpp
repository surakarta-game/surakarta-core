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

std::unique_ptr<std::vector<SurakartaPosition>> SurakartaGetAllLegalTargetUtil::GetAllLegalTarget(const SurakartaPiece& piece) const {
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
                const auto targets = util_.GetAllLegalTarget(*piece);
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

bool SurakartaMovablityUtil::IsMovableTo(const SurakartaPiece& piece, const SurakartaPiece& piece_to) const {
    return IsMovableToNoneCapture(piece, piece_to) || IsMovableToCapture(piece, piece_to);
}
