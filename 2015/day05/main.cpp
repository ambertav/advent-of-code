#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

/*
    Advent of Code 2015 – Day 5

    Problem:
        Determine if strings are "nice" based on two different rule sets.

    Approach:
        Read entire input file into buffer and parse into `std::string_view` to
   avoid copies

        Part 1: Single pass through each string checking all three conditions
            - Check vowel count
            - `std::ranges::adjacent_find` for consecutive characters
            - `std::ranges::any_of` with substring search for "naughty" patterns

        Part 2: Single pass with hash map to track pair positions
            - Store first occurrence position of each character pair
            - Check distance between pair occurrences to ensure non-overlapping
            - Check for xyx pattern by comparing characters (at i-1 and i+1)

    Complexity:
        O(n*m) time -- where n is number of strings, m is average string length
        O(m) space -- for storing unique pairs per string
*/

constexpr std::string_view VOWELS{"aeiou"};
constexpr std::array<std::string_view, 4> COMBOS{"ab", "cd", "pq", "xy"};

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
  int count{};

  auto is_nice = [](std::string_view line) -> bool {
    std::unordered_map<std::string_view, size_t> pair_positions{};
    bool twice_no_overlap{};
    bool repeat_with_inbetween{};

    for (size_t i{1}; i < line.size(); ++i) {
      if (i + 1 < line.size() && line[i - 1] == line[i + 1]) {
        repeat_with_inbetween = true;
      }

      std::string_view pair{line.substr(i - 1, 2)};
      auto [it, inserted] = pair_positions.try_emplace(pair, i - 1);
      if (!inserted && (i - 1) - it->second >= 2) {
        twice_no_overlap = true;
      }

      if (twice_no_overlap && repeat_with_inbetween) {
        break;
      }
    }

    return twice_no_overlap && repeat_with_inbetween;
  };

  size_t pos{};
  while (pos < buffer.size()) {
    size_t end{buffer.find('\n', pos)};
    if (end == std::string::npos) {
      end = buffer.size();
    }

    std::string_view line{buffer.data() + pos, end - pos};

    /*

    Part One Rules

    bool has_naughty_combo{
        std::ranges::any_of(COMBOS, [&line](std::string_view naughty) {
          return line.find(naughty) != std::string_view::npos;
        })};

    if (!has_naughty_combo) {
      auto vowel_count{std::ranges::count_if(line, [&](char c) {
        return VOWELS.find(c) != std::string_view::npos;
      })};
      bool twice_in_row{std::ranges::adjacent_find(line) != line.end()};

      if (twice_in_row && vowel_count >= 3) {
        ++count;
      }
    }
    */

    if (is_nice(line)) {
      ++count;
    }

    pos = end + 1;
  }

  std::cout << count << " strings are nice\n";

  return 0;
}