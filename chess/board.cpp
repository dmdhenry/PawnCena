#include <sstream>
#include <cassert>
#include <cmath>
#include <limits>
#include "board.h"
#include "utils.h"
#include "move.h"
#include "game.h"
#include "gui.h"

void Board::display() const {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int index = (rank*8) + file;
            draw_square(state[index], file, rank);
        }
    }

    refresh_gui();
}

Board Board::inspect_move(Move& move, Color player) {
    Board new_board = *this;
    new_board.update_move(move, player);
    return new_board;
}

double Board::score_position(Color player_to_move, int depth, double material_weight, double king_safety_weight) {
    // Evaluate position without any recursion (for leaf nodes in bot)
    // Lower scores favor black, higher scores favor white
    
    // CHECKMATE
    if (is_checkmated(*this, player_to_move)) {
        if (player_to_move == WHITE) { return -1000000 + depth; }
        if (player_to_move == BLACK) { return 1000000 - depth; }
        assert(false); // should never reach this!
    }

    // DRAWS
    if (is_stalemated(*this, player_to_move)) { return 0.0; }
    if (threefold_repetition_draw(*this)) { return 0.0; }
    if (fifty_move_rule_draw(*this)) { return 0.0; }

    // BOARD EVALUATION
    double material     = calculate_raw_material_score();
    // double mobility     = calculate_mobility_difference();
    double kingSafety   = evaluate_king_safety();
    // double pawnStructure = evaluate_pawn_structure();

    return material * material_weight + kingSafety * king_safety_weight;// + pawnStructure * pawn_structure_weight + mobility * mobility_weight
}

double Board::calculate_mobility_difference() {
    vector<Move> white_moves = get_legal_moves(WHITE);
    vector<Move> black_moves = get_legal_moves(BLACK);
    return 1.0 * (white_moves.size() - black_moves.size());
}

double Board::evaluate_king_safety() {
    double score = 0.0;
    const double maxPenalty = 0.5;    // Maximum penalty when king is at center
    const double threshold  = 3.0;    // Distance below which penalty applies

    // Evaluate white king
    int white_king_index = get_lowest_piece_index(WHITE_KING);
    int white_file = white_king_index % 8;
    int white_rank = white_king_index / 8;
    double white_d = std::sqrt(std::pow(white_file - 3.5, 2) + std::pow(white_rank - 3.5, 2));
    double white_penalty = (white_d < threshold) ? maxPenalty * ((threshold - white_d) / threshold) : 0.0;
    score -= white_penalty;
    
    // Evaluate black king (penalty for black king in center is good for white)
    int black_king_index = get_lowest_piece_index(BLACK_KING);
    int black_file = black_king_index % 8;
    int black_rank = black_king_index / 8;
    double black_d = std::sqrt(std::pow(black_file - 3.5, 2) + std::pow(black_rank - 3.5, 2));
    double black_penalty = (black_d < threshold) ? maxPenalty * ((threshold - black_d) / threshold) : 0.0;
    score += black_penalty;
    
    return score;
}

double Board::evaluate_pawn_structure() {
    double score = 0.0;

    // Simple check for isolated pawns.
    for (int i = 0; i < 64; i++) {
        Piece piece = state[i];
        if (piece == WHITE_PAWN || piece == BLACK_PAWN) {
            int file = i % 8;
            bool has_neighbor = false;
            // Check adjacent files for a friendly pawn.
            for (int j = 0; j < 64; j++) {
                if (j == i) continue;
                int other_file = j % 8;
                if (std::abs(other_file - file) == 1 && state[j] == piece) {
                    has_neighbor = true;
                    break;
                }
            }

            // Penalize if no neighboring pawn.
            if (!has_neighbor) {
                if (piece == WHITE_PAWN)
                    score -= 0.2;
                else
                    score += 0.2;  // Isolated black pawn is bad for black, so bonus for white.
            }
        }
    }
    return score;
}

double Board::calculate_raw_material_score() {

    double material_score = 0.0;
    for (int i = 0; i < 64; i++) {
        switch(state[i]) {
            case EMPTY: break;
            case WHITE_PAWN: material_score += 1.0; break;
            case WHITE_ROOK: material_score += 5.0; break;
            case WHITE_KNIGHT: material_score += 3.0; break;
            case WHITE_BISHOP: material_score += 3.0; break;
            case WHITE_QUEEN: material_score += 9.0; break;
            case BLACK_PAWN: material_score -= 1.0; break;
            case BLACK_ROOK: material_score -= 5.0; break;
            case BLACK_KNIGHT: material_score -= 3.0; break;
            case BLACK_BISHOP: material_score -= 3.0; break;
            case BLACK_QUEEN: material_score -= 9.0; break;
            default: break;
        }
    }

    return material_score;
}

