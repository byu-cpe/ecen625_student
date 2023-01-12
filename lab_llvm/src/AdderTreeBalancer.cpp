#include "AdderTreeBalancer.h"

#include <llvm/Transforms/Utils/BasicBlockUtils.h>

using namespace llvm;

char AdderTreeBalancer::ID = 0;

static RegisterPass<AdderTreeBalancer> X("ATB_625", "Adder Tree Balancer Pass");

bool AdderTreeBalancer::runOnFunction(Function &f) {
  // ADD CODE HERE
  // Add private helper functions if needed

  for (auto &bb : f) {
    outs() << "Hi! I'm BasicBlock " << bb.getName() << " in function "
           << f.getName() << "\n";
  }

  // return whether code has been modified
  return true;
}
