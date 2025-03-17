#include "board.h"
#include "utils.h"
#include "move.h"
#include "game.h"
#include <iostream>
using std::cout, std::endl;

void Board::display() const {
    clear_terminal();

    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int index = (rank*8) + file;
            print_piece(state[index]);
        }
        cout << endl;
    }
    cout << endl;
}

void Board::update_move(const Move& move) {
    // TODO: move piece and update history
}

Piece Board::get_piece(int file, int rank) {
    if (rank < 0 || rank >= 8 || file < 0 || file >= 8) {
        return EMPTY;
    }

    int index = rank * 8 + file;
    return state[index];
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
        for (int i = 0; i < 64; i++) {
            if (state[i] == WHITE_KING) {
                king_index = i;
                break;
            }
        }
    } else {
        for (int i = 0; i < 64; i++) {
            if (state[i] == BLACK_KING) {
                king_index = i;
                break;
            }
        }
    }

    // Check if king_index is currently under attack by opposing pieces
    bool under_attack = false;
    int king_file = king_index % 8;
    int king_rank = king_index / 8;

    // TODO Check if king is currently under attack from...

    // king?

    // pawn?

    // bishop/queen?

    // rook/queen?

    // knight?

    // Undo temporary move
    state[src_index] = state[dst_index];
    state[dst_index] = temp;

    return under_attack;
}

bool Board::is_legal_move(const Move& move, Color player) {

    // Only accept simple 4-character moves like "e2e4"
    std::string notation = move.get_move();
    if (notation.length() != 4 && notation != "oo" && notation != "ooo") {
        return false;
    }

    if (notation == "oo") {
        return false; // TODO: DO CASTLING!
    }

    if (notation == "ooo") {
        return false; // TODO: DO CASTLING!
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
        bool dst_isWhite = (piece >= WHITE_PAWN && piece <= WHITE_KING);
        if (isWhite == dst_isWhite)
            return false;
    }

    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int abs_rank_diff = std::abs(rank_diff);

    if (file_diff == 0 && rank_diff == 0) {
        return false;
    }

    switch (piece) {

        // Pawn
        case WHITE_PAWN:
        case BLACK_PAWN: {
            // TODO: use history for en passant!

            int direction = (piece == WHITE_PAWN) ? 1 : -1;
            int start_rank = (piece == WHITE_PAWN) ? 1 : 6;

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
            
            return false;
        }

        // Knight
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            return ((abs_file_diff == 2 && abs_rank_diff == 1) || (abs_file_diff == 1 && abs_rank_diff == 2)) &&
                     (!pinned_move(player, src_index, dst_index));

        // Bishop
        case WHITE_BISHOP:
        case BLACK_BISHOP: {

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

        // Rook
        case WHITE_ROOK:
        case BLACK_ROOK: {

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

        // Queen
        case WHITE_QUEEN:
        case BLACK_QUEEN: {

            // Diagonal move like bishop
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

            // Straight move like rook
            if((file_diff == 0 && rank_diff != 0) || (rank_diff == 0 && file_diff != 0)) {
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

        // King
        case WHITE_KING:
        case BLACK_KING:
            return (abs_file_diff <= 1 && abs_rank_diff <= 1) && (!pinned_move(player, src_index, dst_index));
            
        default:
            return false;
    
    }
    return false;
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