vector<Move> Board::get_legal_moves(Color player) {

    vector<Move> legal_moves;

    if (is_kingside_castle_legal(player)) {
        legal_moves.push_back(Move("oo"));
    }

    if (is_queenside_castle_legal(player)) {
        legal_moves.push_back(Move("ooo"));
    }

    for (int src_index = 0; src_index < 64; src_index++) {
        Piece piece = state[src_index];
        if (piece == EMPTY) {
            continue;
        }
        
        Color piece_color = piece >= WHITE_PAWN && piece <= WHITE_KING ? WHITE : BLACK;
        if (piece_color != player) {
            continue;
        }

        if (piece == WHITE_PAWN || piece == BLACK_PAWN) {
            append_all_legal_pawn_moves(legal_moves, src_index, player);
        } else if (piece == WHITE_ROOK || piece == BLACK_ROOK) {
            append_all_legal_rook_moves(legal_moves, src_index, player);
        } else if (piece == WHITE_KNIGHT || piece == BLACK_KNIGHT) {
            append_all_legal_knight_moves(legal_moves, src_index, player);
        } else if (piece == WHITE_BISHOP || piece == BLACK_BISHOP) {
            append_all_legal_bishop_moves(legal_moves, src_index, player);
        } else if (piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
            append_all_legal_queen_moves(legal_moves, src_index, player);
        } else if (piece == WHITE_KING || piece == BLACK_KING) {
            append_all_legal_king_moves(legal_moves, src_index, player);
        }
    }

    return legal_moves;
}

void Board::append_all_legal_pawn_moves(vector<Move>& legal_moves, int src_index, Color player) {
    // legal moves: (UNPINNED) 1 step forward, 2 step forward, left diagonal capture, right diagonal capture, promotions if on back rank!

    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int direction = (player == WHITE) ? 1 : -1;
    int start_rank = (player == WHITE) ? 1 : 6;
    int back_rank = (player == WHITE) ? 7 : 0;

    // One square forward
    int dst_index = ((src_rank+direction)*8) + src_file;
    if (state[dst_index] == EMPTY && !pinned_move(player, src_index, dst_index)) {
        legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
        
        // Backrank promotion
        if (src_rank+direction == back_rank) {
            legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index] + "pR"));
            legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index] + "pN"));
            legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index] + "pB"));
        }
    }

    // Left diagonal capture (including en passant and capture with promotion)
    if (src_file > 0) {
        int left_cap_idx = ((src_rank+direction)*8) + src_file - 1;
        if (state[left_cap_idx] != EMPTY) {
            bool captured_piece_is_white = (state[left_cap_idx] >= WHITE_PAWN && state[left_cap_idx] <= WHITE_KING);
            if ((((player == WHITE) && !captured_piece_is_white) || ((player == BLACK && captured_piece_is_white)) || en_passant_square == left_cap_idx)
                    && !pinned_move(player, src_index, dst_index)) {
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[left_cap_idx]));
            }

            // Backrank promotion
            if (src_rank+direction == back_rank) {
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[left_cap_idx] + "pR"));
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[left_cap_idx] + "pN"));
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[left_cap_idx] + "pB"));
            }
        }
    }

    // Right diagonal capture (including en passant and capture with promotion)
    if (src_file < 7) {
        int right_cap_idx = ((src_rank+direction)*8) + src_file + 1;
        if (state[right_cap_idx] != EMPTY) {
            bool captured_piece_is_white = (state[right_cap_idx] >= WHITE_PAWN && state[right_cap_idx] <= WHITE_KING);
            if ((((player == WHITE) && !captured_piece_is_white) || ((player == BLACK && captured_piece_is_white)) || en_passant_square == right_cap_idx)
                    && !pinned_move(player, src_index, dst_index)) {
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[right_cap_idx]));
            }

            // Backrank promotion
            if (src_rank+direction == back_rank) {
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[right_cap_idx] + "pR"));
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[right_cap_idx] + "pN"));
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[right_cap_idx] + "pB"));
            }
        }
    }

    // Two squares forward
    if (src_rank == start_rank) {
        int intermediate_index = ((start_rank+direction)*8) + src_file;
        int dst_index = ((start_rank+(direction*2))*8) + src_file;
        if (state[intermediate_index] == EMPTY && state[dst_index] == EMPTY && !pinned_move(player, src_index, dst_index)) {
            legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
        }
    }
}

void Board::append_all_legal_rook_moves(vector<Move>& legal_moves, int src_index, Color player) {
    // legal moves: (UNPINNED) any horizontal or vertical move until another piece is in the way 
    int src_rank = src_index / 8;
    int src_file = src_index % 8;
    
    // Check horizontal and vertical directions.
    for (auto deltas : std::initializer_list<std::pair<int,int>>{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}) {
        int h_delta = deltas.first;
        int f = src_file + h_delta;

        int v_delta = deltas.second;
        int r = src_rank + v_delta;
        
        while (f >= 0 && f < 8 && r >= 0 && r < 8) {
            int dst_index = r*8 + f;
            Piece piece = state[dst_index];
            if (piece == EMPTY && !pinned_move(player, src_index, dst_index)) {
                // Can move to any empty square when nothing is in between
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
            } else if (piece != EMPTY) {
                // Can capture opponent pieces when nothing is in between
                bool piece_is_white = (piece >= WHITE_PAWN && piece <= WHITE_KING);
                if ((((player == WHITE) && !piece_is_white) || ((player == BLACK) && piece_is_white))
                        && !pinned_move(player, src_index, dst_index)) {
                    legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
                }
                
                // Can't move past pieces!
                break;
            }

            f += h_delta;
            r += v_delta;
        }

    }
}

