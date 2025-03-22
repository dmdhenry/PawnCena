#include "debug.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <chrono>

// Records a call with a given argument.
// Updates the call count and adds the time gap (in seconds) from the last call.
void CallTracker::recordCall(const std::string& arg) {
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    double delta = 0.0;
    if (!firstCall) {
        delta = duration<double>(now - lastCall).count();
    }
    firstCall = false;
    lastCall = now;

    auto& entry = stats[arg];
    entry.first++;         // Increment call count
    entry.second += delta; // Add elapsed time
}

// Prints the call count and total time for a specific argument.
void CallTracker::printStats(const std::string& arg) {
    auto it = stats.find(arg);
    if (it != stats.end()) {
        std::cout << "Argument: " << arg 
                  << " | Count: " << it->second.first 
                  << " | Total Time: " << it->second.second 
                  << " seconds | Runs per second: " << it->second.first / it->second.second << std::endl;
    } else {
        std::cout << "No stats for argument: " << arg << "\n";
    }
}
