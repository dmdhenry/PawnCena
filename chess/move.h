#ifndef MOVE_H
#define MOVE_H
#include <string>

class Move {
private:
    std::string move_str;

public:
    Move();
    Move(const std::string& move);
    
    void set_move(const std::string& move);
    std::string get_move() const;
};

#endif
