/*
 * utils.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: jgoeders
 */

#include <array>
#include <cassert>
#include <glob.h>
#include <memory>
#include <sys/time.h>
#include <vector>

#include "utils.h"

bool boolStrToBool(std::string boolStr) {
  if (boolStr == "False" || boolStr == "false") {
    return false;
  } else if (boolStr == "True" || boolStr == "true") {
    return true;
  } else {
    assert(false);
  }
}

// From
// http://stackoverflow.com/questions/8401777/simple-glob-in-c-on-unix-system
std::vector<std::string> glob(const std::string &pat) {
  glob_t glob_result;
  glob(pat.c_str(), GLOB_TILDE, NULL, &glob_result);
  std::vector<std::string> ret;
  for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
    ret.push_back(std::string(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
  return ret;
}

timestamp_t get_timestamp() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

// From
// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
std::string exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}