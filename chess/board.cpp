#include "board.h"
#include "utils.h"
#include "move.h"
#include "game.h"
#include <iostream>
using std::cout, std::endl;

void Board::display() const {
    clear_terminal();

    cout << "  abcdefgh" << endl;
    for (int rank = 7; rank >= 0; rank--) {
        cout << rank+1 << " ";
        for (int file = 0; file < 8; file++) {
            int index = (rank*8) + file;
            print_piece(state[index]);
        }
        cout << " " << rank+1 << endl;
    }
    cout << "  abcdefgh" << endl;
}

bool Board::is_legal_move(const Move& move, Color player) {

    // Only accept:
    // - simple 4-character moves like "e2e4"
    // - "oo" for kingside castling
    // - "ooo" for queenside castling
    // - "e7e8pQ" for promoting (B,R,N,Q) [implicit promotions are queens]

    std::string notation = move.get_move();
    if (notation == "oo") {
        return is_kingside_castle_legal(player);
    }

    if (notation == "ooo") {
        return is_queenside_castle_legal(player);
    }

    bool valid_move = is_real_move(notation, player);
    if (!valid_move) {
        return false;
    }

    int src_file = notation[0] - 'a';
    int src_rank = notation[1] - '1';
    int dst_file = notation[2] - 'a';
    int dst_rank = notation[3] - '1';
    int src_index = src_rank * 8 + src_file;
    int dst_index = dst_rank * 8 + dst_file;
    Piece piece = state[src_index];

    switch (piece) {

        // Pawn
        case WHITE_PAWN:
        case BLACK_PAWN: 
            return is_legal_pawn_move(notation, player, src_index, dst_index);

        // Knight
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            return is_legal_knight_move(player, src_index, dst_index);

        // Bishop
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            return is_legal_diagonal_move(player, src_index, dst_index);

        // Rook
        case WHITE_ROOK:
        case BLACK_ROOK: 
            return is_legal_straight_move(player, src_index, dst_index);

        // Queen
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            return is_legal_diagonal_move(player, src_index, dst_index) || is_legal_straight_move(player, src_index, dst_index);

        // King
        case WHITE_KING:
        case BLACK_KING:
            return is_legal_king_move(player, src_index, dst_index);
            
        default:
            return false;
    
    }
    return false;
}


bool Board::is_real_move(std::string notation, Color player) {
    // Check if the notation is valid and the move is a real move (make sure the move exists, not necessarily valid)

    // Ensure notation is valid
    if (notation.length() != 4 && notation.length() != 6) {
        return false;
    }
    
    // Ensure pawn promotion notation is valid
    if (notation.length() == 6 && (notation[4] != 'p' || 
            (notation[5] != 'B' && notation[5] != 'R' && notation[5] != 'N' && notation[5] != 'Q'))) {
        return false;
    }

    int src_file = notation[0] - 'a';
    int src_rank = notation[1] - '1';
    int dst_file = notation[2] - 'a';
    int dst_rank = notation[3] - '1';

    // Ensure move is not out of bounds
    if(src_file < 0 || src_file > 7 || dst_file < 0 || dst_file > 7 ||
       src_rank < 0 || src_rank > 7 || dst_rank < 0 || dst_rank > 7) {
        return false;
    }

    int src_index = src_rank * 8 + src_file;
    int dst_index = dst_rank * 8 + dst_file;
    int piece = state[src_index];

    // Ensure we are moving an actual piece
    if (piece == EMPTY) {
        return false;
    }
    
    // Determine piece color and ensure we are moving our own piece
    bool isWhite = (piece >= WHITE_PAWN && piece <= WHITE_KING);
    if ((isWhite && player != WHITE) || (!isWhite && player != BLACK)) {
        return false;
    }
    
    // Cannot capture a friendly piece.
    int dst_piece = state[dst_index];
    if (dst_piece != EMPTY) {
        bool dst_isWhite = (dst_piece >= WHITE_PAWN && dst_piece <= WHITE_KING);
        if (isWhite == dst_isWhite)
            return false;
    }

    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;

    // Ensure we are actually moving a piece.
    if (file_diff == 0 && rank_diff == 0) {
        return false;
    }

    return true;
}

