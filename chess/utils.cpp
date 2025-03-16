#include "utils.h"
#include <iostream>
using std::cout, std::endl;

void clear_terminal() {
    for (int i = 0; i < 100; i++) {
        std::cout << std::endl;
    }
}
