#include "FunctionHLS.h"

#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>

#include "FunctionalUnit.h"
#include "InstructionHLS.h"
#include "SchedHelper.h"

using namespace llvm;

FunctionHLS::FunctionHLS(Function &F) {
  // Simple allocation of functional units
  fabricFU = new FunctionalUnit("fpga_fabric", FUNCTIONAL_UNIT_NUM_UNLIMITED);

  // Loop through all instructions and allocate functional units
  for (auto &bb : F) {
    for (auto &I : bb) {
      allocate(I);
    }
  }

  for (auto &bb : F) {
    for (auto &I : bb) {
      insnHLSmap[&I] = new InstructionHLS(*this, I);
    }
  }
  // Build instruction dependencies
}

FunctionalUnit *FunctionHLS::getFU(Instruction &I) {
  if (insnToFUmap.find(&I) == insnToFUmap.end())
    return NULL;
  else
    return insnToFUmap[&I];
}

FunctionalUnit *FunctionHLS::findOrAllocateRAM(Value &v) {
  // Check if we already have deteremined the RAM for this value
  if (valToRAMmap.find(&v) != valToRAMmap.end())
    return valToRAMmap[&v];

  FunctionalUnit *fu = NULL;

  // If a pointer offset function, return RAM for the base address
  if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&v)) {
    fu = findOrAllocateRAM(*(GEP->getOperand(0)));
  }

  else if (AllocaInst *AI = dyn_cast<AllocaInst>(&v)) {
    fu = new FunctionalUnit(AI->getName(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
  }

  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(&v)) {
    fu = new FunctionalUnit(GV->getName(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
  }

  if (fu) {
    valToRAMmap[&v] = fu;
    return fu;
  }

  errs() << v << "\n";
  report_fatal_error(
      "Unhandled value given to FunctionHLS::findOrAllocateRAM\n");
}

FunctionalUnit *FunctionHLS::allocate(Instruction &I) {
  if (FunctionalUnit *fu = getFU(I))
    return fu;
  if (!SchedHelper::needsScheduling(I)) {
    return NULL;
  }

  // Control flow is handled by state machine, no computation needed
  if (isa<ReturnInst>(I) || isa<BranchInst>(I) || isa<CallInst>(I) ||
      isa<PHINode>(I)) {
    return NULL;
  }

  FunctionalUnit *fu = NULL;

  //   if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I)) {
  //     fu = fabricFU;
  //     // fu = allocate(*(GEP->getOperand(0)));
  //   }

  // Memories
  if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
    fu = findOrAllocateRAM(*(LI->getPointerOperand()));
  }

  if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
    fu = findOrAllocateRAM(*(SI->getPointerOperand()));
  }

  // New functional unit needs to be created
  //   if (AllocaInst *AI = dyn_cast<AllocaInst>(&I)) {
  //     fu = new FunctionalUnit(AI->getName(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
  //     // FUs.push_back(fu);
  //   }

  //   if (GlobalVariable *GV = dyn_cast<GlobalVariable>(&I)) {
  //     fu = new FunctionalUnit(GV->getName(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
  //     // FUs.push_back(fu);
  //   }

  // These use the FPGA fabric
  if (isa<CastInst>(I) || isa<CmpInst>(I) || isa<GetElementPtrInst>(I)) {
    fu = fabricFU;
  }

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
      fu = fabricFU;
    }
  }

  if (!fu) {
    errs() << I << "\n";
    report_fatal_error("Unhandled instruction in allocation phase");
  }

  insnToFUmap[&I] = fu;
  return fu;
}

bool FunctionHLS::hasMemoryDependency(Instruction &I1, Instruction &I2) {
  // All loads/stores after a call instruction are dependent upon it because of
  // possible side affects
  if (isa<CallInst>(I1)) {
    return true;
  }

  if (isa<StoreInst>(I1) && isa<LoadInst>(I2)) {
    // RAW dependency: I2 might load from I1 store
    return getFU(I1) == getFU(I2);

  } else if (isa<StoreInst>(I2)) {
    // WAW or WAR dependency:
    // I1 is a store OR a load and I2 is a store to
    // potentially same address
    return getFU(I1) == getFU(I2);
  }

  return false;
}
