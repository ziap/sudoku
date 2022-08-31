#include <array>
#include <bit>
#include <iostream>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

constexpr u8 w = 9;
constexpr u8 h = 9;

constexpr u16 full = (1 << 9) - 1;

constexpr auto entropy = ([]() constexpr {
  constexpr auto len = 1 << 9;
  std::array<u8, len> result;
  for (auto i = 0u; i < len; i++) result[i] = std::popcount(i);
  return result;
})();

constexpr auto collapsed_value = ([]() constexpr {
  constexpr auto len = 1 << 9;
  std::array<u8, len> result = {0};
  for (auto i = 0u; i < 9; i++) result[1 << i] = i + 1;
  return result;
})();

u16 board[w * h];

u8 remaining = 81;
u32 solutions = 0;

bool propagate(u8 x, u8 y) {
  remaining--;

  const auto rev = (~board[x + y * w] & full);

  for (auto i = 0; i < w; i++) {
    if (i != x) {
      auto &cell = board[i + y * w];
      if ((cell & rev) == 0) return false;
      if (collapsed_value[cell]) continue;
      cell &= rev;
      if (collapsed_value[cell] && !propagate(i, y)) return false;
    }
  }

  for (auto i = 0; i < h; i++) {
    if (i != y) {
      auto &cell = board[x + i * w];
      if ((cell & rev) == 0) return false;
      if (collapsed_value[cell]) continue;
      cell &= rev;
      if (collapsed_value[cell] && !propagate(x, i)) return false;
    }
  }

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

void print_solution() {
  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      const auto val = collapsed_value[board[j + i * w]];
      if (val) std::cout << (int)val << ' ';
      else
        std::cout << "! ";
      if (j % 3 == 2) std::cout << ' ';
    }
    std::cout << '\n';
    if (i % 3 == 2) std::cout << '\n';
  }
}

void solve() {
  if (remaining == 0) {
    solutions++;
    print_solution();
    return;
  }

  u8 empty_x, empty_y;

  for (auto i = 0;; i++) {
    const auto val = entropy[board[i]];
    if (val > 1) {
      empty_x = i % w;
      empty_y = i / w;
      break;
    }
  }

  const auto lowest_entropy_pos = empty_x + empty_y * w;

  auto mask = board[lowest_entropy_pos];

  u16 last[w * h];
  auto last_remaining = remaining;
  std::copy(board, board + (w * h), last);

  while (mask) {
    auto val = mask & -mask;

    board[lowest_entropy_pos] = val;
    if (propagate(empty_x, empty_y)) solve();

    mask ^= val;

    std::copy(last, last + (w * h), board);
    remaining = last_remaining;
  }
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
        if (!collapsed_value[cell]) {
          cell = (1 << (val - 1));
          propagate(j, i);
        }
      }
    }
  }

  solve();

  std::cout << "Found " << solutions << " solutions\n";
}
