#include <array>
#include <bit>
#include <iostream>

using u8 = uint8_t;
using u16 = uint16_t;

constexpr u8 w = 9;
constexpr u8 h = 9;

constexpr u16 full = (1 << 9) - 1;

constexpr auto entropy = ([]() constexpr {
  constexpr auto len = 1 << 9;
  std::array<u8, len> entropy;
  for (auto i = 0u; i < len; i++) entropy[i] = std::popcount(i);
  return entropy;
})();

constexpr auto collapsed = ([]() constexpr {
  constexpr auto len = 1 << 9;
  std::array<u8, len> collapsed = {0};
  for (auto i = 0u; i < 9; i++) collapsed[1 << i] = i + 1;
  return collapsed;
})();

u16 board[w * h];

u8 remaining = 81;

void propagate(u8 x, u8 y) {
  remaining--;

  const auto rev = (~board[x + y * w] & full);

  for (auto i = 0; i < w; i++) {
    if (i != x && !collapsed[board[i + y * w]]) {
      auto &cell = board[i + y * w];
      cell &= rev;
      if (collapsed[cell]) propagate(i, y);
    }
  }

  for (auto i = 0; i < h; i++) {
    if (i != y && !collapsed[board[x + i * w]]) {
      auto &cell = board[x + i * w];
      cell &= rev;
      if (collapsed[cell]) propagate(x, i);
    }
  }

  for (auto i = 0; i < 3; i++) {
    for (auto j = 0; j < 3; j++) {
      const u8 sub_x = (x / 3) * 3 + i;
      const u8 sub_y = (y / 3) * 3 + j;

      if (sub_x != x && sub_y != y && !collapsed[board[sub_x + sub_y * w]]) {
        auto &cell = board[sub_x + sub_y * w];
        cell &= rev;
        if (collapsed[cell]) propagate(sub_x, sub_y);
      }
    }
  }
}

void print_solution() {
  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      const auto val = collapsed[board[j + i * w]];
      if (val) std::cout << (int)val << ' ';
      else
        std::cout << "? ";
      if (j % 3 == 2) std::cout << ' ';
    }
    std::cout << '\n';
    if (i % 3 == 2) std::cout << '\n';
  }
}

bool solve() {
  if (remaining == 0) {
    print_solution();
    return true;
  }

  auto lowest_entropy = 9;
  u8 lowest_entropy_x, lowest_entropy_y;

  for (auto i = 0; i < w; i++) {
    for (auto j = 0; j < h; j++) {
      const auto val = entropy[board[i + j * w]];
      if (val > 1 && val < lowest_entropy) {
        lowest_entropy = val;
        lowest_entropy_x = i;
        lowest_entropy_y = j;
      }
    }
  }

  const auto lowest_entropy_pos = lowest_entropy_x + lowest_entropy_y * w;

  auto mask = board[lowest_entropy_pos];

  u16 last[w * h];
  u8 last_remaining = remaining;
  std::copy(board, board + (w * h), last);

  while (mask) {
    auto val = mask & -mask;

    board[lowest_entropy_pos] = val;
    propagate(lowest_entropy_x, lowest_entropy_y);

    if (solve()) return true;

    mask ^= val;

    std::copy(last, last + (w * h), board);
    remaining = last_remaining;
  }

  return false;
}

int main() {
  std::fill(board, board + w * h, full);

  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      std::string buf;
      std::cin >> buf;
      if (buf != "-") {
        const auto val = std::stoi(buf);

        auto &cell = board[j + i * w];
        if (!collapsed[cell]) {
          cell = (1 << (val - 1));
          propagate(j, i);
        }
      }
    }
  }

  if (!solve()) std::cout << "Can't solve board!\n";
}
