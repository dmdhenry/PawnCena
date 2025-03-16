#include "board.h"
#include "utils.h"
#include "move.h"
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

bool Board::is_legal_move(const Move& move) const {
    return true;
}

void Board::update_move(const Move& move) {
    // board_state = move.get_move();
    // history.push_back(move);
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
        case EMPTY:          
            cout << " ";
            break;
        case WHITE_PAWN:     
            cout << "♙";
            break;
        case WHITE_ROOK:     
            cout << "♖";
            break;
        case WHITE_KNIGHT:   
            cout << "♘";
            break;
        case WHITE_BISHOP:   
            cout << "♗";
            break;
        case WHITE_QUEEN:    
            cout << "♕";
            break;
        case WHITE_KING:     
            cout << "♔";
            break;
        case BLACK_PAWN:     
            cout << "♟";
            break;
        case BLACK_ROOK:     
            cout << "♜";
            break;
        case BLACK_KNIGHT:   
            cout << "♞";
            break;
        case BLACK_BISHOP:   
            cout << "♝";
            break;
        case BLACK_QUEEN:    
            cout << "♛";
            break;
        case BLACK_KING:     
            cout << "♚";
            break;
        default:             
            cout << " ";
            break;
    }
}