bool Board::is_legal_pawn_move(std::string notation, Color player, int src_index, int dst_index) {
    // Assumes move has already been checked to be real

    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int direction = (player == WHITE) ? 1 : -1;
    int start_rank = (player == WHITE) ? 1 : 6;
    int back_rank = (player == WHITE) ? 7 : 0;

    // One square forward (explicit promotion)
    if (notation.length() == 6 && file_diff == 0 && rank_diff == direction && 
            state[dst_index] == EMPTY && dst_rank == back_rank) {
        return (!pinned_move(player, src_index, dst_index));
    }

    // Diagonal capture (explicit promotion)
    if (notation.length() == 6 && abs_file_diff == 1 && rank_diff == direction && 
            state[dst_index] != EMPTY && dst_rank == back_rank) {
        return (!pinned_move(player, src_index, dst_index));
    }

    // If explicit promotion conditions do not pass, explicit promotion move is invalid!
    if (notation.length() == 6) {
        return false;
    }

    // One square forward (no capture)
    if (file_diff == 0 && rank_diff == direction && state[dst_index] == EMPTY) {
        return (!pinned_move(player, src_index, dst_index));
    }
    
    // Two squares forward from starting position
    int intermediate_index = src_index + direction * 8;
    if (file_diff == 0 && rank_diff == 2 * direction && src_rank == start_rank &&
            state[intermediate_index] == EMPTY && state[dst_index] == EMPTY) {
        return (!pinned_move(player, src_index, dst_index));
    }
    
    // Diagonal capture
    if (abs_file_diff == 1 && rank_diff == direction && state[dst_index] != EMPTY) {
        return (!pinned_move(player, src_index, dst_index));
    }

    // En passant capture
    if (abs_file_diff == 1 && rank_diff == direction && dst_index == en_passant_square) {
        return (!pinned_move(player, src_index, dst_index));
    }
    
    return false;
}

bool Board::is_legal_knight_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int abs_rank_diff = std::abs(rank_diff);

    return ((abs_file_diff == 2 && abs_rank_diff == 1) || (abs_file_diff == 1 && abs_rank_diff == 2)) &&
        (!pinned_move(player, src_index, dst_index));
}

bool Board::is_legal_diagonal_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int abs_rank_diff = std::abs(rank_diff);

    if (abs_file_diff == abs_rank_diff && abs_file_diff > 0) {
        int step_file = (file_diff > 0) ? 1 : -1;
        int step_rank = (rank_diff > 0) ? 1 : -1;
        int current_file = src_file + step_file;
        int current_rank = src_rank + step_rank;
        while (current_file != dst_file && current_rank != dst_rank) {
            if (state[current_rank * 8 + current_file] != EMPTY) {
                return false;
            }
            current_file += step_file;
            current_rank += step_rank;
        }
        return (!pinned_move(player, src_index, dst_index));
    }
    return false;
}

bool Board::is_legal_straight_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;

    if ((file_diff == 0 && rank_diff != 0) || (rank_diff == 0 && file_diff != 0)) {
        int step_file = (file_diff == 0) ? 0 : (file_diff > 0 ? 1 : -1);
        int step_rank = (rank_diff == 0) ? 0 : (rank_diff > 0 ? 1 : -1);
        int current_file = src_file + step_file;
        int current_rank = src_rank + step_rank;
        while (current_file != dst_file || current_rank != dst_rank) {
            if (state[current_rank * 8 + current_file] != EMPTY) {
                return false;
            }
            current_file += step_file;
            current_rank += step_rank;
        }
        return (!pinned_move(player, src_index, dst_index));
    }
    return false;
}

bool Board::is_legal_king_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int abs_rank_diff = std::abs(rank_diff);
    return (abs_file_diff <= 1 && abs_rank_diff <= 1) && (!pinned_move(player, src_index, dst_index));
}

bool Board::is_queenside_castle_legal(Color player) {
    // Ensure neither the king nor the rook has previously moved
    if (player == WHITE && !white_can_ooo) {
        return false;
    }

    if (player == BLACK && !black_can_ooo) {
        return false;
    }

    // Ensure there are no pieces between the king and the rook
    if (player == WHITE && (state[1] != EMPTY || state[2] != EMPTY || state[3] != EMPTY)) {
        return false;
    }

    if (player == BLACK && (state[57] != EMPTY || state[58] != EMPTY  || state[59] != EMPTY)) {
        return false;
    }

    // Ensure the king is not castling out of, through, or into check.
    if (player == WHITE && (is_square_under_attack(2, 0, WHITE) || is_square_under_attack(3, 0, WHITE) 
                            || is_square_under_attack(4, 0, WHITE))) {
        return false;
    }

    if (player == BLACK && (is_square_under_attack(2, 7, BLACK) || is_square_under_attack(3, 7, BLACK) 
                            || is_square_under_attack(4, 7, BLACK))) {
        return false;
    }

    return true;
}

