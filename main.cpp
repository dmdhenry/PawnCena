#include <iostream>
#include "chess/gui.h"
#include "chess/game.h"
#include "testing/test_cases.h"
#include "chess/utils.h"

void test() {
    run_all_test_cases();
}

void play() {

    // Start GUI
    initialize_gui();
    
    bool white_real = true;
    bool black_real = false;

    // Display board and run game loop.
    Color result = play_game(white_real, black_real);
    if (result == WHITE) {
        std::cout << "White wins!" << std::endl; 
    } else if (result == BLACK) {
        std::cout << "Black wins!" << std::endl;
    } else {
        std::cout << "Draw!" << std::endl;
    }

    // End GUI
    close_gui();
}

int main() {
    // test();
    reset_debug_log();
    play();
    return 0;
}
