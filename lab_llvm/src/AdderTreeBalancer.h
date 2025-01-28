#ifndef ADDERTREEBALANCER_H
#define ADDERTREEBALANCER_H

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace llvm {

class AdderTreeBalancer : public PassInfoMixin<AdderTreeBalancer> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm
#endif /* ADDERTREEBALANCER_H */
