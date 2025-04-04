#ifndef GAME_H
#define GAME_H
#include "board.h"
#include <string>
#include <cstdint>

enum Color : uint8_t {
    WHITE,
    BLACK,
    DRAW
};

Color play_game(bool white_real, bool black_real);

void play_move(Board& board, Color player, bool is_real);

Move request_bot_move(Board& board, Color player);

bool is_checkmated(Board& board, Color player);
bool is_stalemated(Board& board, Color player);
bool fifty_move_rule_draw(Board& board);
bool threefold_repetition_draw(Board& board);

#endif
