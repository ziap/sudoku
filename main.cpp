#include <array>
#include <iostream>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

constexpr u8 w = 9;
constexpr u8 h = 9;

// We represent all possible values of a cell in a bitmap with length 9
constexpr u16 full = 0x1ff;

// The possible values of a cell AKA the number of set bit
constexpr auto entropy = ([]() constexpr {
  constexpr auto len = 1 << 9;
  std::array<u8, len> result;
  for (auto i = 0u; i < len; i++) {
    result[i] = 0;
    for (auto mask = i; mask; mask &= (mask - 1)) result[i]++;
  }
  return result;
})();

// Return a cell's value if it's collapsed otherwise 0
constexpr auto collapsed_value = ([]() constexpr {
  constexpr auto len = 1 << 9;
  std::array<u8, len> result = {0};
  for (auto i = 0u; i < 9; i++) result[1 << i] = i + 1;
  return result;
})();

u16 board[w * h];

// The number of uncollapsed cells
u8 remaining = w * h;

// Constraint propagation:
// - When a cell is collapsed, remove its value from the possible values of
// cells in the same row, collumn or box.
// - If any of those cells has one possible value left, collapse that cell and
// do the same thing again.
// - If any of those cells has no possible value, we reached an invalid state.
bool propagate(u8 x, u8 y) {
  remaining--;

  const auto rev = (~board[x + y * w] & full);

  // Row
  for (auto i = 0; i < w; i++) {
    if (i != x) {
      auto &cell = board[i + y * w];
      if ((cell & rev) == 0) return false;
      if (collapsed_value[cell]) continue;
      cell &= rev;
      if (collapsed_value[cell] && !propagate(i, y)) return false;
    }
  }

  // Collumn
  for (auto i = 0; i < h; i++) {
    if (i != y) {
      auto &cell = board[x + i * w];
      if ((cell & rev) == 0) return false;
      if (collapsed_value[cell]) continue;
      cell &= rev;
      if (collapsed_value[cell] && !propagate(x, i)) return false;
    }
  }

  // Box
  for (auto i = 0; i < 3; i++) {
    for (auto j = 0; j < 3; j++) {
      const auto sub_x = (x / 3) * 3 + i;
      const auto sub_y = (y / 3) * 3 + j;

      if (sub_x != x && sub_y != y) {
        auto &cell = board[sub_x + sub_y * w];
        if ((cell & rev) == 0) return false;
        if (collapsed_value[cell]) continue;
        cell &= rev;
        if (collapsed_value[cell] && !propagate(sub_x, sub_y)) return false;
      }
    }
  }

  return true;
}

// Solve a sudoku board:
// - Find the first non collapsed cell
// - For each possible value of that cell, collapse it to that value
// - Try to recursively solve each subsequent board
bool solve() {
  // The board is already solved
  if (remaining == 0) return true;

  u8 lowest_x, lowest_y;

  for (auto i = 0;; i++) {
    const auto val = entropy[board[i]];
    if (val > 1) {
      lowest_x = i % w;
      lowest_y = i / w;
      break;
    }
  }

  const auto lowest_entropy_pos = lowest_x + lowest_y * w;

  auto mask = board[lowest_entropy_pos];

  // Store the previous state so we can backtrack
  u16 last[w * h];
  auto last_remaining = remaining;
  std::copy(board, board + (w * h), last);

  while (mask) {
    auto val = mask & -mask;
    mask ^= val;

    // Collapse, propagate the value and try to solve the resulting board
    board[lowest_entropy_pos] = val;
    if (propagate(lowest_x, lowest_y) && solve()) return true;

    // Backtrack
    std::copy(last, last + (w * h), board);
    remaining = last_remaining;
  }

  // We tried all the value, the board is unsolvable
  return false;
}

int main() {
  // Initialize the board with all possible values
  std::fill(board, board + w * h, full);

  // Read stdin and fill the board
  // NOTE: no file support, to read a file pipe it to stdin
  // TODO: improve input with better error handling
  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      std::string buf;
      std::cin >> buf;
      if (buf != "-") {
        const auto val = std::stoi(buf);

        auto &cell = board[j + i * w];
        if (!collapsed_value[cell]) {
          cell = (1 << (val - 1));
          propagate(j, i);
        }
      }
    }
  }

  if (solve()) {
    for (auto i = 0; i < h; i++) {
      for (auto j = 0; j < w; j++) {
        const auto val = collapsed_value[board[j + i * w]];
        if (val) std::cout << u8(val + '0') << ' ';
        else
          std::cout << "! ";
        if (j % 3 == 2) std::cout << ' ';
      }
      std::cout << '\n';
      if (i % 3 == 2) std::cout << '\n';
    }

  } else {
    std::cout << "Can't solve board!\n";
  }

  return 0;
}
