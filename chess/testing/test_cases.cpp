#include "test_cases.h"
#include "../board.h"
#include "../game.h"
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

void run_all_test_cases() {
    // When adding a new test case, ensure that white and black alternate every move!
    run_test_case(1, test1()); // beefy case
    run_test_case(2, test2()); // test implicit queen promotion
    run_test_case(3, test3()); // test explicit queen promotion 
    run_test_case(4, test4()); // test bishop promotion 
    run_test_case(5, test5()); // test rook promotion
    run_test_case(6, test6()); // test knight promotion
    run_test_case(7, test7()); // invalid notation case
}