bool Board::is_kingside_castle_legal(Color player) {
    // Ensure neither the king nor the rook has previously moved
    if (player == WHITE && !white_can_oo) {
        return false;
    }

    if (player == BLACK && !black_can_oo) {
        return false;
    }

    // Ensure there are no pieces between the king and the rook
    if (player == WHITE && (state[5] != EMPTY || state[6] != EMPTY)) {
        return false;
    }

    if (player == BLACK && (state[61] != EMPTY || state[62] != EMPTY)) {
        return false;
    }

    // Ensure the king is not castling out of, through, or into check.
    if (player == WHITE && (is_square_under_attack(4, 0, WHITE) || is_square_under_attack(5, 0, WHITE) 
                            || is_square_under_attack(6, 0, WHITE))) {
        return false;
    }

    if (player == BLACK && (is_square_under_attack(4, 7, BLACK) || is_square_under_attack(5, 7, BLACK) 
                            || is_square_under_attack(6, 7, BLACK))) {
        return false;
    }

    return true;
}

Piece Board::get_piece(int file, int rank) {
    if (rank < 0 || rank >= 8 || file < 0 || file >= 8) {
        return EMPTY;
    }

    int index = rank * 8 + file;
    return state[index];
}

// Get lowest index on board that holds 'piece'. Returns -1 if piece isn't found.
int Board::get_lowest_piece_index(Piece piece) {
    for (int i = 0; i < 64; i++) {
        if (state[i] == piece) {
            return i;
        }
    }
    return -1;
}

// Check if player's piece at file/rank is under attack from an opposing king.
bool Board::is_under_attack_from_king(int file, int rank, Color player) {
    Piece attacker = BLACK_KING;
    if (player == BLACK) {
        attacker = WHITE_KING;
    }

    for (int file_offset = -1; file_offset <= 1; file_offset++) {
        for (int rank_offset = -1; rank_offset <= 1; rank_offset++) {
            if (file_offset == 0 && rank_offset == 0) {
                continue;
            }
            
            if (get_piece(file + file_offset, rank + rank_offset) == attacker) {
                return true;
            }
        }
    }

    return false;
}


// Check if player's piece at file/rank is under attack from an opposing pawn.
bool Board::is_under_attack_from_pawn(int file, int rank, Color player) {
    Piece attacker = BLACK_PAWN;
    int rank_offset = 1;
    if (player == BLACK) {
        attacker = WHITE_PAWN;
        rank_offset = -1;
    }

    if (get_piece(file + 1, rank + rank_offset) == attacker) {
        return true;
    }
    
    if (get_piece(file - 1, rank + rank_offset) == attacker) {
        return true;
    }

    return false;
}

// Check if player's piece at file/rank is under attack from an opposing knight.
bool Board::is_under_attack_from_knight(int file, int rank, Color player) {
    Piece attacker = BLACK_KNIGHT;
    if (player == BLACK) {
        attacker = WHITE_KNIGHT;
    }

    for (int file_offset : {-1, 1, -2, 2}) {
        for (int rank_offset : {-1, 1, -2, 2}) {
            if (get_piece(file + file_offset, rank + rank_offset) == attacker) {
                return true;
            }
        }
    }

    return false;
}

// Check if player's piece at file/rank is under attack from an opposing diagonal piece (bishop/queen).
bool Board::is_under_attack_from_diagonal(int file, int rank, Color player) {
    Piece bishop_attacker = BLACK_BISHOP;
    Piece queen_attacker = BLACK_QUEEN;
    if (player == BLACK) {
        bishop_attacker = WHITE_BISHOP;
        queen_attacker = WHITE_QUEEN;
    }
    
    // Check all 4 diagonal directions.
    for (int df : {1, -1}) {
        for (int dr : {1, -1}) {
            int f = file + df;
            int r = rank + dr;
            while (f >= 0 && f < 8 && r >= 0 && r < 8) {
                Piece piece = get_piece(f, r);
                if (piece != EMPTY) {
                    if (piece == bishop_attacker || piece == queen_attacker) {
                        return true;
                    }
                    break; // blocked by another piece
                }
                f += df;
                r += dr;
            }
        }
    }
    
    return false;
}

