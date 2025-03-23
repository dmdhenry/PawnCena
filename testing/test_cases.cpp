#include "test_cases.h"
#include "../chess/board.h"
#include "../chess/game.h"
#include <iostream>
#include <string>
using std::cout, std::endl;

void run_test_case(int test_index, bool passed) {
    if (passed) {
        cout << "Passed test case " << test_index << "." << endl;
    } else {
        cout << "FAILED test case " << test_index << "!" << endl;
    }
}

// Play a specific move and return whether or not the move is legal!
bool play_dbg_move(Board& board, bool& whiteTurn, std::string notation) {
    
    Color player = WHITE;
    if (!whiteTurn) {
        player = BLACK;
    }

    // Request the move to be played
    Move move;
    move.set_move(notation);
    if (!board.is_legal_move(move, player)) {
        return false;
    }

    // Update the board if it is a legal move
    board.update_move(move, player);

    // Update player if it is a legal move
    whiteTurn = !whiteTurn;

    return true;
}

bool valid(Board& board, bool& whiteTurn, std::string notation) {
    return play_dbg_move(board, whiteTurn, notation);
}

bool INVALID(Board& board, bool& whiteTurn, std::string notation) {
    return !play_dbg_move(board, whiteTurn, notation);
}

bool test1() {
    Board b; // board
    bool t = true; // turn tracker
    
    if (!valid(b, t, "e2e4")) { return false; }
    if (!valid(b, t, "d7d5")) { return false; }
    if (!valid(b, t, "e4d5")) { return false; }
    if (!valid(b, t, "d8d5")) { return false; }
                                                    if (!INVALID(b, t, "d1d5")) { return false; }
    if (!valid(b, t, "b1c3")) { return false; }
    if (!valid(b, t, "d5e4")) { return false; }
                                                    if (!INVALID(b, t, "d5e4")) { return false; }
                                                    if (!INVALID(b, t, "c3d5")) { return false; } // king is checked, must block!
    if (!valid(b, t, "c3e2")) { return false; }
                                                    if (!INVALID(b, t, "c8b7")) { return false; } // black cant capture black
                                                    if (!INVALID(b, t, "c8b6")) { return false; } // bishop cant go through pawn
    if (!valid(b, t, "c8f5")) { return false; }
                                                    if (!INVALID(b, t, "e2d4")) { return false; } // pinned knight
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "b8c6")) { return false; }
                                                    if (!INVALID(b, t, "ooo")) { return false; } // stuff between OOO
                                                    if (!INVALID(b, t, "oo")) { return false; } // stuff between OO
    if (!valid(b, t, "h1g1")) { return false; }
    if (!valid(b, t, "ooo")) { return false; }
    if (!valid(b, t, "g1h1")) { return false; }
    if (!valid(b, t, "d8d2")) { return false; }
    if (!valid(b, t, "g2g3")) { return false; }
    if (!valid(b, t, "a7a6")) { return false; }
    if (!valid(b, t, "f1g2")) { return false; }
    if (!valid(b, t, "a6a5")) { return false; }
                                                    if (!INVALID(b, t, "oo")) { return false; } // already moved castle
    if (!valid(b, t, "e1f1")) { return false; }
    if (!valid(b, t, "d2d1")) { return false; }
                                                    if (!INVALID(b, t, "f1g2")) { return false; } // cant capture own piece
                                                    if (!INVALID(b, t, "f1g1")) { return false; } // cant move from check into check
                                                    if (!INVALID(b, t, "e2d1")) { return false; } // invalid knight move
                                                    if (!INVALID(b, t, "f3e2")) { return false; } // invalid knight move

    return true;
}


bool test2() {
    Board b; // board
    bool t = true; // turn tracker
    
    // SETUP TO PROMOTE IN THE CORNER
    if (!valid(b, t, "e2e4")) { return false; }
    if (!valid(b, t, "d7d5")) { return false; }
    if (!valid(b, t, "e4d5")) { return false; }
    if (!valid(b, t, "c7c6")) { return false; }
    if (!valid(b, t, "d5c6")) { return false; }
    if (!valid(b, t, "b8d7")) { return false; }
    if (!valid(b, t, "c6b7")) { return false; }
    if (!valid(b, t, "a7a5")) { return false; }
    
    if (!valid(b, t, "b7a8")) { return false; } // PROMOTION TO QUEEN
    if (!valid(b, t, "d7f6")) { return false; }
                                                    if (!INVALID(b, t, "a8b6")) { return false; } // not a knight!
    if (!valid(b, t, "a8a6")) { return false; } // straight MOVE
    if (!valid(b, t, "f6d7")) { return false; }
    if (!valid(b, t, "a6d3")) { return false; } // diagonal MOVE

    return true;
}