void Board::append_all_legal_knight_moves(vector<Move>& legal_moves, int src_index, Color player) {
    // legal moves: (UNPINNED) any L move
    int src_rank = src_index / 8;
    int src_file = src_index % 8;

    for (int file_offset : {-1, 1, -2, 2}) {
        for (int rank_offset : {-1, 1, -2, 2}) {
            if (abs(file_offset) == abs(rank_offset)) {
                continue;
            }

            int dst_rank = src_rank + rank_offset;
            int dst_file = src_file + file_offset;

            // Ensure dst_index is on the board
            if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8) {
                continue;
            }

            int dst_index = (dst_rank*8) + dst_file;
            bool dst_piece_is_white = (state[dst_index] >= WHITE_PAWN && state[dst_index] <= WHITE_KING);

            if ((state[dst_index] == EMPTY || (dst_piece_is_white && (player == BLACK)) || (!dst_piece_is_white && (player == WHITE)))
                    && (!pinned_move(player, src_index, dst_index))) {
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
            }
        }
    }
}

void Board::append_all_legal_bishop_moves(vector<Move>& legal_moves, int src_index, Color player) {
    // legal moves: (UNPINNED) any diagonal move until another piece is in the way 
    int src_rank = src_index / 8;
    int src_file = src_index % 8;
    
    // Check horizontal and vertical directions.
    for (auto deltas : std::initializer_list<std::pair<int,int>>{{1, 1}, {-1, -1}, {1, -1}, {-1, 1}}) {
        int h_delta = deltas.first;
        int f = src_file + h_delta;

        int v_delta = deltas.second;
        int r = src_rank + v_delta;
        
        while (f >= 0 && f < 8 && r >= 0 && r < 8) {
            int dst_index = r*8 + f;
            Piece piece = state[dst_index];
            if (piece == EMPTY && !pinned_move(player, src_index, dst_index)) {
                // Can move to any empty square when nothing is in between
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
            } else if (piece != EMPTY) {
                // Can capture opponent pieces when nothing is in between
                bool piece_is_white = (piece >= WHITE_PAWN && piece <= WHITE_KING);
                if ((((player == WHITE) && !piece_is_white) || ((player == BLACK) && piece_is_white))
                        && !pinned_move(player, src_index, dst_index)) {
                    legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
                }
                
                // Can't move past pieces!
                break;
            }
            
            f += h_delta;
            r += v_delta;
        }
    }
}

void Board::append_all_legal_queen_moves(vector<Move>& legal_moves, int src_index, Color player) {
    // legal moves: (UNPINNED) any diagonal/horizontal/vertical move until another piece is in the way 
    append_all_legal_rook_moves(legal_moves, src_index, player);
    append_all_legal_bishop_moves(legal_moves, src_index, player);
}

void Board::append_all_legal_king_moves(vector<Move>& legal_moves, int src_index, Color player) {
    // legal moves: 1 square any direction not into check

    int src_rank = src_index / 8;
    int src_file = src_index % 8;

    for (int file_offset = -1; file_offset <= 1; file_offset++) {
        for (int rank_offset = -1; rank_offset <= 1; rank_offset++) {
            if (file_offset == 0 && rank_offset == 0) {
                continue;
            }

            int dst_rank = src_rank + rank_offset;
            int dst_file = src_file + file_offset;

            if (dst_rank < 0 || dst_rank >= 8 || dst_file < 0 || dst_file >= 8) {
                continue;
            }
            
            int dst_index = dst_rank*8 + dst_file;
            Piece piece = state[dst_index];

            if (piece == EMPTY && !pinned_move(player, src_index, dst_index)) {
                legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
            }

            if (piece != EMPTY) {
                // Can capture opponent pieces when nothing is in between
                bool piece_is_white = (piece >= WHITE_PAWN && piece <= WHITE_KING);
                if ((((player == WHITE) && !piece_is_white) || ((player == BLACK) && piece_is_white))
                        && !pinned_move(player, src_index, dst_index)) {
                    legal_moves.push_back(Move(all_squares[src_index] + all_squares[dst_index]));
                }
            }

        }
    }
}

bool Board::has_no_legal_moves(Color player) {
    return get_legal_moves(player).empty();
}

bool Board::is_legal_move(const Move& move, Color player) {

    // Only accept:
    // - simple 4-character moves like "e2e4"
    // - "oo" for kingside castling
    // - "ooo" for queenside castling
    // - "e7e8pQ" for promoting (B,R,N,Q) [implicit promotions are queens]

    std::string notation = move.get_move();
    if (notation == "oo") {
        return is_kingside_castle_legal(player);
    }

    if (notation == "ooo") {
        return is_queenside_castle_legal(player);
    }

    bool valid_move = is_real_move(notation, player);
    if (!valid_move) {
        return false;
    }

    int src_file = notation[0] - 'a';
    int src_rank = notation[1] - '1';
    int dst_file = notation[2] - 'a';
    int dst_rank = notation[3] - '1';
    int src_index = src_rank * 8 + src_file;
    int dst_index = dst_rank * 8 + dst_file;
    Piece piece = state[src_index];

    switch (piece) {

        // Pawn
        case WHITE_PAWN:
        case BLACK_PAWN: 
            return is_legal_pawn_move(notation, player, src_index, dst_index);

        // Knight
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            return is_legal_knight_move(player, src_index, dst_index);

        // Bishop
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            return is_legal_diagonal_move(player, src_index, dst_index);

        // Rook
        case WHITE_ROOK:
        case BLACK_ROOK: 
            return is_legal_straight_move(player, src_index, dst_index);

        // Queen
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            return is_legal_diagonal_move(player, src_index, dst_index) || is_legal_straight_move(player, src_index, dst_index);

        // King
        case WHITE_KING:
        case BLACK_KING:
            return is_legal_king_move(player, src_index, dst_index);
            
        default:
            return false;
    
    }
    return false;
}


