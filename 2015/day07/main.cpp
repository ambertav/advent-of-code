#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

/*
    Advent of Code 2015 – Day 7

    Problem:
        Simulate a circuit of bitwise logic gates where wires carry 16-bit
   signals.

    Approach:
        Read entire input file into buffer and parse into `std::string_view`

        Parse each instruction using `std::sscanf` to extract:
            - Operation type
            - Input operands (wire names or numeric literals)
            - Output wire name

        Define enum for operations (ASSIGN, AND, OR, NOT, LSHIFT, RSHIFT)
        Store instructions in `std::unordered_map`

        Lazy evaluation with memoization:
            - Recursive lambda wrapped in `std::function` to enable
   self-reference
            - Handle numeric literals
            - Cache computed wire values in `std::unordered_map`
            - Recursively evaluate output dependencies

    Complexity:
        O(n) time -- where n is number of wires, each evaluated once due to
   cache O(n) space -- storing instructions and cached values for all wires
*/

enum class Operation { ASSIGN, NOT, AND, OR, LSHIFT, RSHIFT };

struct Instruction {
  Operation operation;
  std::string lhs;
  std::string rhs;
  int shift;
};

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

  std::unordered_map<std::string /* wire */, Instruction> instructions{};
  std::unordered_map<std::string /* wire */, uint16_t /* 16-bit signal */>
      cache{};

  size_t pos{};
  while (pos < buffer.size()) {
    size_t end{buffer.find('\n', pos)};
    if (end == std::string::npos) {
      end = buffer.size();
    }

    std::string_view line{buffer.data() + pos, end - pos};

    char output[10], lhs[10], rhs[10];
    int shift{};

    if (std::sscanf(line.data(), "%s -> %s", &lhs, &output) == 2) {
      // ASSIGN
      instructions[output] = Instruction{Operation::ASSIGN, lhs, "", 0};

    } else if (std::sscanf(line.data(), "NOT %s -> %s", &lhs, &output) == 2) {
      // NOT
      instructions[output] = Instruction{Operation::NOT, lhs, "", 0};

    } else if (std::sscanf(line.data(), "%s AND %s -> %s", &lhs, &rhs,
                           &output) == 3) {
      // AND
      instructions[output] = Instruction{Operation::AND, lhs, rhs, 0};

    } else if (std::sscanf(line.data(), "%s OR %s -> %s", &lhs, &rhs,
                           &output) == 3) {
      // OR
      instructions[output] = Instruction{Operation::OR, lhs, rhs, 0};

    } else if (std::sscanf(line.data(), "%s LSHIFT %d -> %s", &lhs, &shift,
                           &output) == 3) {
      // LSHIFT
      instructions[output] = Instruction{Operation::LSHIFT, lhs, "", shift};

    } else if (std::sscanf(line.data(), "%s RSHIFT %d -> %s", &lhs, &shift,
                           &output) == 3) {
      // RSHIFT
      instructions[output] = Instruction{Operation::RSHIFT, lhs, "", shift};
    }

    pos = end + 1;
  }

  // std::function enables lambda to reference self for recursion
  std::function<uint16_t(const std::string&)> get_signal =
      [&](const std::string& wire) -> uint16_t {
    if (!wire.empty() && std::isdigit(wire.front())) {
      return static_cast<uint16_t>(std::stoi(wire));
    }

    if (cache.contains(wire)) {
      return cache[wire];
    }

    auto it{instructions.find(wire)};
    if (it == instructions.end()) {
      throw std::invalid_argument(
          "instruction mapping does not contain specified wire: " + wire);
    }

    Instruction& instruction{it->second};
    uint16_t result{};

    switch (instruction.operation) {
      case Operation::ASSIGN:
        // ASSIGN
        result = get_signal(instruction.lhs);
        break;
      case Operation::NOT:
        // NOT
        result = ~(get_signal(instruction.lhs));
        break;
      case Operation::AND:
        // AND
        result = get_signal(instruction.lhs) & get_signal(instruction.rhs);
        break;
      case Operation::OR:
        // OR
        result = get_signal(instruction.lhs) | get_signal(instruction.rhs);
        break;
      case Operation::LSHIFT:
        // LSHIFT
        result = get_signal(instruction.lhs) << instruction.shift;
        break;
      case Operation::RSHIFT:
        // RSHIFT
        result = get_signal(instruction.lhs) >> instruction.shift;
        break;
    }

    cache[wire] = result;
    return result;
  };

  uint16_t a_signal1{get_signal("a")};
  std::cout << "Part 1: the signal provided to wire a is " << a_signal1 << '\n';

  cache.clear();
  cache["b"] = a_signal1;
  uint16_t a_signal2{get_signal("a")};
  std::cout << "Part 2: the signal provided to wire a is " << a_signal2 << '\n';

  return 0;
}