bool test3() {
    Board b; // board
    bool t = true; // turn tracker
    
    // SETUP TO PROMOTE IN THE CORNER
    if (!valid(b, t, "e2e4")) { return false; }
    if (!valid(b, t, "d7d5")) { return false; }
    if (!valid(b, t, "e4d5")) { return false; }
    if (!valid(b, t, "c7c6")) { return false; }
    if (!valid(b, t, "d5c6")) { return false; }
    if (!valid(b, t, "b8d7")) { return false; }
    if (!valid(b, t, "c6b7")) { return false; }
    if (!valid(b, t, "a7a5")) { return false; }
    
    if (!valid(b, t, "b7a8pQ")) { return false; } // PROMOTION TO QUEEN
    if (!valid(b, t, "d7f6")) { return false; }
                                                    if (!INVALID(b, t, "a8b6")) { return false; } // not a knight!
    if (!valid(b, t, "a8a6")) { return false; } // straight MOVE
    if (!valid(b, t, "f6d7")) { return false; }
    if (!valid(b, t, "a6d3")) { return false; } // diagonal MOVE

    return true;
}

bool test4() {
    Board b; // board
    bool t = true; // turn tracker
    
    // SETUP TO PROMOTE IN THE CORNER
    if (!valid(b, t, "e2e4")) { return false; }
    if (!valid(b, t, "d7d5")) { return false; }
    if (!valid(b, t, "e4d5")) { return false; }
    if (!valid(b, t, "c7c6")) { return false; }
    if (!valid(b, t, "d5c6")) { return false; }
    if (!valid(b, t, "b8d7")) { return false; }
    if (!valid(b, t, "c6b7")) { return false; }
    if (!valid(b, t, "a7a5")) { return false; }
    
    if (!valid(b, t, "b7a8pB")) { return false; } // PROMOTION TO BISHOP
    if (!valid(b, t, "d7f6")) { return false; }
                                                    if (!INVALID(b, t, "a8a6")) { return false; } // straight MOVE
                                                    if (!INVALID(b, t, "a8b6")) { return false; } // not a knight!
    if (!valid(b, t, "a8f3")) { return false; } // diagonal MOVE

    return true;
}

bool test5() {
    Board b; // board
    bool t = true; // turn tracker
    
    // SETUP TO PROMOTE IN THE CORNER
    if (!valid(b, t, "e2e4")) { return false; }
    if (!valid(b, t, "d7d5")) { return false; }
    if (!valid(b, t, "e4d5")) { return false; }
    if (!valid(b, t, "c7c6")) { return false; }
    if (!valid(b, t, "d5c6")) { return false; }
    if (!valid(b, t, "b8d7")) { return false; }
    if (!valid(b, t, "c6b7")) { return false; }
    if (!valid(b, t, "a7a5")) { return false; }
    
    if (!valid(b, t, "b7a8pR")) { return false; } // PROMOTION TO ROOK
    if (!valid(b, t, "d7f6")) { return false; }
                                                    if (!INVALID(b, t, "a8b6")) { return false; } // not a knight!
    if (!valid(b, t, "a8a6")) { return false; } // straight MOVE
    if (!valid(b, t, "f6d7")) { return false; }
                                                    if (!INVALID(b, t, "a6d3")) { return false; } // diagonal MOVE

    return true;
}

