#ifndef BOARD_H
#define BOARD_H
#include "move.h"
#include "game.h"
#include <cstdint>

enum Piece : uint8_t {
    EMPTY,

    WHITE_PAWN,
    WHITE_ROOK, 
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_QUEEN,
    WHITE_KING,
    
    BLACK_PAWN,
    BLACK_ROOK,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_QUEEN,
    BLACK_KING,
};

class Board {
private:
    Piece state[64];
    Piece get_piece(int file, int rank);
    
    public:
    Board();
    
    void display() const;
    bool is_legal_move(const Move& move, Color player);
    void update_move(const Move& move);
    bool pinned_move(Color player, int src_index, int dst_index);
};

void print_piece(const Piece piece);

#endif
