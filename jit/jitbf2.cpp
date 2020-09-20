#include "parser.h"
#include "util.h"
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

constexpr int MEMORY_SIZE = 30000;

enum class BfOpKind {
  INVALID_OP = 0,
  INC_PTR,
  DEC_PTR,
  INC_DATA,
  DEC_DATA,
  READ_STDIN,
  WRITE_STDOUT,
  JUMP_IF_DATA_ZERO,
  JUMP_IF_DATA_NOT_ZERO
};

const char *BfOpKind_name(BfOpKind kind) {
  switch (kind) {
  case BfOpKind::INVALID_OP:
    return "x";
  case BfOpKind::INC_PTR:
    return ">";
  case BfOpKind::DEC_PTR:
    return "<";
  case BfOpKind::INC_DATA:
    return "+";
  case BfOpKind::DEC_DATA:
    return "-";
  case BfOpKind::READ_STDIN:
    return ",";
  case BfOpKind::WRITE_STDOUT:
    return ".";
  case BfOpKind::JUMP_IF_DATA_ZERO:
    return "[";
  case BfOpKind::JUMP_IF_DATA_NOT_ZERO:
    return "]";
  }
  return nullptr;
}

struct BfOp {
  BfOp(BfOpKind kind_param, size_t argument_param)
      : kind(kind_param), argument(argument_param){};
  void serialize(std::string *s) const {
    *s += BfOpKind_name(kind) + std::to_string(argument);
  }
  BfOpKind kind = BfOpKind::INVALID_OP;
  size_t argument = 0;
};

std::vector<BfOp> translate_program(const Program &p) {
  size_t pc = 0;
  size_t program_size = p.instructions.size();
  std::vector<BfOp> ops;

  std::stack<size_t> open_bracket_stack;

  while (pc < program_size) {
    char instruction = p.instructions[pc];
    if (instruction == '[') {
      open_bracket_stack.push(ops.size());
      ops.push_back(BfOp(BfOpKind::JUMP_IF_DATA_ZERO, 0));
      pc++;
    } else if (instruction == ']') {
      if (open_bracket_stack.empty()) {
        DIE << "unmatched ] " << pc;
      }
      size_t open_bracket_offset = open_bracket_stack.top();
      open_bracket_stack.pop();

      ops[open_bracket_offset].argument = ops.size();
      ops.push_back(BfOp(BfOpKind::JUMP_IF_DATA_NOT_ZERO, open_bracket_offset));
      pc++;
    } else {
      size_t start = pc++;
      while (pc < program_size && p.instructions[pc] == instruction) {
        pc++;
      }

      size_t num_repeats = pc - start;

      BfOpKind kind = BfOpKind::INVALID_OP;
      switch (instruction) {
      case '>':
        kind = BfOpKind::INC_PTR;
        break;
      case '<':
        kind = BfOpKind::DEC_PTR;
        break;
      case '+':
        kind = BfOpKind::INC_DATA;
        break;
      case '-':
        kind = BfOpKind::DEC_DATA;
        break;
      case '.':
        kind = BfOpKind::WRITE_STDOUT;
        break;
      case ',':
        kind = BfOpKind::READ_STDIN;
        break;
      default:
        DIE << "bad char at " << instruction << " at pc " << start;
      }

      ops.push_back(BfOp(kind, num_repeats));
    }
  }

  return ops;
}

void interpretor(const Program &p, bool verbose) {
  std::vector<uint8_t> memory(MEMORY_SIZE, 0);
  size_t pc = 0;
  size_t dataptr = 0;

#ifndef BFTRACE
  std::unordered_map<int, size_t> op_exec_count;
  std::string current_trace;
  std::unordered_map<std::string, size_t> trace_count;
#endif

  Timer t1;
  std::vector<BfOp> ops = translate_program(p);

  std::cout << "Time to translate " << t1.elapsed() << "\n";
  if (verbose) {
    for (size_t i = 0; i < ops.size(); ++i) {
      std::cout << "[" << i << "]" << BfOpKind_name(ops[i].kind) << " "
                << ops[i].argument << "\n";
    }
  }

  size_t opt_size = ops.size();

  while (pc < opt_size) {
    BfOp op = ops[pc];
    BfOpKind kind = op.kind;

#ifdef BFTRACE
    op_exec_count[static_cast<int>(kind)++];
#endif
    switch (kind) {
    case BfOpKind::INC_PTR:
      dataptr += op.argument;
      break;
    case BfOpKind::DEC_PTR:
      dataptr -= op.argument;
      break;
    case BfOpKind::INC_DATA:
      memory[dataptr] += op.argument;
      break;
    case BfOpKind::DEC_DATA:
      memory[dataptr] -= op.argument;
      break;
    case BfOpKind::WRITE_STDOUT:
      for (size_t i = 0; i < op.argument; ++i) {
        std::cout.put(memory[dataptr]);
      }
      break;
    case BfOpKind::READ_STDIN:
      for (size_t i = 0; i < op.argument; ++i) {
        memory[dataptr] = std::cin.get();
      }
      break;
    case BfOpKind::JUMP_IF_DATA_ZERO:
      if (memory[dataptr] == 0) {
        pc = op.argument;
      }
      break;
    case BfOpKind::JUMP_IF_DATA_NOT_ZERO:
      if (memory[dataptr] != 0) {
        pc = op.argument;
      }
      break;
    default:
      DIE << "unknown character at " << pc;
      break;
    }

#ifdef BFTRACE
    if (kind == BfOpKind::JUMP_IF_DATA_ZERO) {
      current_trace = "";
    } else if (kind == BfOpKind::JUMP_IF_DATA_NOT_ZERO) {
      if (current_trace.size() > 0) {
        trace_count[current_trace] += 1;
        current_trace = "";
      } else {
        op.serialize(&current_trace);
      }
    }
#endif

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

#ifdef BFTRACE
    std::cout << "\n";
    std::cout << "* Tracing \n";
    std::cout.imbue(std::locale(""));
    size_t total = 0;
    for (auto i : op_exec_count) {
      std::cout << BfOpKind_name(static_cast<BfOpKind>(i.first)) << "  -->  "
                << i.second << "\n";
      total += i.second;
    }
    std::cout << ".. Total: " << total << "\n\n";

    using TracePair = std::pair<std::string, size_t>;
    std::vector<TracePair> tracevec;
    std::copy(trace_count.begin(), trace_count.end(),
              std::back_inserter<std::vector<TracePair>>(tracevec));
    std::sort(tracevec.begin(), tracevec.end(),
              [](const TracePair &a, const TracePair &b) {
                return a.second > b.second;
              });

    for (auto const &t : tracevec) {
      std::cout << std::setw(15) << std::left << t.first << " --> " << t.second
                << "\n";
    }
#endif
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