// Check if player's piece at file/rank is under attack from an opposing straight piece (rook/queen).
bool Board::is_under_attack_from_straight(int file, int rank, Color player) {
    Piece rook_attacker = BLACK_ROOK;
    Piece queen_attacker = BLACK_QUEEN;
    if (player == BLACK) {
        rook_attacker = WHITE_ROOK;
        queen_attacker = WHITE_QUEEN;
    }
    
    // Check horizontal directions.
    for (int d : {1, -1}) {
        int f = file + d;
        int r = rank;
        while (f >= 0 && f < 8) {
            Piece piece = get_piece(f, r);
            if (piece != EMPTY) {
                if (piece == rook_attacker || piece == queen_attacker) {
                    return true;
                }
                break;
            }
            f += d;
        }
    }
    
    // Check vertical directions.
    for (int d : {1, -1}) {
        int f = file;
        int r = rank + d;
        while (r >= 0 && r < 8) {
            Piece piece = get_piece(f, r);
            if (piece != EMPTY) {
                if (piece == rook_attacker || piece == queen_attacker) {
                    return true;
                }
                break;
            }
            r += d;
        }
    }
    
    return false;
}

bool Board::is_square_under_attack(int file, int rank, Color player) {

    // Check if the square is under attack from a king
    bool under_attack = is_under_attack_from_king(file, rank, player);

    // Check if the square is under attack from a pawn
    if (!under_attack) {
        under_attack = is_under_attack_from_pawn(file, rank, player);
    }

    // Check if the square is under attack from a knight
    if (!under_attack) {
        under_attack = is_under_attack_from_knight(file, rank, player);
    }

    // Check if the square is under attack from a bishop/queen
    if (!under_attack) {
        under_attack = is_under_attack_from_diagonal(file, rank, player);
    }

    // Check if the square is under attack from a rook/queen
    if (!under_attack) {
        under_attack = is_under_attack_from_straight(file, rank, player);
    }

    return under_attack;
}

bool Board::pinned_move(Color player, int src_index, int dst_index) {
    // Assumes that move is completely valid and possible!
    // Checks to see if after moving, the king would be in check, making it an invalid move.

    // Temporarily make move to see board after move
    Piece temp = state[dst_index];
    state[dst_index] = state[src_index];
    state[src_index] = EMPTY;

    // Determine index of king
    int king_index = 0;
    if (player == WHITE) {
        king_index = get_lowest_piece_index(WHITE_KING);
    } else {
        king_index = get_lowest_piece_index(BLACK_KING);
    }

    // Check if king_index is currently under attack by opposing pieces
    int king_file = king_index % 8;
    int king_rank = king_index / 8;
    bool under_attack = is_square_under_attack(king_file, king_rank, player);

    // Undo temporary move
    state[src_index] = state[dst_index];
    state[dst_index] = temp;

    return under_attack;
}

// Assumes move legality has already been checked!
void Board::update_move(const Move& move, Color player) {
    // Every move cancels current valid en passant moves... unless this move reveals en passant! (later in function)
    en_passant_square = -1;
    
    // Move Piece and Update History
    std::string notation = move.get_move();

    if (notation == "oo") {
        castle_kingside(player);
    } else if (notation == "ooo") {
        castle_queenside(player);
    } else {
        // Normal piece move
        int src_file = notation[0] - 'a';
        int src_rank = notation[1] - '1';
        int dst_file = notation[2] - 'a';
        int dst_rank = notation[3] - '1';
        int src_index = src_rank * 8 + src_file;
        int dst_index = dst_rank * 8 + dst_file;

        Piece piece = state[src_index];
        state[dst_index] = piece;
        state[src_index] = EMPTY;
        
        // Castling
        handle_castling_history(piece, src_index);

        // En passant
        handle_en_passant_history(piece, src_rank, dst_rank, src_file);
        
        // Promotion
        handle_promotion(piece, notation, dst_rank, dst_index);
    }
}


void Board::castle_kingside(Color player) {
    if (player == WHITE) {
        state[7] = EMPTY; // old rook square
        state[4] = EMPTY; // old king square
        state[6] = WHITE_KING;
        state[5] = WHITE_ROOK;
        white_can_oo = false;
        white_can_ooo = false;
    } else {
        state[63] = EMPTY; // old rook square
        state[60] = EMPTY; // old king square
        state[62] = BLACK_KING;
        state[61] = BLACK_ROOK;
        black_can_oo = false;
        black_can_ooo = false;
    }
}

void Board::castle_queenside(Color player) {
    if (player == WHITE) {
        state[0] = EMPTY; // old rook square
        state[4] = EMPTY; // old king square
        state[2] = WHITE_KING;
        state[3] = WHITE_ROOK;
        white_can_oo = false;
        white_can_ooo = false;
    } else {
        state[56] = EMPTY; // old rook square
        state[60] = EMPTY; // old king square
        state[58] = BLACK_KING;
        state[59] = BLACK_ROOK;
        black_can_oo = false;
        black_can_ooo = false;
    }
}