bool Board::is_real_move(std::string notation, Color player) {
    // Check if the notation is valid and the move is a real move (make sure the move exists, not necessarily valid)

    // Ensure notation is valid
    if (notation.length() != 4 && notation.length() != 6) {
        return false;
    }
    
    // Ensure pawn promotion notation is valid
    if (notation.length() == 6 && (notation[4] != 'p' || 
            (notation[5] != 'B' && notation[5] != 'R' && notation[5] != 'N' && notation[5] != 'Q'))) {
        return false;
    }

    int src_file = notation[0] - 'a';
    int src_rank = notation[1] - '1';
    int dst_file = notation[2] - 'a';
    int dst_rank = notation[3] - '1';

    // Ensure move is not out of bounds
    if(src_file < 0 || src_file > 7 || dst_file < 0 || dst_file > 7 ||
       src_rank < 0 || src_rank > 7 || dst_rank < 0 || dst_rank > 7) {
        return false;
    }

    int src_index = src_rank * 8 + src_file;
    int dst_index = dst_rank * 8 + dst_file;
    int piece = state[src_index];

    // Ensure we are moving an actual piece
    if (piece == EMPTY) {
        return false;
    }
    
    // Determine piece color and ensure we are moving our own piece
    bool isWhite = (piece >= WHITE_PAWN && piece <= WHITE_KING);
    if ((isWhite && player != WHITE) || (!isWhite && player != BLACK)) {
        return false;
    }
    
    // Cannot capture a friendly piece.
    int dst_piece = state[dst_index];
    if (dst_piece != EMPTY) {
        bool dst_isWhite = (dst_piece >= WHITE_PAWN && dst_piece <= WHITE_KING);
        if (isWhite == dst_isWhite)
            return false;
    }

    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;

    // Ensure we are actually moving a piece.
    if (file_diff == 0 && rank_diff == 0) {
        return false;
    }

    return true;
}

bool Board::is_legal_pawn_move(std::string notation, Color player, int src_index, int dst_index) {
    // Assumes move has already been checked to be real

    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int direction = (player == WHITE) ? 1 : -1;
    int start_rank = (player == WHITE) ? 1 : 6;
    int back_rank = (player == WHITE) ? 7 : 0;

    // One square forward (explicit promotion)
    if (notation.length() == 6 && file_diff == 0 && rank_diff == direction && 
            state[dst_index] == EMPTY && dst_rank == back_rank) {
        return (!pinned_move(player, src_index, dst_index));
    }

    // Diagonal capture (explicit promotion)
    if (notation.length() == 6 && abs_file_diff == 1 && rank_diff == direction && 
            state[dst_index] != EMPTY && dst_rank == back_rank) {
        return (!pinned_move(player, src_index, dst_index));
    }

    // If explicit promotion conditions do not pass, explicit promotion move is invalid!
    if (notation.length() == 6) {
        return false;
    }

    // One square forward (no capture)
    if (file_diff == 0 && rank_diff == direction && state[dst_index] == EMPTY) {
        return (!pinned_move(player, src_index, dst_index));
    }
    
    // Two squares forward from starting position
    int intermediate_index = src_index + direction * 8;
    if (file_diff == 0 && rank_diff == 2 * direction && src_rank == start_rank &&
            state[intermediate_index] == EMPTY && state[dst_index] == EMPTY) {
        return (!pinned_move(player, src_index, dst_index));
    }
    
    // Diagonal capture
    if (abs_file_diff == 1 && rank_diff == direction && state[dst_index] != EMPTY) {
        return (!pinned_move(player, src_index, dst_index));
    }

    // En passant capture
    if (abs_file_diff == 1 && rank_diff == direction && dst_index == en_passant_square) {
        return (!pinned_move(player, src_index, dst_index));
    }
    
    return false;
}

bool Board::is_legal_knight_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int abs_rank_diff = std::abs(rank_diff);

    return ((abs_file_diff == 2 && abs_rank_diff == 1) || (abs_file_diff == 1 && abs_rank_diff == 2)) &&
        (!pinned_move(player, src_index, dst_index));
}

bool Board::is_legal_diagonal_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int abs_rank_diff = std::abs(rank_diff);

    if (abs_file_diff == abs_rank_diff && abs_file_diff > 0) {
        int step_file = (file_diff > 0) ? 1 : -1;
        int step_rank = (rank_diff > 0) ? 1 : -1;
        int current_file = src_file + step_file;
        int current_rank = src_rank + step_rank;
        while (current_file != dst_file && current_rank != dst_rank) {
            if (state[current_rank * 8 + current_file] != EMPTY) {
                return false;
            }
            current_file += step_file;
            current_rank += step_rank;
        }
        return (!pinned_move(player, src_index, dst_index));
    }
    return false;
}

