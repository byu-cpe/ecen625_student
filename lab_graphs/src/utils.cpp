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

timestamp_t get_timestamp() {
  struct timeval now = {0};
  gettimeofday(&now, NULL);

  const int usPerS = 1000000;
  return now.tv_usec + (timestamp_t)now.tv_sec * usPerS;
}

// From
// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
std::string exec(const char *cmd) {
  const int bufSize = 128;
  std::array<char, bufSize> buffer = {0};
  std::string result;
  FILE *pipe = popen(cmd, "r");
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
  }
  return result;
}