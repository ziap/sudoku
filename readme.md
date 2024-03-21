# Sudoku solver using constraint propagation

Moderately-fast Sodoku solver with constraint propagation and back-tracking in C.

## Example

### Building

The project can be built with the script `build.sh`.

```sh
# Compile with the default configuration
./build.sh

# Compile with gcc
CC=gcc ./build.sh
```

### Running

Currently the only way to input a puzzle is through a file. We'll test with
this case from Wikipeia which is designed against brute force algorithms.

![](https://upload.wikimedia.org/wikipedia/commons/e/ed/Sudoku_puzzle_hard_for_brute_force.svg)

We'll store the puzzle in the file `example.txt` and repeat it 10 times to get
an average estimation.

```
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9
```

```sh
./sudoku example.txt example-result.txt
```

This runs the program with the file that we just created, and output the result
to the file `example-result.txt`. The program will also output the solving speed:

```
Solving speed: 567.83 puzzles/s
```

Or about 0.0018 seconds per puzzle.

You can run the program in the provided [dataset](data.tar.gz), which is a
subset of the dataset provided in <https://github.com/t-dillon/tdoku>.

```sh
tar xf data.tar.gz
./sudoku data/puzzles_forum_hardest_1905_11+
./sudoku data/puzzles_magictour_top1465
```

## TODO

- Optimizations:
    - Better constraint representation
    - ...
- Visualization
- Create sudoku generator and web app

## License

This project is licensed under the [MIT license](LICENSE).
