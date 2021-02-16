#include <iomanip>
#include <iostream>
#include <stdio.h>

#include "test_data.h"
#include "xparameters.h"

int main() {

  const bool logging = false;
  int errors = 0;
  int cnt = 0;

  std::cout << "Hello World\n";

  // Initialize HLS IP
  // ADD CODE HERE

  // Get start time
  // ADD CODE HERE

  for (auto data : test_data) {
    uint64_t input = data.first;
    uint8_t golden = data.second;
    uint32_t result;

    // Run HLS Accelerator and wait for it to finish
    // Provide 'input' and get 'result'
    // ADD CODE HERE

    cnt++;
    if (logging) {
      std::cout << std::left << "#" << std::dec << std::setw(5) << cnt;
      std::cout << std::left << std::setw(20) << ": \tTestInstance=" << std::hex
                << input;
      std::cout << " \tInterpreted=" << result << " \tExpected=" << (int)golden;
    }

    if (result != golden) {
      errors++;
      if (logging)
        std::cout << " \t[Mismatch!]";
    }
    if (logging)
      std::cout << std::endl;
  }

  // Get end time and calculate time difference
  double t;
  // ADD CODE HERE

  std::cout << "Overall Accuracy = " << std::setprecision(3)
            << ((double)(cnt - errors) / cnt) * 100 << "%" << std::endl;

  std::cout << "Test size: " << test_data.size() << "\n";
  std::cout << "Runtime: " << t << "(" << t / test_data.size()
            << " per test)\n";
  std::cout << "Done\n";
}