bool test6() {
    Board b; // board
    bool t = true; // turn tracker
    
    // SETUP TO PROMOTE IN THE CORNER
    if (!valid(b, t, "e2e4")) { return false; }
    if (!valid(b, t, "d7d5")) { return false; }
    if (!valid(b, t, "e4d5")) { return false; }
    if (!valid(b, t, "c7c6")) { return false; }
    if (!valid(b, t, "d5c6")) { return false; }
    if (!valid(b, t, "b8d7")) { return false; }
    if (!valid(b, t, "c6b7")) { return false; }
    if (!valid(b, t, "a7a5")) { return false; }
    
    if (!valid(b, t, "b7a8pN")) { return false; } // PROMOTION TO KNIGHT
    if (!valid(b, t, "d7f6")) { return false; }       
                                                    if (!INVALID(b, t, "a8a6")) { return false; } // straight MOVE
    if (!valid(b, t, "a8b6")) { return false; }   // knight MOVE
    if (!valid(b, t, "f6d7")) { return false; }       
                                                    if (!INVALID(b, t, "b6e3")) { return false; } // diagonal MOVE

    return true;
}

bool test7() {
    Board b; // board
    bool t = true; // turn tracker
    
    if (!INVALID(b, t, "a1a1")) { return false; } // dont move piece
    if (!INVALID(b, t, "a7a5")) { return false; } // white moves black piece
    if (!INVALID(b, t, "a2a5")) { return false; } // pawn 3 moves
    if (!INVALID(b, t, "a1a0")) { return false; } // a9  ...
    if (!INVALID(b, t, "a1a9")) { return false; }
    if (!INVALID(b, t, "a1i1")) { return false; }
    if (!INVALID(b, t, ".1a1")) { return false; }
    if (!INVALID(b, t, "a2a3a")) { return false; }
    if (!INVALID(b, t, "a2a4p")) { return false; }
    if (!INVALID(b, t, "a2a4pQ")) { return false; }
    if (!INVALID(b, t, "a7a8pQ")) { return false; }
    if (!INVALID(b, t, "a2a8pQ")) { return false; }

    return true;
}

bool test8() {
    Board b; // board
    bool t = true; // turn tracker
    
    if (!valid(b, t, "e2e3")) { return false; }
    if (!valid(b, t, "e7e6")) { return false; }
    if (!valid(b, t, "e1e2")) { return false; }
    if (!valid(b, t, "e8e7")) { return false; }
    if (!valid(b, t, "e2e1")) { return false; }
    if (!valid(b, t, "e7e8")) { return false; }
    if (!valid(b, t, "e1e2")) { return false; }
    if (!valid(b, t, "e8e7")) { return false; }
    if (!valid(b, t, "e2e1")) { return false; }
    if (!valid(b, t, "e7e8")) { return false; }

    if (b.is_threefold_repetition_draw()) {
        return false; // only 2 fold so far
    }

    if (!valid(b, t, "e1e2")) { return false; }
    if (!valid(b, t, "e8e7")) { return false; }
    if (!valid(b, t, "e2e1")) { return false; }
    if (!valid(b, t, "e7e8")) { return false; }

    if (!b.is_threefold_repetition_draw()) {
        return false; // is 3 fold now
    }

    return true;
}

bool test9() {
    Board b; // board
    bool t = true; // turn tracker
    
    if (!valid(b, t, "e2e3")) { return false; }
    if (!valid(b, t, "f7f6")) { return false; }
    if (!valid(b, t, "f2f4")) { return false; }

    if (is_checkmated(b, WHITE)) {
        return false; // white never checkmated
    }

    if (is_checkmated(b, BLACK)) {
        return false; // not yet checkmated
    }
    
    if (!valid(b, t, "g7g5")) { return false; }
    if (!valid(b, t, "d1h5")) { return false; }

    if (!is_checkmated(b, BLACK)) {
        return false; // black is checkmated now
    }

    if (is_checkmated(b, WHITE)) {
        return false; // white never checkmated 
    }

    return true;
}

bool test10() {
    Board b; // board
    bool t = true; // turn tracker
    
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }

    if (b.is_threefold_repetition_draw()) {
        return false; // not a threefold repetition
    }

    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }
    if (!valid(b, t, "f6d5")) { return false; }
    if (!valid(b, t, "d4f3")) { return false; }
    if (!valid(b, t, "d5f6")) { return false; }
    if (!valid(b, t, "f3g1")) { return false; }
    if (!valid(b, t, "f6g8")) { return false; }
    if (!valid(b, t, "g1f3")) { return false; }
    if (!valid(b, t, "g8f6")) { return false; }
    if (!valid(b, t, "f3d4")) { return false; }

    if (b.is_fifty_move_rule_draw()) {
        return false; // 49.5/50 ... not yet!
    }

    if (!valid(b, t, "f6d5")) { return false; }

    if (!b.is_fifty_move_rule_draw()) {
        return false; // 50/50... is a draw
    }

    return true;
}

