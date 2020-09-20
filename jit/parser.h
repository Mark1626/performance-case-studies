#ifndef PARSER_H
#define PARSER_H

#include <iostream>

struct Program {
  std::string instructions;
};

Program parse_from_stream(std::istream &stream);

#endif