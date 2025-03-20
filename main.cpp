#include <iostream>
#include "chess/game.h"
#include "chess/testing/test_cases.h"
using std::cout, std::endl;

void test() {
    run_all_test_cases();
}

void play() {
    Color result = play_game(true, true);
    if (result == WHITE) {
        cout << "White wins!" << endl; 
    } else if (result == BLACK) {
        cout << "Black wins!" << endl;
    } else {
        cout << "Draw!" << endl;
    }
}

int main() {
    // test();
    play();
    return 0;
}
