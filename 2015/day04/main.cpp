#include <openssl/md5.h>

#include <iostream>

/*
    Advent of Code 2015 – Day 4

  Problem:
        Find the lowest positive number that produces an MD5 hash starting
        with five or six leading zeros in hexadecimal

    Approach:
        Concatenate secret key with counter to form input string
        Calculate MD5 hash of the input string using OpenSSL
        Check bytes of hash for leading zeros without full hex conversion

    Complexity:
        O(k) time -- where k is the answer (number of iterations needed)
        O(1) space
*/

bool check_hash(unsigned char* hash);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "please provide a secret key as an argument\n";
    return -1;
  }

  const std::string key{std::string(argv[1])};

  int k{};
  while (true) {
    unsigned char hash[16];
    std::string input{key + std::to_string(k)};

    // openssl MD5 implementation
    MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(),
        hash);

    if (check_hash(hash)) {
      break;
    }
    ++k;
  }

  std::cout << "lowest positive number: " << k << '\n';

  return 0;
}

bool check_hash(unsigned char* hash) {
  /*
  to check a hash that with 5 zeros in hex
  return hash[0] == 0 && hash[1] == 0 && hash[2] < 16;
  */

  // to check a hash that with 6 zeros in hex
  return hash[0] == 0 && hash[1] == 0 && hash[2] < 16;
}