#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>

/*
Advent of Code 2015 – Day 13

    Problem:
        Find the optimal seating arrangement between guests with various
        happiness levels sitting next to each other

    Approach:
        Parse the seating arrangement input to construct an adjacency matrix and
        solve as a closed path variant of Traveling Salesman Probelm using Held-Karp
        algorithm

        The adjacency matrix is n x n and holds the combined happiness of
        seating the pair together via summing both directions

        The dp table is (1 << n) x n, where `dp[mask][i]` holds the best
        happiness for a partial arrangement seating exactly the people in mask, 
        ending with person i

        The final answer is the maximum happiness value over i of   
        `dp[full_mask][i] + adjacency_matrix[i][0]`, closing the path back 
        to the first person

        
    Complexity:
        O(2^n * n^2) time, where n is the number of people
        O(2^n * n) space, for the dp table
*/

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

  struct Seating {
    std::string name;
    std::string to;
    int value;
  };

  size_t current_index{};
  std::unordered_map<std::string, size_t> name_to_index{};
  std::vector<Seating> arrangements{};

  size_t pos{};

  while (pos < buffer.size()) {
    size_t line_end{buffer.find('\n', pos)};
    if (line_end == std::string::npos) {
      line_end = buffer.size();
    }

    std::string_view line{buffer.data() + pos, line_end - pos};

    char p1[32], gain_or_lose[8], p2[32];
    int happiness;

    std::sscanf(line.data(),
                "%31s would %7s %d happiness units by sitting next to %31[^.].",
                p1, gain_or_lose, &happiness, p2);

    if (std::string(gain_or_lose) == "lose") {
      happiness *= -1;
    }

    std::string person_one(p1);
    std::string person_two(p2);

    // contruct mapping of name to index for adj matrix
    if (!name_to_index.contains(person_one)) {
      name_to_index[person_one] = current_index++;
    }
    if (!name_to_index.contains(person_two)) {
      name_to_index[person_two] = current_index++;
    }

    arrangements.emplace_back(person_one, person_two, happiness);

    pos = line_end + 1;
  }

  // add self to seating
  size_t n{current_index + 1};

  std::vector<std::vector<int>> adjacency_matrix(
      n, std::vector<int>(n, 0));  // n x n

  for (const auto& arrangement : arrangements) {
    size_t i{name_to_index.at(arrangement.name)};
    size_t j{name_to_index.at(arrangement.to)};

    adjacency_matrix[i][j] += arrangement.value;
    adjacency_matrix[j][i] += arrangement.value;
  }

  std::vector<std::vector<int>> dp(
      1u << n,
      std::vector<int>(n, std::numeric_limits<int>::min()));  // (1 << n) x n

  dp[1][0] = 0;

  for (size_t mask{}; mask < (1u << n); ++mask) {
    for (size_t i{}; i < n; ++i) {
      if (!(mask & (1u << i)) ||
          dp[mask][i] == std::numeric_limits<int>::min()) {
        continue;
      }

      for (size_t j{}; j < n; ++j) {
        if (mask & (1u << j)) {
          continue;
        }

        size_t new_mask{mask | (1u << j)};
        dp[new_mask][j] =
            std::max(dp[new_mask][j], (dp[mask][i] + adjacency_matrix[i][j]));
      }
    }
  }

  size_t full_mask{(1u << n) - 1};
  int best{std::numeric_limits<int>::min()};

  for (size_t i{}; i < n; ++i) {
    if (dp[full_mask][i] == std::numeric_limits<int>::min()) {
      continue;
    }
    best = std::max(best, (dp[full_mask][i] + adjacency_matrix[i][0]));
  }

  std::cout << "optimal seating arrangement happiness level is " << best
            << '\n';

  return 0;
}