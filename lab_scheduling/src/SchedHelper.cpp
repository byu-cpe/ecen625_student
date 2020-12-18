#include "SchedHelper.h"

using namespace llvm;

int SchedHelper::getInsnLatency(Instruction &I) {
  if (isa<LoadInst>(&I))
    return 2;
  if (isa<StoreInst>(&I))
    return 1;
  if (isa<UnaryInstruction>(&I) || isa<CmpInst>(&I) ||
      isa<GetElementPtrInst>(&I))
    return 0;
  if (isa<CallInst>(&I) || isa<ReturnInst>(&I))
    return 1;
  if (isa<BranchInst>(&I) || isa<PHINode>(&I))
    return 0;
  if (BinaryOperator *BO = dyn_cast<BinaryOperator>(&I)) {
    switch (BO->getOpcode()) {
    case Instruction::Add:
    case Instruction::Sub:
    case Instruction::Mul:
    case Instruction::And:
    case Instruction::Or:
    case Instruction::Xor:
    case Instruction::Shl:
    case Instruction::LShr:
    case Instruction::AShr:
      return 0;
    }
  }
  errs() << I << "\n";
  report_fatal_error("unhandled instruction in SchedHelper::getInsnLatency");
}

bool SchedHelper::needsScheduling(Instruction &I) {
  if (isa<AllocaInst>(&I) || isa<PHINode>(&I))
    return false;
  return true;
}