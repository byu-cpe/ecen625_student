#include "FunctionalUnit.h"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

using namespace llvm;

FunctionalUnit *FunctionalUnits::getFU(Value *v) {
  if (valueToFUmap.find(v) == valueToFUmap.end())
    return NULL;
  else
    return valueToFUmap[v];
}

FunctionalUnit *FunctionalUnits::findOrAllocate(Value *v) {
  if (FunctionalUnit *fu = getFU(v))
    return fu;

  // Control flow is handled by state machine, no computation needed
  if (isa<ReturnInst>(v) || isa<BranchInst>(v) || isa<CallInst>(v)) {
    return NULL;
  }

  FunctionalUnit *fu = NULL;

  if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(v)) {
    fu = findOrAllocate(GEP->getOperand(0));
  }

  if (LoadInst *LI = dyn_cast<LoadInst>(v)) {
    Value *addr = LI->getPointerOperand();
    fu = findOrAllocate(addr);
  }

  if (StoreInst *SI = dyn_cast<StoreInst>(v)) {
    Value *addr = SI->getPointerOperand();
    fu = findOrAllocate(addr);
  }

  // These use the FPGA fabric
  if (isa<CastInst>(v) || isa<CmpInst>(v)) {
    fu = fabricFU;
  }

  if (BinaryOperator *BO = dyn_cast<BinaryOperator>(v)) {
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
      fu = fabricFU;
    }
  }

  // New functional unit needs to be created
  if (AllocaInst *AI = dyn_cast<AllocaInst>(v)) {
    fu = new FunctionalUnit(AI->getName(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
    FUs.push_back(fu);
  }

  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(v)) {
    fu = new FunctionalUnit(GV->getName(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
    FUs.push_back(fu);
  }

  if (!fu) {
    outs() << "Error: Unhandled instruction in allocation phase:\n"
           << *v << "\n";
    assert(false);
  }

  valueToFUmap[v] = fu;
  return fu;
}

FunctionalUnits::FunctionalUnits(Function &F) {
  // Simple allocation of functional units

  fabricFU = new FunctionalUnit("fpga_fabric", FUNCTIONAL_UNIT_NUM_UNLIMITED);

  // Loop through all instructions and allocate functional units
  for (auto &bb : F) {
    for (auto &I : bb) {
      findOrAllocate(&I);
    }
  }
}
