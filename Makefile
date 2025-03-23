CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -std=c++17
TARGET = app

SRCS = main.cpp chess/game.cpp chess/board.cpp chess/move.cpp chess/utils.cpp testing/test_cases.cpp testing/debug.cpp bot/driver.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
