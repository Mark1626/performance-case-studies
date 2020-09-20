#include "util.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

namespace internal {
FatalLogError::~FatalLogError() {
  std::cerr << "Fatal error " << str() << "\n";
  exit(EXIT_FAILURE);
}
} // namespace internal

Timer::Timer() : t1_(std::chrono::high_resolution_clock::now()) {}

double Timer::elapsed() {
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = t2 - t1_;
  return elapsed.count();
}

void print_usage() {
  std::cout << "Expecting bf [flags] -f <BF file>\n";
  std::cout << "\nSupported flags:\n";
  std::cout << "    -f           name of file\n";
  std::cout << "    -v           enable verbose output\n";
  std::cout << "    -h           print this message\n";
}

void parse_command_line(int argc, char **argv, std::string *bf_file_path,
                        bool *verbose) {
  *verbose = false;

  if (argc < 2) {
    print_usage();
    exit(1);
  }

  int c;
  while ((c = getopt(argc, argv, "f:vh")) != -1) {
    switch (c) {
    case 'f':
      if (optarg)
        *bf_file_path = optarg;
      break;
    case 'v':
      *verbose = true;
      break;
    case 'h':
      print_usage();
      break;
    default:
      break;
    }
  }
}
