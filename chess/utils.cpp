#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
using std::cout, std::endl;

void debug_log(const std::string& message) {
    std::ofstream logFile("debug_log.txt", std::ios_base::app);
    if (logFile) {
        logFile << message << "\n";
    }
}

void reset_debug_log() {
    std::ifstream file("deubg_log.txt");
    if (file.good()) {
        std::remove("debug_log.txt");
    }
}
