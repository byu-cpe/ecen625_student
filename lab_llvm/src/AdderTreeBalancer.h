#ifndef LAB_LLVM_SRC_ADDERTREEBALANCER
#define LAB_LLVM_SRC_ADDERTREEBALANCER

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/Pass.h>

#include <deque>

namespace llvm {

class AdderTreeBalancer : public BasicBlockPass {
public:
  static char ID;

  AdderTreeBalancer() : BasicBlockPass(ID) {}
  ~AdderTreeBalancer() {}

private:
public:
  bool runOnBasicBlock(BasicBlock &BB);
};
} // namespace llvm
#endif /* LAB_LLVM_SRC_ADDERTREEBALANCER */
