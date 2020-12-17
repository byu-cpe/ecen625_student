#include "SchedHelper.h"

using namespace llvm;

int SchedHelper::getInsnLatency(Instruction &I) {

  errs() << "Error: unhandled instruction in SchedHelper::getInsnLatency:\n"
         << I << "\n";
}