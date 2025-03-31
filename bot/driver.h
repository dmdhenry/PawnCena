#ifndef BOT_DRIVER_H
#define BOT_DRIVER_H
#include "../chess/board.h"

class Bot {
private:
    int max_depth;
    double material_weight;
    double king_safety_weight;
    
    double evaluate_move(Board& board, Move& move, Color player, int depth, double alpha, double beta);

public:
    Bot();
    Bot(int max_depth);
    Bot(int max_depth, double material_weight, double king_safety_weight);

    Move request_move(Board& board, Color player);
};

#endif
