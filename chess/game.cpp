#include "game.h"
#include "board.h"
#include "move.h"
#include <iostream>
#include <string>
using std::cout, std::endl;

Color play_game(bool white_real, bool black_real) {
    
    Board board;

    while (true) {

        /////////////////////////////////////////
        ///////////////// WHITE /////////////////
        /////////////////////////////////////////
        play_move(board, WHITE, white_real);

        // See if black is checkmated
        if (is_checkmated(board, BLACK)) {
            return WHITE;
        }
        
        // See if a stalemate exists
        if (is_stalemated(board, BLACK)) {
            return DRAW;
        }

        // Check for fifty move rule draws
        if (fifty_move_rule_draw(board)) {
            return DRAW;
        }
    
        // Check for threefold repetition rule draws
        if (threefold_repetition_draw(board)) {
            return DRAW;
        } 

        /////////////////////////////////////////
        ///////////////// BLACK /////////////////
        /////////////////////////////////////////
        play_move(board, BLACK, black_real);

        // See if white is checkmated
        if (is_checkmated(board, WHITE)) {
            return BLACK;
        }
        
        // See if a stalemate exists
        if (is_stalemated(board, WHITE)) {
            return DRAW;
        }

        // Check for fifty move rule draws
        if (fifty_move_rule_draw(board)) {
            return DRAW;
        }
    
        // Check for threefold repetition rule draws
        if (threefold_repetition_draw(board)) {
            return DRAW;
        } 

    }

    return DRAW;
}

// NOTE: This function assumes at least 1 legal move can be played by 'player'!
void play_move(Board& board, Color player, bool is_real) {

    // Request the move to be played
    Move move;
    if (is_real) {
        board.display();

        bool trying_again = false;
        while (true) {
            move.set_move(request_player_move(trying_again));
            if (board.is_legal_move(move, player)) {
                break;
            }

            trying_again = true;
            board.display();
        }
    } else {
        move.set_move(request_bot_move(board, player));
    }

    // Update the board
    board.update_move(move, player);
}

std::string request_player_move(bool trying_again) {
    std::string move;
    if (trying_again) {
        std::cout << "TRY AGAIN: ";
    } else {
        std::cout << "Input a move: ";
    }
    std::cin >> move;
    return move;
}

std::string request_bot_move(Board& board, Color player) {
    std::cerr << "NOT YET IMPLEMENTED!" << std::endl;
    return "";
}

bool is_checkmated(Board& board, Color player) {
    return board.is_checked(player) && board.has_no_legal_moves(player);
}

bool is_stalemated(Board& board, Color player) {
    return !board.is_checked(player) && board.has_no_legal_moves(player);
}

bool fifty_move_rule_draw(Board& board) {
    return board.is_fifty_move_rule_draw();
}

bool threefold_repetition_draw(Board& board) {
    return board.is_threefold_repetition_draw();
}