bool Board::is_legal_straight_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;

    if ((file_diff == 0 && rank_diff != 0) || (rank_diff == 0 && file_diff != 0)) {
        int step_file = (file_diff == 0) ? 0 : (file_diff > 0 ? 1 : -1);
        int step_rank = (rank_diff == 0) ? 0 : (rank_diff > 0 ? 1 : -1);
        int current_file = src_file + step_file;
        int current_rank = src_rank + step_rank;
        while (current_file != dst_file || current_rank != dst_rank) {
            if (state[current_rank * 8 + current_file] != EMPTY) {
                return false;
            }
            current_file += step_file;
            current_rank += step_rank;
        }
        return (!pinned_move(player, src_index, dst_index));
    }
    return false;
}

bool Board::is_legal_king_move(Color player, int src_index, int dst_index) {
    int src_file = src_index % 8;
    int src_rank = src_index / 8;
    int dst_file = dst_index % 8;
    int dst_rank = dst_index / 8;
    int file_diff = dst_file - src_file;
    int rank_diff = dst_rank - src_rank;
    int abs_file_diff = std::abs(file_diff);
    int abs_rank_diff = std::abs(rank_diff);
    return (abs_file_diff <= 1 && abs_rank_diff <= 1) && (!pinned_move(player, src_index, dst_index));
}

bool Board::is_queenside_castle_legal(Color player) {
    // Ensure neither the king nor the rook has previously moved
    if (player == WHITE && !white_can_ooo) {
        return false;
    }

    if (player == BLACK && !black_can_ooo) {
        return false;
    }

    // Ensure there are no pieces between the king and the rook
    if (player == WHITE && (state[1] != EMPTY || state[2] != EMPTY || state[3] != EMPTY)) {
        return false;
    }

    if (player == BLACK && (state[57] != EMPTY || state[58] != EMPTY  || state[59] != EMPTY)) {
        return false;
    }

    // Ensure the king is not castling out of, through, or into check.
    if (player == WHITE && (is_square_under_attack(2, 0, WHITE) || is_square_under_attack(3, 0, WHITE) 
                            || is_square_under_attack(4, 0, WHITE))) {
        return false;
    }

    if (player == BLACK && (is_square_under_attack(2, 7, BLACK) || is_square_under_attack(3, 7, BLACK) 
                            || is_square_under_attack(4, 7, BLACK))) {
        return false;
    }

    return true;
}

bool Board::is_kingside_castle_legal(Color player) {
    // Ensure neither the king nor the rook has previously moved
    if (player == WHITE && !white_can_oo) {
        return false;
    }

    if (player == BLACK && !black_can_oo) {
        return false;
    }

    // Ensure there are no pieces between the king and the rook
    if (player == WHITE && (state[5] != EMPTY || state[6] != EMPTY)) {
        return false;
    }

    if (player == BLACK && (state[61] != EMPTY || state[62] != EMPTY)) {
        return false;
    }

    // Ensure the king is not castling out of, through, or into check.
    if (player == WHITE && (is_square_under_attack(4, 0, WHITE) || is_square_under_attack(5, 0, WHITE) 
                            || is_square_under_attack(6, 0, WHITE))) {
        return false;
    }

    if (player == BLACK && (is_square_under_attack(4, 7, BLACK) || is_square_under_attack(5, 7, BLACK) 
                            || is_square_under_attack(6, 7, BLACK))) {
        return false;
    }

    return true;
}

Piece Board::get_piece(int file, int rank) {
    if (rank < 0 || rank >= 8 || file < 0 || file >= 8) {
        return EMPTY;
    }

    int index = rank * 8 + file;
    return state[index];
}

// Get lowest index on board that holds 'piece'. Returns -1 if piece isn't found.
int Board::get_lowest_piece_index(Piece piece) {
    for (int i = 0; i < 64; i++) {
        if (state[i] == piece) {
            return i;
        }
    }
    return -1;
}

// Check if player's piece at file/rank is under attack from an opposing king.
bool Board::is_under_attack_from_king(int file, int rank, Color player) {
    Piece attacker = BLACK_KING;
    if (player == BLACK) {
        attacker = WHITE_KING;
    }

    for (int file_offset = -1; file_offset <= 1; file_offset++) {
        for (int rank_offset = -1; rank_offset <= 1; rank_offset++) {
            if (file_offset == 0 && rank_offset == 0) {
                continue;
            }
            
            if (get_piece(file + file_offset, rank + rank_offset) == attacker) {
                return true;
            }
        }
    }

    return false;
}


// Check if player's piece at file/rank is under attack from an opposing pawn.
bool Board::is_under_attack_from_pawn(int file, int rank, Color player) {
    Piece attacker = BLACK_PAWN;
    int rank_offset = 1;
    if (player == BLACK) {
        attacker = WHITE_PAWN;
        rank_offset = -1;
    }

    if (get_piece(file + 1, rank + rank_offset) == attacker) {
        return true;
    }
    
    if (get_piece(file - 1, rank + rank_offset) == attacker) {
        return true;
    }

    return false;
}

// Check if player's piece at file/rank is under attack from an opposing knight.
bool Board::is_under_attack_from_knight(int file, int rank, Color player) {
    Piece attacker = BLACK_KNIGHT;
    if (player == BLACK) {
        attacker = WHITE_KNIGHT;
    }

    for (int short_offset : {-1, 1}) {
        for (int long_offset : {-2, 2}) {
            if (get_piece(file + short_offset, rank + long_offset) == attacker) {
                return true;
            }

            if (get_piece(file + long_offset, rank + short_offset) == attacker) {
                return true;
            }
        }
    }

    return false;
}

