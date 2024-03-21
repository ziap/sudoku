#ifndef SOLVER_H
#define SOLVER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint16_t possibilities[81];
  int remaining;
} Solver;

// Initialize the board with all possible values
extern Solver Solver_new(void);

// Add a clue to the solver
extern bool Solver_insert(Solver *solver, int x, int y, int val);

// Solve the sudoku puzzle with backtracking
extern bool Solver_solve(Solver *solver);

// Write the internal state of the solver to an array
// 1-9 denode clues or solved tiles
// . denodes unsolved tiles
extern void Solver_write(Solver *restrict solver, char buf[restrict 81]);

#endif
