#include "move.h"

Move::Move() : move_str("") {}
Move::Move(const std::string& move) : move_str(move) {}

void Move::set_move(const std::string& move) {
    move_str = move;
}

std::string Move::get_move() const {
    return move_str;
}