// Check if player's piece at file/rank is under attack from an opposing diagonal piece (bishop/queen).
bool Board::is_under_attack_from_diagonal(int file, int rank, Color player) {
    Piece bishop_attacker = BLACK_BISHOP;
    Piece queen_attacker = BLACK_QUEEN;
    if (player == BLACK) {
        bishop_attacker = WHITE_BISHOP;
        queen_attacker = WHITE_QUEEN;
    }
    
    // Check all 4 diagonal directions.
    for (int df : {1, -1}) {
        for (int dr : {1, -1}) {
            int f = file + df;
            int r = rank + dr;
            while (f >= 0 && f < 8 && r >= 0 && r < 8) {
                Piece piece = get_piece(f, r);
                if (piece != EMPTY) {
                    if (piece == bishop_attacker || piece == queen_attacker) {
                        return true;
                    }
                    break; // blocked by another piece
                }
                f += df;
                r += dr;
            }
        }
    }
    
    return false;
}

// Check if player's piece at file/rank is under attack from an opposing straight piece (rook/queen).
bool Board::is_under_attack_from_straight(int file, int rank, Color player) {
    Piece rook_attacker = BLACK_ROOK;
    Piece queen_attacker = BLACK_QUEEN;
    if (player == BLACK) {
        rook_attacker = WHITE_ROOK;
        queen_attacker = WHITE_QUEEN;
    }
    
    // Check horizontal directions.
    for (int d : {1, -1}) {
        int f = file + d;
        int r = rank;
        while (f >= 0 && f < 8) {
            Piece piece = get_piece(f, r);
            if (piece != EMPTY) {
                if (piece == rook_attacker || piece == queen_attacker) {
                    return true;
                }
                break;
            }
            f += d;
        }
    }
    
    // Check vertical directions.
    for (int d : {1, -1}) {
        int f = file;
        int r = rank + d;
        while (r >= 0 && r < 8) {
            Piece piece = get_piece(f, r);
            if (piece != EMPTY) {
                if (piece == rook_attacker || piece == queen_attacker) {
                    return true;
                }
                break;
            }
            r += d;
        }
    }
    
    return false;
}

bool Board::is_square_under_attack(int file, int rank, Color player) {

    // Check if the square is under attack from a king
    bool under_attack = is_under_attack_from_king(file, rank, player);

    // Check if the square is under attack from a pawn
    if (!under_attack) {
        under_attack = is_under_attack_from_pawn(file, rank, player);
    }

    // Check if the square is under attack from a knight
    if (!under_attack) {
        under_attack = is_under_attack_from_knight(file, rank, player);
    }

    // Check if the square is under attack from a bishop/queen
    if (!under_attack) {
        under_attack = is_under_attack_from_diagonal(file, rank, player);
    }

    // Check if the square is under attack from a rook/queen
    if (!under_attack) {
        under_attack = is_under_attack_from_straight(file, rank, player);
    }

    return under_attack;
}

bool Board::pinned_move(Color player, int src_index, int dst_index) {
    // Assumes that move is completely valid and possible!
    // Checks to see if after moving, the king would be in check, making it an invalid move.

    // Temporarily make move to see board after move
    Piece temp = state[dst_index];
    state[dst_index] = state[src_index];
    state[src_index] = EMPTY;

    bool under_attack = is_checked(player);

    // Undo temporary move
    state[src_index] = state[dst_index];
    state[dst_index] = temp;

    return under_attack;
}

bool Board::is_checked(Color player) {
    // Determine index of king
    int king_index = 0;
    if (player == WHITE) {
        king_index = get_lowest_piece_index(WHITE_KING);
    } else {
        king_index = get_lowest_piece_index(BLACK_KING);
    }

    // Check if king_index is currently under attack by opposing pieces
    int king_file = king_index % 8;
    int king_rank = king_index / 8;
    return is_square_under_attack(king_file, king_rank, player);
}

bool Board::is_fifty_move_rule_draw() {
    return draw_move_counter >= 100;
}

bool Board::is_threefold_repetition_draw() {
    // NOTE: This assumes the threefold repetition is consecutive! This isn't necessarily true (at least over the board).

    if (prev_moves.size() < 12) {
        return false;
    }

    return (prev_moves[0].get_move() == prev_moves[4].get_move() && prev_moves[4].get_move() == prev_moves[8].get_move() &&
            prev_moves[1].get_move() == prev_moves[5].get_move() && prev_moves[5].get_move() == prev_moves[9].get_move() && 
            prev_moves[2].get_move() == prev_moves[6].get_move() && prev_moves[6].get_move() == prev_moves[10].get_move() &&
            prev_moves[3].get_move() == prev_moves[7].get_move() && prev_moves[7].get_move() == prev_moves[11].get_move());
}

