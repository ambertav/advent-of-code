#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

/*
    Advent of Code 2015 – Day 6

    Problem:
        Read in instructions to turn on, turn off, or toggle light in a
   1000x1000 grid.

        Part 1: Count lights that are on
        Part 2: Sum and track brightness levels

    Approach:
        Read entire input file into buffer and parse into `std::string_view`

        Use 2D vector (1000x1000) to represent grid state
        - Part 1: `std::vector<std::vector<bool>>` for on/off state
        - Part 2: `std::vector<std::vector<int>>` for brightness levels

        Parse each instruction using `std::sscanf` to extract:
        - Action type (turn on, turn off, toggle)
        - Coordinate ranges (x1,y1 through x2,y2)

        Iterate through the specified rectangular regions and apply instructions

        Count result using `std::ranges::count` with flattened view
        Iterate and sum brightness levels across all lights

    Complexity:
        O(n*a) time -- where n is number of instructions, a is average area per
   instruction O(1) space
*/

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "please provide an input file as an argument\n";
    return -1;
  }

  std::ifstream file{argv[1]};
  if (!file.is_open()) {
    throw std::runtime_error("could not open input file");
  }

  std::string buffer{std::istreambuf_iterator<char>{file}, {}};
  std::vector<std::vector<int>> grid(
      1000, std::vector<int>(1000, 0));  // 1000 x 1000 elements, all zeros

  size_t pos{};
  while (pos < buffer.size()) {
    size_t end{buffer.find('\n', pos)};
    if (end == std::string::npos) {
      end = buffer.size();
    }

    std::string_view line{buffer.data() + pos, end - pos};

    size_t x_1{}, y_1{};
    size_t x_2{}, y_2{};

    bool turn_on{}, turn_off{}, toggle{};

    // parse line for action and coordinate range
    if (std::sscanf(line.data(), "turn on %d,%d through %d,%d", &x_1, &y_1,
                    &x_2, &y_2) == 4) {
      turn_on = true;
    } else if (std::sscanf(line.data(), "turn off %d,%d through %d,%d", &x_1,
                           &y_1, &x_2, &y_2) == 4) {
      turn_off = true;
    } else if (std::sscanf(line.data(), "toggle %d,%d through %d,%d", &x_1,
                           &y_1, &x_2, &y_2) == 4) {
      toggle = true;
    }

    for (size_t y{y_1}; y <= y_2; ++y) {
      auto& row{grid[y]};
      for (size_t x{x_1}; x <= x_2; ++x) {
        if (turn_on) {
          ++row[x];
        } else if (turn_off && row[x] > 0) {
          --row[x];
        } else if (toggle) {
          row[x] += 2;
        }
      }
    }

    pos = end + 1;
  }

  auto count{std::ranges::count_if(grid | std::ranges::views::join,
                                   [](int lit) { return lit > 0; })};

  int brightness{};
  for (const auto& row : grid) {
    for (const auto& value : row) {
      brightness += value;
    }
  }

  std::cout << count << " lights are lit\n";
  std::cout << "the total brightness is " << brightness << '\n';

  return 0;
}