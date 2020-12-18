#include "InstructionHLS.h"

#include <llvm/IR/Instruction.h>

#include "FunctionHLS.h"
#include "SchedHelper.h"

using namespace llvm;

InstructionHLS::InstructionHLS(FunctionHLS &F, Instruction &I) : I(I) {
  // Find data dependencies
  if (SchedHelper::needsScheduling(I)) {
    for (auto operand = I.op_begin(), end = I.op_end(); operand != end;
         ++operand) {

      // ignore operands that aren't instructions
      Instruction *Idep = dyn_cast<Instruction>(*operand);
      if (!Idep)
        continue;

      // ignore instructions that aren't scheduled
      if (!SchedHelper::needsScheduling(*Idep))
        continue;

      // ignore operands from other basic blocks
      if (Idep->getParent() != I.getParent())
        continue;

      deps.push_back(Idep);
    }
  }

  // Find memory dependencies (Idep->I)
  for (auto &Idep : *(I.getParent())) {
    // If we reach I, stop
    if (&Idep == &I)
      break;

    // if (!isa<LoadInst>(&I1) && !isa<StoreInst>(&I1) && !isa<CallInst>(&I1))
    //   continue;

    // if (isaDummyCall(I1))
    //   continue;

    if (F.hasMemoryDependency(Idep, I) &&
        (std::find(deps.begin(), deps.end(), &Idep) == deps.end())) {
      deps.push_back(&Idep);
    }
  }
}