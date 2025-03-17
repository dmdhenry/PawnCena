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
    bool black_can_oo;
    bool black_can_ooo;
    bool white_can_oo;
    bool white_can_ooo;
    int en_passant_square;

    Piece get_piece(int file, int rank);
    int get_lowest_piece_index(Piece piece);

    bool pinned_move(Color player, int src_index, int dst_index);
    bool is_square_under_attack(int file, int rank, Color player);

    bool is_under_attack_from_king(int file, int rank, Color player);
    bool is_under_attack_from_pawn(int file, int rank, Color player);
    bool is_under_attack_from_knight(int file, int rank, Color player);
    bool is_under_attack_from_straight(int file, int rank, Color player);
    bool is_under_attack_from_diagonal(int file, int rank, Color player);

    bool is_kingside_castle_legal(Color player);
    bool is_queenside_castle_legal(Color player);

public:
    Board();
    
    void display() const;
    bool is_legal_move(const Move& move, Color player);
    void update_move(const Move& move, Color player);
};

void print_piece(const Piece piece);

#endif
