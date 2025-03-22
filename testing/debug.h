#ifndef CALLTRACKER_H
#define CALLTRACKER_H
#include <unordered_map>
#include <string>
#include <chrono>

class CallTracker {
public:
    // Records a call with a given argument.
    static void recordCall(const std::string& arg);

    // Prints the call count and total time for a specific argument.
    static void printStats(const std::string& arg);

private:
    
    // Map storing for each argument a pair {call count, total elapsed time}
    inline static std::unordered_map<std::string, std::pair<int, double>> stats;
    inline static std::chrono::high_resolution_clock::time_point lastCall = std::chrono::high_resolution_clock::now();
    inline static bool firstCall = true;

};

#endif // CALLTRACKER_H
