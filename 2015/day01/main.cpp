#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

/*
    Advent of Code 2015 – Day 1

    Problem:
        Interpret '(' as up and ')' as down to track Santa's floor
        Determine the 1-indexed position of the first character that places
   Santa in the basement (floor -1)

    Approach:
        Read input file into string buffer, and read character by character
        Use `std::optional<size_t>` to track Santa's first trip to the basement
   and store the character's position

    Complexity:
        O(n) time
        O(1) space
 */

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "please provide an input file as an argument\n";
    return 1;
  }

  std::ifstream file{argv[1]};
  if (!file.is_open()) {
    throw std::runtime_error("could not open input file");
  }

  std::string buffer{std::istreambuf_iterator<char>{file}, {}};

  int floor{};
  std::optional<size_t> basement_tracking{};

  for (size_t i{}; i < buffer.size(); ++i) {
    const char& ch{buffer[i]};

    if (ch == '(') {
      ++floor;
    } else if (ch == ')') {
      --floor;
    }

    if (floor == -1 && !basement_tracking.has_value()) {
      basement_tracking = i + 1;
    }
  }

  std::cout << "Floor: " << floor << '\n';
  
  if (basement_tracking.has_value()) {
    std::cout << "First character that directs to basement: "
              << basement_tracking.value() << '\n';
  } else {
    std::cout << "never made it to the basement\n";
  }

  return 0;
}