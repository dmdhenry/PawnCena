#include <iostream>
#include "chess/game.h"
using std::cout, std::endl;

int main() {
    Color result = play_game(true, true);
    if (result == WHITE) {
        cout << "White wins!" << endl; 
    } else if (result == BLACK) {
        cout << "Black wins!" << endl;
    } else {
        cout << "Draw!" << endl;
    }

    return 0;
}
