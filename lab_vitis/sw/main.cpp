#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdio.h>

#include "sleep.h"
#include "test_data.h"
#include "training_data.h"
// #include "xdigitrec_global.h"
#include "xdigitrec_local.h"
#include "xiltimer.h"
// #include "xmm2s.h"
#include "xparameters.h"
#include "xtmrctr.h"

XTmrCtr timer;
XTmrCtr_Config *timer_config = NULL;

XDigitrec_local digitrec_local;
XDigitrec_local_Config *digitrec_local_config = NULL;

// XDigitrec_global digitrec_global;
// XDigitrec_global_Config *digitrec_global_config = NULL;

// XMm2s mm2s;
// XMm2s_Config *mm2s_config = NULL;

uint64_t getTimerCount() {
  uint32_t high1 = XTmrCtr_GetValue(&timer, 1);
  uint32_t low = XTmrCtr_GetValue(&timer, 0);
  uint32_t high2 = XTmrCtr_GetValue(&timer, 1);

  if (high1 != high2) {
    low = XTmrCtr_GetValue(&timer, 0);
  }

  return ((uint64_t)high2 << 32) | low;
}

double timerTicksToSeconds(uint64_t ticks) {
  assert(timer_config != NULL);
  return (double)ticks / timer_config->SysClockFreqHz;
}

int main() {

  const bool logging = false;

  std::cout << "Hello World\n";

  timer_config = XTmrCtr_LookupConfig(XPAR_AXI_TIMER_0_BASEADDR);
  XTmrCtr_CfgInitialize(&timer, timer_config, XPAR_AXI_TIMER_0_BASEADDR);
  XTmrCtr_Start(&timer, 0);

  digitrec_local_config =
      XDigitrec_local_LookupConfig(XPAR_DIGITREC_LOCAL_0_BASEADDR);
  XDigitrec_local_CfgInitialize(&digitrec_local, digitrec_local_config);

  // digitrec_global_config =
  //     XDigitrec_global_LookupConfig(XPAR_DIGITREC_GLOBAL_0_BASEADDR);
  // XDigitrec_global_CfgInitialize(&digitrec_global, digitrec_global_config);

  // mm2s_config = XMm2s_LookupConfig(XPAR_XMM2S_0_BASEADDR);
  // XMm2s_CfgInitialize(&mm2s, mm2s_config);

  uint64_t startTicks, endTicks;
  double elapsed;

  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////// Local /////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  int errors = 0;
  int cnt = 0;

  startTicks = getTimerCount();

  std::cout << "*************************" << std::endl;
  std::cout << "Starting Local Test" << std::endl;
  std::cout << "*************************" << std::endl;
  for (auto data : test_data) {
    uint64_t input = data.first;
    uint8_t golden = data.second;
    uint32_t result;

    // Set the input value and start the accelerator,
    // then wait for it to finish
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

  endTicks = getTimerCount();

  std::cout << "Overall Accuracy = " << std::setprecision(3)
            << ((double)(cnt - errors) / cnt) * 100 << "%" << std::endl;

  elapsed = timerTicksToSeconds(endTicks - startTicks);
  std::cout << "Test size: " << test_data.size() << "\n";
  std::cout << "Runtime: " << elapsed << "(" << elapsed / test_data.size()
            << " per test)\n";
  std::cout << "Done\n";
}
