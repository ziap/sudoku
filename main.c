#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "solver.h"

typedef union {
  int arr[81];
  int matrix[9][9];
} Puzzle;

typedef struct {
  Puzzle *data;
  int len;
} Puzzles;

Puzzles Puzzles_parse(const char* file_path) {
  FILE *f = fopen(file_path, "rb");
  if (!f) {
    fprintf(stderr, "Failed to open `%s`: %s\n", file_path, strerror(errno));
    Puzzles result;
    result.data = NULL;
    result.len = 0;
    return result;
  }

  fseek(f, 0, SEEK_END);
  size_t file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buf = malloc(file_size + 1);
  fread(buf, 1, file_size, f);
  buf[file_size] = 0;

  fclose(f);

  int result_cap = 1024;
  Puzzles result;
  result.data = malloc(result_cap * sizeof(Puzzle));
  result.len = 0;

  const char *ptr = buf;
  int line = 1;

  while (*ptr) {
    if (*ptr == '#') goto next_line;
    if (result.len == result_cap) {
      result_cap *= 2;
      result.data = realloc(result.data, result_cap * sizeof(Puzzle));
    }
    for (int col = 0; col < 81; ++col) {
      char c = ptr[col];
      if (c >= '1' && c <= '9') {
        result.data[result.len].arr[col] = c - '0';
      } else if (c == '.') {
        result.data[result.len].arr[col] = 0;
      } else {
        fprintf(stderr, "%s:%d:%d: Invalid character: `%c`", file_path, line + 1, col, c);
        free(result.data);
        result.data = NULL;
        result.len = 0;
        return result;
      }
    }
    ++result.len;
    ptr += 81;
  next_line:
    while (*ptr != '\n') ++ptr;
    while (*ptr == '\n') ++ptr;
    ++line;
  }

  free(buf);
  return result;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Input file not provided!\n");
    return 1;
  }

  Puzzles puzzles = Puzzles_parse(argv[1]);
  if (puzzles.data == NULL) return 1;

  char *result = malloc(puzzles.len * 82);
  
  clock_t start = clock();
  for (int t = 0; t < puzzles.len; ++t) {
    Solver solver = Solver_new();
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        int val = puzzles.data[t].matrix[i][j];
        if (val) Solver_insert(&solver, j, i, val);
      }
    }

    Solver_solve(&solver);
    Solver_write(&solver, result + (82 * t));

    result[82 * t + 81] = '\n';
  }
  clock_t end = clock();
  
  printf("Solving speed: %.2f puzzles/s\n", (double)(puzzles.len * CLOCKS_PER_SEC) / (end - start));

  if (argc > 2) {
    FILE *f = fopen(argv[2], "wb");
    if (f) {
      fwrite(result, puzzles.len * 82, 1, f);
      fclose(f);
    } else {
      fprintf(stderr, "Failed to open `%s`: %s\n", argv[2], strerror(errno));
    }
  }

  free(result);
  free(puzzles.data);
}