void Board::handle_castling_history(Piece piece, int src_index) {
    if (piece == WHITE_KING) {
        white_can_oo = false;
        white_can_ooo = false;
    } else if (piece == BLACK_KING) {
        black_can_oo = false;
        black_can_ooo = false;
    } else if (piece == WHITE_ROOK && src_index == 0) {
        white_can_ooo = false;
    } else if (piece == WHITE_ROOK && src_index == 7) {
        white_can_oo = false;
    } else if (piece == BLACK_ROOK && src_index == 56) {
        black_can_ooo = false;
    } else if (piece == BLACK_ROOK && src_index == 63) {
        black_can_oo = false;
    }
}

void Board::handle_en_passant_history(Piece piece, int src_rank, int dst_rank, int src_file) {
    if (piece == WHITE_PAWN && src_rank == 1 && dst_rank == 3) {
        en_passant_square = 16 + src_file;
    } else if (piece == BLACK_PAWN && src_rank == 6 && dst_rank == 4) {
        en_passant_square = 40 + src_file;
    }
}

void Board::handle_promotion(Piece piece, std::string notation, int dst_rank, int dst_index) {

    if (piece == WHITE_PAWN && dst_rank == 7) {

        Piece promotion_piece = WHITE_QUEEN;
        if (notation.length() == 6) {
            char piece_char = notation[5];
            if (piece_char == 'B') {
                promotion_piece = WHITE_BISHOP;
            } else if (piece_char == 'R') {
                promotion_piece = WHITE_ROOK;
            } else if (piece_char == 'N') {
                promotion_piece = WHITE_KNIGHT;
            }
        }

        state[dst_index] = promotion_piece;

    } else if (piece == BLACK_PAWN && dst_rank == 0) {

        Piece promotion_piece = BLACK_QUEEN;
        if (notation.length() == 6) {
            char piece_char = notation[5];
            if (piece_char == 'B') {
                promotion_piece = BLACK_BISHOP;
            } else if (piece_char == 'R') {
                promotion_piece = BLACK_ROOK;
            } else if (piece_char == 'N') {
                promotion_piece = BLACK_KNIGHT;
            }
        }

        state[dst_index] = promotion_piece;
    }
}

Board::Board() {
    // 'state' is structured as follows: A1 [0] --> H1 [7], A2 [8] --> H2 [15], etc.
    state[0] = WHITE_ROOK;
    state[1] = WHITE_KNIGHT;
    state[2] = WHITE_BISHOP;
    state[3] = WHITE_QUEEN;
    state[4] = WHITE_KING;
    state[5] = WHITE_BISHOP;
    state[6] = WHITE_KNIGHT;
    state[7] = WHITE_ROOK;

    for (int i = 8; i < 16; i++) {
        state[i] = WHITE_PAWN;
    }

    for (int i = 16; i < 48; i++) {
        state[i] = EMPTY;
    }

    for (int i = 48; i < 56; i++) {
        state[i] = BLACK_PAWN;
    }

    state[56] = BLACK_ROOK;
    state[57] = BLACK_KNIGHT;
    state[58] = BLACK_BISHOP;
    state[59] = BLACK_QUEEN;
    state[60] = BLACK_KING;
    state[61] = BLACK_BISHOP;
    state[62] = BLACK_KNIGHT;
    state[63] = BLACK_ROOK;

    // History variables
    black_can_oo = true;
    black_can_ooo = true;
    white_can_oo = true;
    white_can_ooo = true;
    en_passant_square = -1;
}

void print_piece(const Piece piece) {
    switch(piece) {
        case BLACK_PAWN:     
            cout << "♙";
            break;
        case BLACK_ROOK:     
            cout << "♖";
            break;
        case BLACK_KNIGHT:   
            cout << "♘";
            break;
        case BLACK_BISHOP:   
            cout << "♗";
            break;
        case BLACK_QUEEN:    
            cout << "♕";
            break;
        case BLACK_KING:     
            cout << "♔";
            break;
        case WHITE_PAWN:     
            cout << "♟";
            break;
        case WHITE_ROOK:     
            cout << "♜";
            break;
        case WHITE_KNIGHT:   
            cout << "♞";
            break;
        case WHITE_BISHOP:   
            cout << "♝";
            break;
        case WHITE_QUEEN:    
            cout << "♛";
            break;
        case WHITE_KING:     
            cout << "♚";
            break;
        default:             
            cout << ".";
            break;
    }
}
