CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

SRC = poset.cc poset.h
OBJ = poset.o
PROG = poset

.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(OBJ) $(PROG)