bool test11() {
    Board b; // board
    bool t = true; // turn tracker
    
    if (!valid(b, t, "e2e3")) { return false; } 
    if (!valid(b, t, "a7a5")) { return false; } 
    if (!valid(b, t, "d1h5")) { return false; } 
    if (!valid(b, t, "a8a6")) { return false; } 
    if (!valid(b, t, "h5a5")) { return false; } 
    if (!valid(b, t, "h7h5")) { return false; } 
    if (!valid(b, t, "a5c7")) { return false; } 
    if (!valid(b, t, "a6h6")) { return false; } 
    if (!valid(b, t, "h2h4")) { return false; } 
    if (!valid(b, t, "f7f6")) { return false; } 
    if (!valid(b, t, "c7d7")) { return false; }
    if (!valid(b, t, "e8f7")) { return false; } 
    if (!valid(b, t, "d7b7")) { return false; } 
    if (!valid(b, t, "d8d3")) { return false; } 
    if (!valid(b, t, "b7b8")) { return false; } 
    if (!valid(b, t, "d3h7")) { return false; } 
    if (!valid(b, t, "b8c8")) { return false; } 
    if (!valid(b, t, "f7g6")) { return false; }

    if (is_stalemated(b, BLACK)) { 
        return false; // not yet
    }

    if (!valid(b, t, "c8e6")) { return false; }

    if (!is_stalemated(b, BLACK)) { 
        return false; // black has no legal moves
    }

    return true;
}

bool test12() {
    Board b; // board
    bool t = true; // turn tracker
    
    if (!valid(b, t, "e2e4")) { return false; } 
    if (!valid(b, t, "h7h5")) { return false; } 
    if (!valid(b, t, "e4e5")) { return false; } 
    if (!valid(b, t, "f7f5")) { return false; } 
    if (!valid(b, t, "e5f6")) { return false; } 

    if (b.get_piece(5, 4) != EMPTY) {
        return false;
    }

    return true;
}

bool test13() {
    Board b("rnbqkbnr/ppppp2p/5p2/6p1/5P2/4P3/PPPP2PP/RNBQKBNR w KQ - 0 1"); // board
    bool t = true; // turn tracker
    
    // White should find the 1-move checkmate!
    Move move = request_bot_move(b, WHITE);
    if (move.get_move() != "d1h5") {
        return false;
    }

    // Ensure that move actually checkmates properly
    if (!valid(b, t, move.get_move())) { return false; } 
    
    if (!is_checkmated(b, BLACK)) {
        return false;
    }

    return true;
}

bool test14() {
    Board b("q3k3/8/8/8/8/7q/8/3K4 w - - 0 1"); // board
    bool t = false; // turn tracker --> false means START WITH BLACK!!!!!!!!!!!!!
    
    // Black must find the 2-move checkmate!
    int max_moves = 2;
    for (int i = 0; i < max_moves; i++) {
        Move move = request_bot_move(b, BLACK);
        if (!valid(b, t, move.get_move())) { return false; } 

        if (is_checkmated(b, WHITE)) {
            return true;
        }

        move = request_bot_move(b, WHITE);
        if (!valid(b, t, move.get_move())) { return false; } 
    }

    return true;
}

void run_all_test_cases() {

    // GAME TEST CASES
    run_test_case(1, test1()); // beefy case
    run_test_case(2, test2()); // test implicit queen promotion
    run_test_case(3, test3()); // test explicit queen promotion 
    run_test_case(4, test4()); // test bishop promotion 
    run_test_case(5, test5()); // test rook promotion
    run_test_case(6, test6()); // test knight promotion
    run_test_case(7, test7()); // invalid notation case
    run_test_case(8, test8()); // threefold repetition draw
    run_test_case(9, test9()); // checkmate 
    run_test_case(10, test10()); // fifty move rule draw
    run_test_case(11, test11()); // stalemate
    run_test_case(12, test12()); // en passant

    // BOT TEST CASES
    run_test_case(13, test13()); // bot finds mate in 1
    run_test_case(14, test14()); // bot finds mate in 2
    
}
