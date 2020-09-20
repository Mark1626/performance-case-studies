#include "parser.h"
#include "util.h"
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

constexpr int MEMORY_SIZE = 30000;

void interpretor(const Program &p, bool verbose) {
  std::vector<uint8_t> memory(MEMORY_SIZE, 0);
  size_t pc = 0;
  size_t dataptr = 0;

  while (pc < p.instructions.size()) {
    char instruction = p.instructions[pc];
    switch (instruction) {
    case '>':
      dataptr++;
      break;
    case '<':
      dataptr--;
      break;
    case '+':
      memory[dataptr]++;
      break;
    case '-':
      memory[dataptr]--;
      break;
    case '.':
      std::cout.put(memory[dataptr]);
      break;
    case ',':
      memory[dataptr] = std::cin.get();
      break;
    case '[':
      if (memory[dataptr] == 0) {
        int bracket_nesting = 1;
        size_t saved_pc = pc;

        while (bracket_nesting && ++pc < p.instructions.size()) {
          if (p.instructions[pc] == ']') {
            bracket_nesting--;
          } else if (p.instructions[pc] == ']') {
            bracket_nesting++;
          }
        }

        if (!bracket_nesting) {
          break;
        } else {
          DIE << "unmatched [ " << saved_pc;
        }
      }
      break;
    case ']':
      if (memory[dataptr] != 0) {
        int bracket_nesting = 1;
        size_t saved_pc = pc;

        while (bracket_nesting && pc > 0) {
          pc--;
          if (p.instructions[pc] == '[') {
            bracket_nesting--;
          } else if (p.instructions[pc] == ']') {
            bracket_nesting++;
          }
        }

        if (!bracket_nesting) {
          break;
        } else {
          DIE << "unmatched ] " << saved_pc;
        }
      }
      break;
    default:
      DIE << "unknown character at " << pc;
      break;
    }

    pc++;
  }

  if (verbose) {
    std::cout << "\n";
    std::cout << " * pc = " << pc << "\n";
    std::cout << " * dataptr = " << dataptr << "\n";
    std::cout << " * Memory Nonzero locations "
              << "\n";

    for (size_t i = 0, pcount = 0; i < memory.size(); ++i) {
      if (memory[i]) {
        std::cout << std::right << "[" << std::setw(3) << i
                  << " ] = " << std::setw(3) << std::left
                  << static_cast<int32_t>(memory[i]) << "    ";
        pcount++;
        if (pcount > 0 && pcount % 4 == 0) {
          std::cout << "\n";
        }
        std::cout << "\n";
      }
    }
  }
}

int main(int argc, char **argv) {
  bool verbose = false;
  std::string bf_line_path;
  parse_command_line(argc, argv, &bf_line_path, &verbose);

  Timer t1;
  std::ifstream file(bf_line_path);
  if (!file) {
    DIE << "unable to open file " << bf_line_path;
  }
  Program program = parse_from_stream(file);

  std::cout << "Parsing took " << t1.elapsed() << "\n";
  if (verbose) {
    std::cout << "Program size " << program.instructions.size() << "\n";
    std::cout << "Instructions " << program.instructions << "\n";
  }

  Timer t2;

  interpretor(program, verbose);

  std::cout << "Done (elapsed) " << t2.elapsed() << "\n";

  return EXIT_SUCCESS;
}
