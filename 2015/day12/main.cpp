#include <charconv>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

/*
Advent of Code 2015 – Day 12

    Problem:
        Sum the numbers in a given JSON document. For part two, ignore all
        objects containing the value "red"

    Approach:
        Uses a recursive JSON parser over a std::string_view of the input
        buffer.

        parse(...)
            - handles the current char
                - '{', calls parse_object(...)
                - '[', calls parse_array(...)
                - '"'
                    - checks if equal to "red" and flags
                    - otherwise converts char to int and sums
            - returns a Result

        parse_object(...)
            - recursively parses each key/value pair via parse(...)
            - accumulates nested sums and tracks if any value is "red"
            - if red, zeros out own sum

        parse_array(...)
            - recursively parses each element via parse(...)
            - accumulates child sums


    Complexity:
        O(n) time -- where n is char length on the input
        O(k) space -- where k is the maximum depth due to recursion
*/

struct Result {
  int sum;
  bool is_red;
  size_t pos;
};

Result parse(std::string_view sv, size_t pos);
Result parse_object(std::string_view sv, size_t pos);
Result parse_array(std::string_view sv, size_t pos);
int char_to_int(std::string_view sv, size_t pos, size_t* new_pos);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "please provide an input file as an argument\n";
    return -1;
  }

  std::ifstream file{argv[1], std::ios::ate};
  if (!file.is_open()) {
    throw std::runtime_error("could not open input file");
  }

  std::string buffer(file.tellg(), '\0');
  file.seekg(0);
  file.read(buffer.data(), buffer.size());

  Result result{parse(std::string_view(buffer), 0)};

  std::cout << "the sum is " << result.sum << '\n';
}

Result parse(std::string_view sv, size_t pos) {
  if (sv[pos] == '{') {
    return parse_object(sv, pos);
  }
  if (sv[pos] == '[') {
    return parse_array(sv, pos);
  }
  if (sv[pos] == '"') {
    size_t end{sv.find('"', pos + 1)};
    bool is_red{sv.substr(pos + 1, end - pos - 1) == "red"};
    return Result{0, is_red, end + 1};
  }

  size_t new_pos;
  int n{char_to_int(sv, pos, &new_pos)};
  return {n, false, new_pos};
}

Result parse_object(std::string_view sv, size_t pos) {
  ++pos;
  int sum{};
  bool is_red{false};
  while (sv[pos] != '}') {
    if (sv[pos] == ',' || sv[pos] == ':') {
      ++pos;
      continue;
    }
    Result result{parse(sv, pos)};
    sum += result.sum;
    if (result.is_red) {
      is_red = true;
    }
    pos = result.pos;
  }
  return Result{is_red ? 0 : sum, false, pos + 1};
}

Result parse_array(std::string_view sv, size_t pos) {
  ++pos;
  int sum{};
  while (sv[pos] != ']') {
    if (sv[pos] == ',') {
      ++pos;
      continue;
    }
    Result result{parse(sv, pos)};
    sum += result.sum;
    pos = result.pos;
  }
  return Result{sum, false, pos + 1};
}

int char_to_int(std::string_view sv, size_t pos, size_t* new_pos) {
  int result{};
  auto [ptr,
        ec]{std::from_chars(sv.data() + pos, sv.data() + sv.size(), result)};
  *new_pos = ptr - sv.data();
  return result;
}