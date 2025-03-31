#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <locale.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "game.h"
#include "board.h"
#include "gui.h"

using namespace std;

void initialize_gui() {
    // Initialize ncurses
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);

    // Initialize a board and draw it.
    clear();

    // Write down each file number
    mvprintw(0, 2, "abcdefgh");
    mvprintw(9, 2, "abcdefgh");

    // Write down each rank number
    for (int i = 0; i < 8; i++) {
        int rank = 8 - i;
        mvprintw(i + 1, 0, "%d ", rank);
        mvprintw(i + 1, 10, " %d", rank);
    }

    refresh_gui();
}

void close_gui() {
    endwin();
}

void draw_square(Piece piece, int file, int rank) {
    if (rank < 0 || rank >= 8 || file < 0 || file >= 8) {
        std::cerr << "Rank or file is out of drawing bounds!" << std::endl;
        exit(1);
    }

    // NOTE: file and rank are 0-indexed... 0-7 for each!
    std::wstring piece_str = get_piece_string(piece);
    mvaddwstr(8 - rank, file + 2, piece_str.c_str());

    // mvprintw(8 - rank, file + 2, piece_str.c_str());
    refresh_gui();
}

void write_gui_box(std::string& text) {
    // Pad string to be 80 chars long (max length)
    if (text.length() < 80) {
        text = text + std::string(80 - text.length(), ' ');
    }

    // Write text below board
    mvprintw(11, 0, "%s", text.c_str());
    refresh_gui();
}

void refresh_gui() {
    refresh();
}

// Returns an updated move string if move string is a castling move (modifies to oo or ooo)
std::string check_for_castling(Board& board, Color player, std::string& mv) {

    // Only attempt a castle if certain moves are inputted.
    if (((player == WHITE) && mv != "e1a1" && mv != "e1h1") ||
        ((player == BLACK) && mv != "e8a8" && mv != "e8h8")) {
        return mv;
    }

    // Ensure that king exists on source square (e1 for white, e8 for black)
    if (player == WHITE) {
        Piece e1piece = board.get_piece(4, 0);
        if (e1piece != WHITE_KING) {
            return mv;
        }

        if (mv == "e1a1") {
            return "ooo"; // e1a1 is white ooo IF white king is on e1
        } else if (mv == "e1h1") {
            return "oo"; // e1h1 is white oo IF white king is on e1
        } else {
            return mv;
        }
    } else {
        Piece e8piece = board.get_piece(4, 7);
        if (e8piece != BLACK_KING) {
            return mv;
        }

        if (mv == "e8a8") {
            return "ooo"; // e8a8 is black ooo IF black king is on e8
        } else if (mv == "e8h8") {
            return "oo"; // e8h8 is black oo IF black king is on e8
        } else {
            return mv;
        }
    }
}

// Returns a move based on player input
std::string request_player_move(Board& board, Color player) {
    vector<std::string> clickedSquares;

    while (true) {
        int ch = getch();
        if (ch == KEY_MOUSE) {
            MEVENT event;
            if (getmouse(&event) == OK) {
                // Debug: Print mouse event coordinates and button state
                // mvprintw(13, 0, "Mouse event: x=%d, y=%d, bstate=%lu         ", event.x, event.y, event.bstate);

                string square = getSquare(event.y, event.x);
                if (square.empty()) {
                    clickedSquares.clear();

                    std::string text = "No square currently selected. Press a square to select it.";
                    write_gui_box(text);
                } else {
                    clickedSquares.push_back(square);
                    if (clickedSquares.size() == 2) {
                        string movestr = clickedSquares[0] + clickedSquares[1];
                        movestr = check_for_castling(board, player, movestr);

                        Move move;
                        move.set_move(movestr);

                        if (board.is_legal_move(move, player)) {
                            std::string color_string = (player == WHITE) ? "White" : "Black";
                            std::string text = color_string + " plays " + movestr + ".";
                            write_gui_box(text);
                            return movestr;
                        } else {
                            clickedSquares.clear();
                            std::string text = "Illegal move! Try again.";
                            write_gui_box(text);
                        }
                        clickedSquares.clear();
                    } else {
                        std::string text = "Selected " + clickedSquares[0] + ". Select another to play a move.";
                        write_gui_box(text);
                    }
                }
                refresh();
            }
        } else if (ch == 'q') {
            // End the program
            endwin();
            exit(0);
        }
    }

    // Should never reach this!
    assert(false);
    return "";
}

// Map mouse coordinates to board square string (e.g., "e2")
string getSquare(int y, int x) {
    // Our board is drawn from row 1 to 8, and columns 2 to 9.
    if (y < 1 || y > 8 || x < 2 || x > 9)
        return "";
    int rank = 9 - y;           // row 1 => rank 8, row 8 => rank 1
    char file = 'a' + (x - 2);    // col 2 => 'a', col 9 => 'h'
    return string{file} + to_string(rank);
}
