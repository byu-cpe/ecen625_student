#ifndef SRC_COMMON_UTILS_H_
#define SRC_COMMON_UTILS_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

typedef uint64_t timestamp_t;

#define MICROSECS_PER_SEC 1000000.0L

timestamp_t get_timestamp();

bool boolStrToBool(std::string boolStr);

inline bool fileExists(const std::string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

std::vector<std::string> glob(const std::string &pat);

std::string exec(const char *cmd);

inline std::string exec(std::string cmd) { return exec(cmd.c_str()); }

#endif /* SRC_COMMON_UTILS_H_ */
