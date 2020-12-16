#include "Scheduler625.h"

#include <cassert>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/DependenceAnalysis.h>
#include <llvm/Analysis/MemoryDependenceAnalysis.h>
#include <llvm/Analysis/MemorySSA.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Pass.h>

using namespace llvm;

char Scheduler625::ID = 0;
static RegisterPass<Scheduler625> X("sched625", "HLS Scheduler for ECEN 625",
                                    false /* Only looks at CFG? */,
                                    true /* Analysis pass? */);

Value *findAlloca(Value *v) {
  // assert(isa<LoadInst>(I) || isa<StoreInst>(I));

  if (AllocaInst *AI = dyn_cast<AllocaInst>(v))
    return AI;
  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(v))
    return GV;

  if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(v)) {
    return findAlloca(GEP->getOperand(0));
  }

  if (LoadInst *LI = dyn_cast<LoadInst>(v)) {
    Value *addr = LI->getPointerOperand();
    return findAlloca(addr);
  }

  if (StoreInst *SI = dyn_cast<StoreInst>(v)) {
    Value *addr = SI->getPointerOperand();
    return findAlloca(addr);
  }

  dbgs() << "I:" << *v << "\n";
  // dbgs() << "addr:" << *addr << "\n";
  assert(false);
  return NULL;
}

bool Scheduler625::runOnFunction(Function &F) {
  // std::vector <

  MemoryDependenceResults &memDep =
      getAnalysis<MemoryDependenceWrapperPass>().getMemDep();

  // Print hello world
  printf("Hello world\n");

  // Loop through basic blocks
  for (auto &bb : F.getBasicBlockList()) {
    for (auto &I : bb.getInstList()) {
      if (isa<LoadInst>(I) || isa<StoreInst>(I)) {
        // outs() << I << "\n";
        // MemDepResult r = memDep.getDependency(&I);
        // outs() << "  ";
        // if (r.getInst())
        //   outs() << *(r.getInst());
        // outs() << "\n";
        Value *AI = findAlloca(&I);
      }
      //   outs() << "  " << getFU(I).latency << "\n";
    }
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