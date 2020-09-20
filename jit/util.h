#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <string>

namespace internal {
class FatalLogError : public std::basic_ostringstream<char> {
public:
  ~FatalLogError();
};
} // namespace internal

class Timer {
public:
  Timer();
  double elapsed();

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> t1_;
};

#define DIE internal::FatalLogError()

void parse_command_line(int argc, char **argv, std::string *bf_file_path,
                        bool *verbose);

#endif
