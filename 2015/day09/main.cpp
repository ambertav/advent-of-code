#include <climits>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

/*
    Advent of Code 2015 – Day 9

    Problem:
        Find the shortest and longest distances for Santa to visit all cities
        in a Hamiltonian path

    Approach:
        Read entire input file into buffer and parse into `std::string_view`
        Use `sscanf` to extract city pairs and distances.

        While the number of cities is small, warranting a brute force solution,
        this approach utilizes Held-Karp algorithm for exploration

        Graph Construction:
            Parse each line into (from, to, distance) triplets.
            Map city names to indices for bitmask operations.
            Build a symmetric adjacency list storing edge weights
            Construct a adjacency matrix of distances for use in Held-Karp
            algorithm

        Held-Karp Algorithm:
            Maintain state of (mask, current) where mask is the set of visited
            cities and current is the index of the last visited city.

            Base case: all cities visited (mask == (1 << n) - 1) → return 0.
            Recursive step: for each unvisited city i, recurse with mask | (1 <<
            i) and take the optimal result via a comparator.

            Memoize results in a 2D table memo[1 << n][n]
            Run from each city as a starting point and track the best path


        Part 1 – Shortest Path:
            Run Held-Karp with `init_limit = INT_MAX` and `std::less`
            comparator.

        Part 2 – Longest Path:
            Reset memo table
            Run Held-Karp with `init_limit = INT_MIN` and `std::greater`
            comparator.

    Complexity:
        O(2^n * n^2) time  -- Held-Karp over n cities
        O(2^n * n)   space
*/

int held_karp(int mask, int current, int n,
              const std::vector<std::vector<int>>& dist,
              std::vector<std::vector<int>>& memo, int init_limit,
              std::function<bool(int, int)> comparator);

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

  std::unordered_map<
      std::string /* city */,
      std::vector<std::pair<std::string /* city */, int /* distance */>>>
      adjacency_list{};

  std::unordered_map<std::string, size_t> cities{};
  size_t index{};

  size_t pos{};
  while (pos < buffer.size()) {
    size_t end{buffer.find('\n', pos)};
    if (end == std::string::npos) {
      end = buffer.size();
    }

    std::string_view line{buffer.data() + pos, end - pos};

    char from[20], to[20];
    int distance{};
    std::sscanf(line.data(), "%s to %s = %d", &from, &to, &distance);

    // map city names to integers for use in bitmask
    if (!cities.contains(from)) {
      cities[from] = index++;
    }

    if (!cities.contains(to)) {
      cities[to] = index++;
    }

    adjacency_list[from].emplace_back(to, distance);
    adjacency_list[to].emplace_back(from, distance);

    pos = end + 1;
  }

  // adjacency matrix of distances
  // where indices correspond to cities
  int n{static_cast<int>(cities.size())};
  std::vector<std::vector<int /* distance */>> dist(
      n, std::vector<int>(n, INT_MAX));
  std::vector<std::vector<int /* distance */>> memo(1 << n,
                                                    std::vector<int>(n, -1));

  // constructs 2D distances vector
  for (const auto& [from, connections] : adjacency_list) {
    for (const auto& [to, distance] : connections) {
      dist[cities.at(from)][cities.at(to)] = distance;
    }
  }

  int shortest_distance{INT_MAX};
  int longest_distance{INT_MIN};
  for (const auto& [city, i] : cities) {
    int curr{static_cast<int>(i)};

    shortest_distance = std::min(
        shortest_distance,
        held_karp(1 << curr, curr, n, dist, memo, INT_MAX, std::less<int>{}));

    std::fill(memo.begin(), memo.end(), std::vector<int>(n, -1));

    longest_distance =
        std::max(longest_distance, held_karp(1 << curr, curr, n, dist, memo,
                                             INT_MIN, std::greater<int>{}));
  }

  std::cout << "shortest path is " << shortest_distance << '\n';
  std::cout << "longest path is " << longest_distance << '\n';

  return 0;
}

int held_karp(int mask, int current, int n,
              const std::vector<std::vector<int>>& dist,
              std::vector<std::vector<int>>& memo, int init_limit,
              std::function<bool(int, int)> comparator) {
  if (mask == (1 << n) - 1) {
    return 0;
  }

  if (memo[mask][current] != -1) {
    return memo[mask][current];
  }

  int best{init_limit};
  for (int i{}; i < n; ++i) {
    if (!(mask & (1 << i)) && dist[current][i] != init_limit) {
      int result{
          held_karp(mask | (1 << i), i, n, dist, memo, init_limit, comparator)};

      if (result != init_limit && comparator(result, best)) {
        best = dist[current][i] + result;
      }
    }
  }

  memo[mask][current] = best;
  return best;
}
