#include <algorithm>
#include <array>
#include <charconv>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

/*
    Advent of Code 2015 – Day 2

  Problem:
        Calculate wrapping paper and ribbon needed for presents

    Approach:
        Read input file into string buffer, parse on delimiter 'x' and convert `std::string_view` into integers 
        Sort dimensions to easily identify smallest values
        Calculate wrapping paper (surface area + slack) and ribbon (perimeter + bow)

    Complexity:
        O(n) time - constant time per line
        O(n) space - stores entire input file in memory
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

  auto char_to_int = [](std::string_view sv) -> int {
    int result{};
    auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.size(), result)};
    if (ec == std::errc{}) {
      return result;
    } else {
      return -1;
    }
  };

  int total_wp{};
  int total_rib{};
  std::array<int, 3> dims{};

  size_t pos{};
  while (pos < buffer.size()) {
    size_t end{buffer.find('\n', pos)};
    if (end == std::string::npos) {
      end = buffer.size();
    }

    size_t idx{};
    size_t dim_start{};
    std::string_view line{buffer.data() + pos, end - pos};

    for (size_t i{}; i <= line.size(); ++i) {
      if (i == line.size() || line[i] == 'x') {
        std::string_view val_sv{line.data() + dim_start, i - dim_start};
        int val{char_to_int(val_sv)};
        if (val == -1) {
          throw std::runtime_error("invalid line format\n");
        }
        dims[idx++] = val;
        dim_start = i + 1;
      }
    }

    if (idx != 3) {
      throw std::runtime_error("expected 3 dimensions per line\n");
    }

    std::sort(dims.begin(), dims.end());

    auto [side1, side2, side3] = dims;

    // 2*l*w + 2*w*h + 2*h*l + min_area
    total_wp += ((2 * side1 * side2) + (2 * side2 * side3) +
                 (2 * side3 * side1) + (side1 * side2));

    // l*w*h + min_perimeter
    total_rib += ((side1 * side2 * side3) + (2 * (side1 + side2)));

    pos = end + 1;
  }

  std::cout << "Wrapping paper: " << total_wp << " sqft\n";
  std::cout << "Ribbon: " << total_rib << " ft\n";

  return 0;
}