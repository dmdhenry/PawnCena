#include "../bot/driver.h"
#include <iostream>
#include <string>
#include "utils.h"
#include "board.h"
#include "game.h"
#include "move.h"
#include "gui.h"
using std::cout, std::endl;

Color play_game(bool white_real, bool black_real) {

    Board board;
    board.display();

    while (true) {

        /////////////////////////////////////////
        ///////////////// WHITE /////////////////
        /////////////////////////////////////////
        play_move(board, WHITE, white_real);

        // See if black is checkmated
        if (is_checkmated(board, BLACK)) {
            board.display();
            return WHITE;
        }
        
        // See if a stalemate exists
        if (is_stalemated(board, BLACK)) {
            board.display();
            return DRAW;
        }

        // Check for fifty move rule draws
        if (fifty_move_rule_draw(board)) {
            board.display();
            return DRAW;
        }
    
        // Check for threefold repetition rule draws
        if (threefold_repetition_draw(board)) {
            board.display();
            return DRAW;
        } 

        /////////////////////////////////////////
        ///////////////// BLACK /////////////////
        /////////////////////////////////////////
        play_move(board, BLACK, black_real);

        // See if white is checkmated
        if (is_checkmated(board, WHITE)) {
            board.display();
            return BLACK;
        }
        
        // See if a stalemate exists
        if (is_stalemated(board, WHITE)) {
            board.display();
            return DRAW;
        }

        // Check for fifty move rule draws
        if (fifty_move_rule_draw(board)) {
            board.display();
            return DRAW;
        }
    
        // Check for threefold repetition rule draws
        if (threefold_repetition_draw(board)) {
            board.display();
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
        move.set_move(request_player_move(board, player));
    } else {
        move = request_bot_move(board, player);
    }

    // Update the board
    board.update_move(move, player);
    
    if (is_real) {
        board.display();
    }
}

Move request_bot_move(Board& board, Color player) {
    Bot bot(5, 20.0, 1.0);

    std::string text = "Pawn Cena is selecting move...";
    write_gui_box(text);

    Move move = bot.request_move(board, player);

    std::string player_color = (player == WHITE) ? "white" : "black";
    text = "Pawn Cena (" + player_color + ") plays " + move.get_move() + ".";
    write_gui_box(text);

    return move;
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
