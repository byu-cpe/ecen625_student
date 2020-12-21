#include "AdderTreeBalancer.h"

#include <llvm/Transforms/Utils/BasicBlockUtils.h>

using namespace llvm;

namespace legup {

char AdderTreeBalancer::ID = 0;

static RegisterPass<AdderTreeBalancer> X("ATB_625", "Adder Tree Balancer Pass");

AdderTreeBalancer::AdderTreeBalancer() : BasicBlockPass(ID) {
  // TODO Auto-generated constructor stub
}

AdderTreeBalancer::~AdderTreeBalancer() {
  // TODO Auto-generated destructor stub
}

bool AdderTreeBalancer::runOnBasicBlock(BasicBlock &BB) {
  // ADD CODE HERE
  // Add private helper functions if needed

  // return whether code has been modified
  return false;
}
} // namespace legup
