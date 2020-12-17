#include "Scheduler625.h"

#include <cassert>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/MemoryDependenceAnalysis.h>
#include <llvm/Analysis/MemorySSA.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Pass.h>

#include "FunctionalUnit.h"

using namespace llvm;

char Scheduler625::ID = 0;
static RegisterPass<Scheduler625> X("sched625", "HLS Scheduler for ECEN 625",
                                    false /* Only looks at CFG? */,
                                    true /* Analysis pass? */);

void Scheduler625::scheduleASAP(BasicBlock &bb) {}

bool Scheduler625::runOnFunction(Function &F) {
  FunctionalUnits FUs(F);

  // Loop through basic blocks
  for (auto &bb : F.getBasicBlockList()) {
    scheduleASAP(bb);
  }
  return false;
}

void Scheduler625::print(raw_ostream &OS, const Module *M) const {
  outs() << "Hi there\n";
}

void Scheduler625::getAnalysisUsage(AnalysisUsage &Info) const {
  Info.addRequired<MemoryDependenceWrapperPass>();
  // Info.addRequired < AliasAnalysis
}