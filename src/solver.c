#include "solver.h"

#define popcnt32 __builtin_popcount
#define ctz32    __builtin_ctz

// We represent all possible values of a cell in a bitmap with length 9
const uint16_t full = (1 << 9) - 1;

Solver Solver_new(void) {
  Solver solver;
  for (int i = 0; i < 81; ++i) {
    solver.possibilities[i] = full;
  }
  solver.remaining = 81;
  return solver;
}

// Constraint propagation:
// - When a cell is collapsed, remove its value from the possible values of
// cells in the same row, collumn or box.
// - If any of those cells has one possible value left, collapse that cell and
// do the same thing again.
// - If any of those cells has no possible value, we reached an invalid state.
bool Solver_propagate(Solver *solver, int x, int y) {
  --solver->remaining;

  const uint16_t rev = (~solver->possibilities[x + y * 9]) & full;

  // Row
  for (int i = 0; i < 9; i++) {
    if (i != x) {
      uint16_t *cell = solver->possibilities + (i + y * 9);
      if ((*cell & rev) == 0) return false;
      if (popcnt32(*cell) == 1) continue;
      *cell &= rev;
      if (popcnt32(*cell) == 1 && !Solver_propagate(solver, i, y)) {
        return false;
      }
    }
  }

  // Collumn
  for (int i = 0; i < 9; i++) {
    if (i != y) {
      uint16_t *cell = solver->possibilities + (x + i * 9);
      if ((*cell & rev) == 0) return false;
      if (popcnt32(*cell) == 1) continue;
      *cell &= rev;
      if (popcnt32(*cell) == 1 && !Solver_propagate(solver, x, i)) {
        return false;
      }
    }
  }

  // Box
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      const int sub_x = (x / 3) * 3 + i;
      const int sub_y = (y / 3) * 3 + j;

      if (sub_x != x && sub_y != y) {
        uint16_t *cell = solver->possibilities + (sub_x + sub_y * 9);
        if ((*cell & rev) == 0) return false;
        if (popcnt32(*cell) == 1) continue;
        *cell &= rev;
        if (popcnt32(*cell) == 1 && !Solver_propagate(solver, sub_x, sub_y)) {
          return false;
        }
      }
    }
  }

  return true;
}

bool Solver_insert(Solver *solver, int x, int y, int val) {
  uint16_t *cell = solver->possibilities + (x + y * 9);
  uint16_t mask = (1 << (val - 1));
  if (*cell & mask) {
    if (*cell != mask) {
      *cell = mask;
      Solver_propagate(solver, x, y);
    }
    return true;
  }
  return false;
}

bool Solver_solve(Solver *solver) {
  // Solve a sudoku board:
  // - Find the most-constrained non collapsed cell
  // - For each possible value of that cell, collapse it to that value
  // - Try to recursively solve each subsequent board

  // The board is already solved
  if (solver->remaining == 0) return true;

  int target_x = -1, target_y = -1;
  int min_entropy = 10;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; ++j) {
      const int val = popcnt32(solver->possibilities[9 * i + j]);
      if (val > 1 && val < min_entropy) {
        target_x = j;
        target_y = i;
        min_entropy = val;
      }
    }
  }

  const int target_pos = target_x + target_y * 9;

  uint16_t mask = solver->possibilities[target_pos];

  // Store the previous state so we can backtrack
  Solver last = *solver;

  while (mask) {
    uint16_t val = mask & -mask;
    mask ^= val;

    // Collapse, propagate the value and try to solve the resulting board
    solver->possibilities[target_pos] = val;
    if (Solver_propagate(solver, target_x, target_y) && Solver_solve(solver)) {
      return true;
    }

    // Backtrack
    *solver = last;
  }

  // We tried all the value, the board is unsolvable
  return false;
}

void Solver_write(Solver *restrict solver, char buf[restrict 81]) {
  for (int i = 0; i < 81; ++i) {
    uint16_t val = solver->possibilities[i];
    if (popcnt32(val) == 1) {
      buf[i] = ctz32(solver->possibilities[i]) + '1';
    } else {
      buf[i] = '.';
    }
  }
}
