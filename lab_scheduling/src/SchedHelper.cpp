#include "SchedHelper.h"

using namespace llvm;

int SchedHelper::getInsnLatency(Instruction &I) {
  if (isa<LoadInst>(&I))
    return 2;
  if (isa<StoreInst>(&I))
    return 1;
  if (isa<CallInst>(&I) || isa<ReturnInst>(&I))
    return 1;
  if (isa<BranchInst>(&I) || isa<PHINode>(&I))
    return 0;
  if (isa<UnaryInstruction>(&I) || isa<CmpInst>(&I) ||
      isa<GetElementPtrInst>(&I) || isa<SelectInst>(&I))
    return 0;
  if (BinaryOperator *BO = dyn_cast<BinaryOperator>(&I)) {
    switch (BO->getOpcode()) {
    case Instruction::Add:
    case Instruction::Sub:
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

double SchedHelper::getInsnDelay(Instruction &I) {
  if (isa<LoadInst>(&I) || isa<StoreInst>(&I))
    return 5.0;
  else if (I.isCast() || I.isTerminator() || isa<CallInst>(&I) ||
           isa<GetElementPtrInst>(&I))
    return 0;
  else if (BinaryOperator *BO = dyn_cast<BinaryOperator>(&I)) {
    switch (BO->getOpcode()) {
    case Instruction::Add:
    case Instruction::Sub:
    case Instruction::And:
    case Instruction::Or:
    case Instruction::Xor:
      return 3.5;
    case Instruction::Shl:
    case Instruction::LShr:
    case Instruction::AShr:
      return 4.0;
    }
  } else if (isa<CmpInst>(&I)) {
    return 3.0;
  } else if (isa<SelectInst>(&I)) {
    return 3.8;
  }

  errs() << I << "\n";
  report_fatal_error("unhandled instruction in SchedHelper::getInsnDelay");
}

bool SchedHelper::needsScheduling(Instruction &I) {
  if (isa<AllocaInst>(&I) || isa<PHINode>(&I))
    return false;
  if (CallInst *CI = dyn_cast<CallInst>(&I)) {
    if (CI->getCalledFunction()->getName().find("llvm.") == 0) {
      return false;
    }
  }
  return true;
}