#ifndef GUI_H
#define GUI_H
#include "board.h"
#include <string>

void initialize_gui();
void close_gui();
void refresh_gui();

void draw_square(Piece piece, int file, int rank);
void write_gui_box(std::string& text);

std::string request_player_move(Board& board, Color player);
std::string getSquare(int y, int x);
std::string check_for_castling(Board& board, Color player, std::string& movestr);

#endif
