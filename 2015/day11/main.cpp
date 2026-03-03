#include <algorithm>
#include <iostream>
#include <string>

/*
Advent of Code 2015 – Day 11

    Problem:
        Find the next password that meet a set a rules by incrementing the current password

    Approach:
        Uses a while loop to increment the password string like a base-26 number, 
        handling carry-over and wrap-around from 'z' to 'a'. 

        After each increment, a lambda is invoked to validate the password:
            - Checks for forbidden characters (i, o, and l)
            - Scans the string for an increasing triplet of consecutive letters
            - Uses `std::adjacent_find` to detect two non-overlapping pairs of identical characters

    Complexity:
        O(n) time -- where n is the number of increments needed to reach a valid password
        O(1) space
*/

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "please provide an input as an agrument\n";
    return -1;
  }

  std::string password{argv[1]};
  bool valid{false};

  auto validate_password = [](const std::string& password) -> bool {
    if (password.find('i') != std::string::npos ||
        password.find('o') != std::string::npos ||
        password.find('l') != std::string::npos) {
      return false;
    }

    bool increasing_triplet{};
    for (size_t i{}; i < password.size(); ++i) {
      if (i > password.size() - 3 && !increasing_triplet) {
        return false;
      }

      if (!increasing_triplet && password[i] + 1 == password[i + 1] &&
          password[i] + 2 == password[i + 2]) {
        increasing_triplet = true;
      }
    }

    auto first_it{std::adjacent_find(password.begin(), password.end())};
    auto second_it{std::adjacent_find(first_it + 2, password.end())};
    if (first_it == password.end() || second_it == password.end()) {
      return false;
    }

    return true;
  };

  size_t n{password.size() - 1};
  while (!valid) {
    if (password[n] == 'z') {
      password[n] = 'a';

      size_t carry_index{n - 1};
      while (carry_index > 0) {
        if (password[carry_index] == 'z') {
          password[carry_index] = 'a';
          --carry_index;
        } else {
          ++password[carry_index];
          break;
        }
      }
    } else {
      ++password[n];
    }

    valid = validate_password(password);
  }

  std::cout << "next password would be: " << password << '\n';

  return 0;
}