// Assumes move legality has already been checked!
void Board::update_move(const Move& move, Color player) {    
    // Move Piece and Update History
    std::string notation = move.get_move();

    // Update Previous Move History
    handle_prev_move_history(notation);

    if (notation == "oo") {
        castle_kingside(player);
    } else if (notation == "ooo") {
        castle_queenside(player);
    } else {
        // Normal piece move
        int src_file = notation[0] - 'a';
        int src_rank = notation[1] - '1';
        int dst_file = notation[2] - 'a';
        int dst_rank = notation[3] - '1';
        int src_index = src_rank * 8 + src_file;
        int dst_index = dst_rank * 8 + dst_file;

        Piece piece = state[src_index];
        state[dst_index] = piece;
        
        // Remove en-passanted pawns 
        if (piece == WHITE_PAWN && dst_index == en_passant_square) {
            state[dst_index-8] = EMPTY;
        } else if (piece == BLACK_PAWN && dst_index == en_passant_square) {
            state[dst_index+8] = EMPTY;
        }

        state[src_index] = EMPTY;
        
        // Castling
        handle_castling_history(piece, src_index);

        // En passant
        handle_en_passant_history(piece, src_rank, dst_rank, src_file);
        
        // Promotion
        handle_promotion(piece, notation, dst_rank, dst_index);
    }
}


void Board::castle_kingside(Color player) {
    if (player == WHITE) {
        state[7] = EMPTY; // old rook square
        state[4] = EMPTY; // old king square
        state[6] = WHITE_KING;
        state[5] = WHITE_ROOK;
        white_can_oo = false;
        white_can_ooo = false;
    } else {
        state[63] = EMPTY; // old rook square
        state[60] = EMPTY; // old king square
        state[62] = BLACK_KING;
        state[61] = BLACK_ROOK;
        black_can_oo = false;
        black_can_ooo = false;
    }
}

void Board::castle_queenside(Color player) {
    if (player == WHITE) {
        state[0] = EMPTY; // old rook square
        state[4] = EMPTY; // old king square
        state[2] = WHITE_KING;
        state[3] = WHITE_ROOK;
        white_can_oo = false;
        white_can_ooo = false;
    } else {
        state[56] = EMPTY; // old rook square
        state[60] = EMPTY; // old king square
        state[58] = BLACK_KING;
        state[59] = BLACK_ROOK;
        black_can_oo = false;
        black_can_ooo = false;
    }
}

void Board::handle_prev_move_history(std::string& notation) {

    // Update previous move buffer
    prev_moves.push_back(notation);
    if (prev_moves.size() > 12) {
        prev_moves.erase(prev_moves.begin());
    }

    // Update 50-move counter
    int src_file = notation[0] - 'a';
    int src_rank = notation[1] - '1';
    int dst_file = notation[2] - 'a';
    int dst_rank = notation[3] - '1';
    int src_index = src_rank * 8 + src_file;
    int dst_index = dst_rank * 8 + dst_file;

    Piece src_piece = state[src_index];
    Piece dst_piece = state[dst_index];

    // If piece is captured or moved piece is a pawn, reset counter to 0!
    if (dst_piece != EMPTY || src_piece == WHITE_PAWN || src_piece == BLACK_PAWN) {
        draw_move_counter = 0;
    } else {
        draw_move_counter++;
    }
}

void Board::handle_castling_history(Piece piece, int src_index) {
    if (piece == WHITE_KING) {
        white_can_oo = false;
        white_can_ooo = false;
    } else if (piece == BLACK_KING) {
        black_can_oo = false;
        black_can_ooo = false;
    } else if (piece == WHITE_ROOK && src_index == 0) {
        white_can_ooo = false;
    } else if (piece == WHITE_ROOK && src_index == 7) {
        white_can_oo = false;
    } else if (piece == BLACK_ROOK && src_index == 56) {
        black_can_ooo = false;
    } else if (piece == BLACK_ROOK && src_index == 63) {
        black_can_oo = false;
    }
}

void Board::handle_en_passant_history(Piece piece, int src_rank, int dst_rank, int src_file) {
    if (piece == WHITE_PAWN && src_rank == 1 && dst_rank == 3) {
        en_passant_square = 16 + src_file;
    } else if (piece == BLACK_PAWN && src_rank == 6 && dst_rank == 4) {
        en_passant_square = 40 + src_file;
    } else {
        en_passant_square = -1;
    }
}

void Board::handle_promotion(Piece piece, std::string& notation, int dst_rank, int dst_index) {

    if (piece == WHITE_PAWN && dst_rank == 7) {

        Piece promotion_piece = WHITE_QUEEN;
        if (notation.length() == 6) {
            char piece_char = notation[5];
            if (piece_char == 'B') {
                promotion_piece = WHITE_BISHOP;
            } else if (piece_char == 'R') {
                promotion_piece = WHITE_ROOK;
            } else if (piece_char == 'N') {
                promotion_piece = WHITE_KNIGHT;
            }
        }

        state[dst_index] = promotion_piece;

    } else if (piece == BLACK_PAWN && dst_rank == 0) {

        Piece promotion_piece = BLACK_QUEEN;
        if (notation.length() == 6) {
            char piece_char = notation[5];
            if (piece_char == 'B') {
                promotion_piece = BLACK_BISHOP;
            } else if (piece_char == 'R') {
                promotion_piece = BLACK_ROOK;
            } else if (piece_char == 'N') {
                promotion_piece = BLACK_KNIGHT;
            }
        }

        state[dst_index] = promotion_piece;
    }
}

