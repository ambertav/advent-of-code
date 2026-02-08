#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>

/*
    Advent of Code 2015 – Day 3

  Problem:
        Calculate the number of houses that received at least one present from
        either Santa or Robo Santa

    Approach:
        Read input file into string buffer, and read character by character
        Track Santa and Robo Santa's locations using even/odd indices
        Use `std::unordered_set` to store unqiue coordinate pairs for visited
  houses

    Complexity:
        O(n) time -- where n is the number of directions
        O(n) space -- for storing unique house coordinates
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

  struct PairHash {
    std::size_t operator()(const std::pair<int, int>& pair) const {
      size_t h1{std::hash<int>{}(pair.first)};
      size_t h2{std::hash<int>{}(pair.second)};

      return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
  };
  std::unordered_set<std::pair<int /* x-coordinate */, int /* y-coordinate */>,
                     PairHash>
      deliveries{};

  int s_x{}, r_x{};
  int s_y{}, r_y{};
  deliveries.insert({s_x, s_y});  // initial delivery at starting location

  for (size_t i{}; i < buffer.size(); ++i) {
    int& x{(i % 2 == 0) ? s_x : r_x};
    int& y{(i % 2 == 0) ? s_y : r_y};

    switch (buffer[i]) {
      case '>':
        ++x;
        break;
      case '<':
        --x;
        break;
      case '^':
        ++y;
        break;
      case 'v':
        --y;
        break;
      default:
        break;
    }
    deliveries.insert({x, y});
  }

  std::cout << deliveries.size() << " houses received at least one present\n";

  return 0;
}