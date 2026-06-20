#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

/*
Advent of Code 2015 - Day 14

    Problem:
        Determine the lead distance and points for the reindeer olympics race
        given input about the reindeer's speed and fly/rest duration requirements.

    Approach:
        Parse through the input to construct a vector of `Reindder` to hold
        their speed values, fly and rest durations along with mutable fields like
        distance and points. For each second, iterate through the vector of reindeer and
        update its position according to whether or not its flying or resting. Then,
        iterate through the vector of reindeer to award a point to the reindeer(s)
        currently in the lead.


    Complexity:
        O(n * k) time, where n is the number of reindeer and k is the race duration
        O(n) space, for the vector of reindeer
*/

constexpr int SECONDS_PASSED{2503};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "please provide an input file as an argument\n";
    return -1;
  }

  std::ifstream file{argv[1], std::ios::ate};
  if (!file.is_open()) {
    throw std::runtime_error("could not ope input file");
  }

  std::string buffer(file.tellg(), '\0');
  file.seekg(0);
  file.read(buffer.data(), buffer.size());

  struct Reindeer {
    std::string name;
    int speed;
    int fly_duration;
    int rest_duration;

    int distance;
    int points;
    int duration;
    bool is_flying;
  };

  std::vector<Reindeer> reindeers{};
  size_t pos{};

  while (pos < buffer.size()) {
    size_t line_end{buffer.find('\n', pos)};
    if (line_end == std::string::npos) {
      line_end = buffer.size();
    }

    std::string line{buffer.data() + pos, line_end - pos};

    char name[32];
    int speed, fly, rest;

    std::sscanf(line.data(),
                "%31s can fly %d km/s for %d seconds, but then must rest for "
                "%d seconds.",
                name, &speed, &fly, &rest);

    reindeers.emplace_back(std::string(name), speed, fly, rest,
                           0 /* distance */, 0 /* points */, 0 /* duration */,
                           true /* is_flying */);

    pos = line_end + 1;
  }

  int winning_distance{std::numeric_limits<int>::min()};
  int winning_points{std::numeric_limits<int>::min()};

  for (size_t i{}; i < SECONDS_PASSED; ++i) {
    // update reindeer movement
    for (auto& reindeer : reindeers) {
      ++reindeer.duration;

      if (reindeer.is_flying) {
        reindeer.distance += reindeer.speed;
        winning_distance = std::max(winning_distance, reindeer.distance);
      }

      if ((reindeer.is_flying && reindeer.duration >= reindeer.fly_duration) ||
          (!reindeer.is_flying &&
           reindeer.duration >= reindeer.rest_duration)) {
        reindeer.duration = 0;
        reindeer.is_flying = !reindeer.is_flying;
      }
    }

    // score reindeers
    for (auto& reindeer : reindeers) {
      if (reindeer.distance == winning_distance) {
        ++reindeer.points;
        winning_points = std::max(winning_points, reindeer.points);
      }
    }
  }

  std::cout << "the winning distance is " << winning_distance << '\n';
  std::cout << "the highest point value is " << winning_points << '\n';

  return 0;
}