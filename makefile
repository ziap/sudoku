CXX=clang++
FLAGS=-Wall -Wextra -O3 -march=native -mtune=native -std=c++20

all: main

main: main.cpp
	$(CXX) $(FLAGS) -o main main.cpp
