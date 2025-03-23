#include "../testing/debug.h"
#include "../chess/board.h"
#include "../chess/move.h"
#include "../chess/game.h"
#include "driver.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <chrono>

Bot::Bot() : max_depth(5),
             material_weight(1.0), 
             mobility_weight(1.0), 
             king_safety_weight(1.0), 
             pawn_structure_weight(1.0) {}

Bot::Bot(int max_depth) : max_depth(max_depth), 
                          material_weight(1.0), 
                          mobility_weight(1.0), 
                          king_safety_weight(1.0), 
                          pawn_structure_weight(1.0) {}

Bot::Bot(int max_depth, double material_weight, double mobility_weight, double king_safety_weight, double pawn_structure_weight) : 
                          max_depth(max_depth),                           
                          material_weight(material_weight), 
                          mobility_weight(mobility_weight), 
                          king_safety_weight(king_safety_weight), 
                          pawn_structure_weight(pawn_structure_weight) {}

int moves_evaluted = 0;

Move Bot::request_move(Board& board, Color player) {
    vector<Move> legal_moves = board.get_legal_moves(player);
    Move best_move;

    ////////////////////////////////////////// DEBUG INFO //////////////////////////////////////////
    // auto DBG_timer_start = std::chrono::high_resolution_clock::now();
    ////////////////////////////////////////// DEBUG INFO //////////////////////////////////////////

    if (player == WHITE) { // WHITE: maximize the score
        double best_move_score = -std::numeric_limits<double>::infinity();
        for (Move& move : legal_moves) {
            double move_score = evaluate_move(board, move, player, 1, 
                                                -std::numeric_limits<double>::infinity(), 
                                                std::numeric_limits<double>::infinity());
            if (move_score > best_move_score) {
                best_move_score = move_score;
                best_move = move;
            }
        }
    } else { // BLACK: minimize the score
        double best_move_score = std::numeric_limits<double>::infinity();
        for (Move& move : legal_moves) {
            double move_score = evaluate_move(board, move, player, 1, 
                                                -std::numeric_limits<double>::infinity(), 
                                                std::numeric_limits<double>::infinity());
            if (move_score < best_move_score) {
                best_move_score = move_score;
                best_move = move;
            }
        }
    }

    ////////////////////////////////////////// DEBUG INFO //////////////////////////////////////////

    // std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;

    // auto DBG_timer_end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = DBG_timer_end - DBG_timer_start;
    // std::cout << moves_evaluted << " moves evaluted in " << elapsed.count() << " seconds." << std::endl;
    // std::cout << moves_evaluted / elapsed.count() << " moves per second!" << std::endl;

    // std::cout << std::endl << std::endl;
    
    // CallTracker::printStats("score");
    // CallTracker::printStats("legal");
    // std::cout << std::endl;
    
    ////////////////////////////////////////// DEBUG INFO //////////////////////////////////////////


    return best_move;
}

double Bot::evaluate_move(Board& board, Move& move, Color player, int depth, double alpha, double beta) {

    // moves_evaluted++;  // DEBUG

    // Create a board after the move is played
    Board board_after_move = board.inspect_move(move, player);
    Color player_to_move = player == WHITE ? BLACK : WHITE;

    // Terminal condition: reached max search depth or no moves available
    if (depth >= max_depth) {
        // CallTracker::recordCall("start");
        double score = board_after_move.score_position(player_to_move, depth, material_weight, mobility_weight, king_safety_weight, pawn_structure_weight);
        // CallTracker::recordCall("score");
        return score;
    }

    // Get all legal moves
    // CallTracker::recordCall("start");
    std::vector<Move> legal_moves = board_after_move.get_legal_moves(player_to_move);
    // CallTracker::recordCall("legal");

    // If no legal moves, score position!
    if (legal_moves.empty()) {
        return board_after_move.score_position(player_to_move, depth, material_weight, mobility_weight, king_safety_weight, pawn_structure_weight);
    }

    // If player is WHITE, we assume WHITE is maximizing and BLACK is minimizing
    if (player_to_move == WHITE) {  
        double max_eval = -std::numeric_limits<double>::infinity();
        for (const Move& next_move : legal_moves) {
            double eval = evaluate_move(board_after_move, 
                                          const_cast<Move&>(next_move), 
                                          WHITE, 
                                          depth + 1, 
                                          alpha, 
                                          beta);
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break;  // beta cut-off
            }
        }
        return max_eval;
    } 
    // Otherwise, for BLACK, we minimize
    else {
        double min_eval = std::numeric_limits<double>::infinity();
        for (const Move& next_move : legal_moves) {
            double eval = evaluate_move(board_after_move, 
                                          const_cast<Move&>(next_move), 
                                          BLACK, 
                                          depth + 1, 
                                          alpha, 
                                          beta);
            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break;  // alpha cut-off
            }
        }
        return min_eval;
    }
}
