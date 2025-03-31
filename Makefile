CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -std=c++17 -Wc++17-extensions -I/usr/local/Cellar/ncurses/6.5/include
LDFLAGS = -L/usr/local/Cellar/ncurses/6.5/lib
TARGET = app

SRCS = main.cpp chess/game.cpp chess/board.cpp chess/move.cpp chess/gui.cpp chess/utils.cpp testing/test_cases.cpp testing/debug.cpp bot/driver.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) -lncurses -lncursesw

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
