# Sudoku solver using constraint propagation

Quick and dirty C++ code for solving sudoku with constraint propagation.

## How to use

- Compile: `make`

- Create an input file with the following format: (`-` for unknown cell, or use
  the [example](examples/) files)
  ```
  x x x  x x x  x x x
  x x x  x x x  x x x
  x x x  x x x  x x x

  x x x  x x x  x x x
  x x x  x x x  x x x
  x x x  x x x  x x x

  x x x  x x x  x x x
  x x x  x x x  x x x
  x x x  x x x  x x x
  ```

- Solve the board: `cat <filename> | ./main`

## TODO

- Visualization
- Improve file parsing and error handling
- Create sudoku generator and web app
- Benchmark against conventional backtracking algorithm 

## License

This project is licensed under the [MIT license](LICENSE).
