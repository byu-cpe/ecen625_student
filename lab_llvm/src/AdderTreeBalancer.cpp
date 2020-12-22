#include "AdderTreeBalancer.h"

#include <llvm/Transforms/Utils/BasicBlockUtils.h>

using namespace llvm;

char AdderTreeBalancer::ID = 0;

static RegisterPass<AdderTreeBalancer> X("ATB_625", "Adder Tree Balancer Pass");

bool AdderTreeBalancer::runOnBasicBlock(BasicBlock &BB) {
  // ADD CODE HERE
  // Add private helper functions if needed

  // return whether code has been modified
  return true;
}