Board::Board() {
    // 'state' is structured as follows: A1 [0] --> H1 [7], A2 [8] --> H2 [15], etc.
    state[0] = WHITE_ROOK;
    state[1] = WHITE_KNIGHT;
    state[2] = WHITE_BISHOP;
    state[3] = WHITE_QUEEN;
    state[4] = WHITE_KING;
    state[5] = WHITE_BISHOP;
    state[6] = WHITE_KNIGHT;
    state[7] = WHITE_ROOK;

    for (int i = 8; i < 16; i++) {
        state[i] = WHITE_PAWN;
    }

    for (int i = 16; i < 48; i++) {
        state[i] = EMPTY;
    }

    for (int i = 48; i < 56; i++) {
        state[i] = BLACK_PAWN;
    }

    state[56] = BLACK_ROOK;
    state[57] = BLACK_KNIGHT;
    state[58] = BLACK_BISHOP;
    state[59] = BLACK_QUEEN;
    state[60] = BLACK_KING;
    state[61] = BLACK_BISHOP;
    state[62] = BLACK_KNIGHT;
    state[63] = BLACK_ROOK;

    // History variables
    black_can_oo = true;
    black_can_ooo = true;
    white_can_oo = true;
    white_can_ooo = true;
    en_passant_square = -1;
    draw_move_counter = 0;
    prev_moves.clear();
}

vector<std::string> Board::generate_all_squares() {
    // Note: This ABSOLUTELY MUST generate squares A1->H1, A2->H2, etc... in that order!

    vector<std::string> squares;
    for (char rank = '1'; rank <= '8'; ++rank) {
        for (char file = 'a'; file <= 'h'; ++file) {
            std::string square;
            square.push_back(file);
            square.push_back(rank);
            squares.push_back(square);
        }
    }
    return squares;
}

Board::Board(const Board& other) {
    prev_moves = other.prev_moves;
    std::copy(std::begin(other.state), std::end(other.state), std::begin(state));
    en_passant_square = other.en_passant_square;
    draw_move_counter = other.draw_move_counter;
    black_can_oo = other.black_can_oo;
    black_can_ooo = other.black_can_ooo;
    white_can_oo = other.white_can_oo;
    white_can_ooo = other.white_can_ooo;
}

Board::Board(std::string FEN) {
    // FEN format: "<piece placement> <active color> <castling> <en passant> <halfmove> <fullmove>"
    std::istringstream iss(FEN);
    std::string piecePlacement, activeColor, castling, enPassant, halfmove, fullmove;
    iss >> piecePlacement >> activeColor >> castling >> enPassant >> halfmove >> fullmove;

    // Clear board state.
    for (int i = 0; i < 64; i++) {
        state[i] = EMPTY;
    }

    // Parse piece placement. FEN ranks go from 8 (top) to 1 (bottom).
    // Our state[0] corresponds to A1, so rank 8 maps to indices 56-63.
    int rank = 7, file = 0;
    for (char c : piecePlacement) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (std::isdigit(c)) {
            file += c - '0';
        } else {
            int idx = rank * 8 + file;
            switch (c) {
                case 'P': state[idx] = WHITE_PAWN; break;
                case 'N': state[idx] = WHITE_KNIGHT; break;
                case 'B': state[idx] = WHITE_BISHOP; break;
                case 'R': state[idx] = WHITE_ROOK; break;
                case 'Q': state[idx] = WHITE_QUEEN; break;
                case 'K': state[idx] = WHITE_KING; break;
                case 'p': state[idx] = BLACK_PAWN; break;
                case 'n': state[idx] = BLACK_KNIGHT; break;
                case 'b': state[idx] = BLACK_BISHOP; break;
                case 'r': state[idx] = BLACK_ROOK; break;
                case 'q': state[idx] = BLACK_QUEEN; break;
                case 'k': state[idx] = BLACK_KING; break;
                default: state[idx] = EMPTY; break;
            }
            file++;
        }
    }

    // Set castling rights.
    white_can_oo = white_can_ooo = black_can_oo = black_can_ooo = false;
    if (castling != "-") {
        for (char c : castling) {
            if (c == 'K') white_can_oo = true;
            else if (c == 'Q') white_can_ooo = true;
            else if (c == 'k') black_can_oo = true;
            else if (c == 'q') black_can_ooo = true;
        }
    }

    // Set en passant square.
    if (enPassant == "-") {
        en_passant_square = -1;
    } else {
        int fileIdx = enPassant[0] - 'a';
        int rankIdx = enPassant[1] - '1';
        en_passant_square = rankIdx * 8 + fileIdx;
    }

    // Set draw move counter from halfmove clock.
    draw_move_counter = std::stoi(halfmove);

    // Clear move history.
    prev_moves.clear();
}

std::wstring get_piece_string(const Piece piece) {
    switch(piece) {
        case BLACK_PAWN:     
            return L"♙";
        case BLACK_ROOK:     
            return L"♖";
        case BLACK_KNIGHT:   
            return L"♘";
        case BLACK_BISHOP:   
            return L"♗";
        case BLACK_QUEEN:    
            return L"♕";
        case BLACK_KING:     
            return L"♔";
        case WHITE_PAWN:     
            return L"♟";
        case WHITE_ROOK:     
            return L"♜";
        case WHITE_KNIGHT:   
            return L"♞";
        case WHITE_BISHOP:   
            return L"♝";
        case WHITE_QUEEN:    
            return L"♛";
        case WHITE_KING:     
            return L"♚";
        default:             
            return L".";
    }
}
