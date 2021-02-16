//=========================================================================
// testbench.cpp
//=========================================================================
// @brief: testbench for k-nearest-neighbor digit recognition application

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "digitrec.h"
#include "test_data.h"

int main() {
  // Output file that saves the test bench results
  std::ofstream outfile;
  outfile.open("out.dat");

  int num_test_insts = 0;
  int error = 0;

  for (auto data : test_data) {
    digit input_digit = data.first;
    uint8_t golden = data.second;

    // Call design under test (DUT)
    bit4 interpreted_digit = digitrec(input_digit);

    // Print result messages to console
    num_test_insts++;
    std::cout << "#" << std::dec << num_test_insts;
    std::cout << ": \tTestInstance=" << std::hex << input_digit;
    std::cout << " \tInterpreted=" << std::dec << interpreted_digit
              << " \tExpected=" << unsigned(golden);
    // Print result messages to file
    outfile << "#" << std::dec << num_test_insts;
    outfile << ": \tTestInstance=" << std::hex << input_digit;
    outfile << " \tInterpreted=" << std::dec << interpreted_digit
            << " \tExpected=" << unsigned(golden);

    // Check for any difference between k-NN interpreted digit vs. expected
    // digit
    if (interpreted_digit != golden) {
      error++;
      std::cout << " \t[Mismatch!]";
      outfile << " \t[Mismatch!]";
    }

    std::cout << std::endl;
    outfile << std::endl;
  }

  // Report overall error out of all testing instances
  std::cout << "Overall Accuracy = " << std::setprecision(3)
            << ((double)(num_test_insts - error) / num_test_insts) * 100 << "%"
            << std::endl;
  outfile << "Overall Accuracy = " << std::setprecision(3)
          << ((double)(num_test_insts - error) / num_test_insts) * 100 << "%"
          << std::endl;

  // Close output file
  outfile.close();

  return 0;
}
