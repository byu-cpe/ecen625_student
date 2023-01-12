#ifndef ADDERTREEBALANCER_H
#define ADDERTREEBALANCER_H

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/Pass.h>

#include <deque>

namespace llvm {

class AdderTreeBalancer : public FunctionPass {
public:
  static char ID;

  AdderTreeBalancer() : FunctionPass(ID) {}
  ~AdderTreeBalancer() {}

private:
public:
  bool runOnFunction(Function &F) override;
};
} // namespace llvm
#endif /* ADDERTREEBALANCER_H */
