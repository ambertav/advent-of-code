#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

/*
    Advent of Code 2015 – Day 8

    Problem:
        Compute the difference between the number of characters of code and the
   number of characters in memory, then compute the difference between the
   encoded length and the original code length.

    Approach:
        Read entire input file into buffer and parse into `std::string_view`

        Part 1 – Code vs Memory:
            Accumulate raw line length into total_in_code.
            Walk char by char between the surrounding quotes:
                - `\\` or `\"` → advance 2, count 1 memory char
                - `\x??`       → advance 4, count 1 memory char
                - otherwise    → advance 1, count 1 memory char

        Part 2 – Encoding:
            Base encoded length per line = line.size() + 4
                (+2 for new surrounding quotes, +2 for escaping existing quotes)
            Walk char by char between the surrounding quotes:
                - `\\` or `\"` → advance 2, add 2 (both chars need escaping)
                - `\x??`       → advance 4, add 1 (only the `\` needs escaping)

    Complexity:
        O(n) time -- where n is total characters
        O(n) space
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

  int total_in_code{};
  int total_in_memory{};
  int total_to_encode{};

  size_t pos{};
  while (pos < buffer.size()) {
    size_t end{buffer.find('\n', pos)};
    if (end == std::string::npos) {
      end = buffer.size();
    }

    std::string_view line{buffer.data() + pos, end - pos};
    total_in_code += line.size();
    total_to_encode += line.size() + 4;  // add "" and escape chars

    // bounds skip opening and closing "
    size_t i{1};
    while (i < line.size() - 1) {
      if (line[i] == '\\') {
        if (line[i + 1] == '\\' || line[i + 1] == '"') {
          i += 2;
          total_to_encode += 2;  // add escapes to both
        } else if (line[i + 1] == 'x') {
          i += 4;
          total_to_encode += 1;  // add an escape
        }
      } else {
        ++i;
      }
      ++total_in_memory;
    }

    pos = end + 1;
  }

  std::cout
      << "Difference between total number of characters in code vs in memory:  "
      << total_in_code - total_in_memory << '\n';

  std::cout << "Difference between total number of characters to encode the "
               "string vs in code:  "
            << total_to_encode - total_in_code << '\n';

  return 0;
}