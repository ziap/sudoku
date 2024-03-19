CXX=clang++
FLAGS=-Wall -Wextra -O3 -march=native -mtune=native -pedantic -std=c++20

.PHONY: all clean

all: main

main: main.cpp
	$(CXX) $(FLAGS) -o main main.cpp

clean:
	rm -f main
