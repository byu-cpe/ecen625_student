#include "FunctionHLS.h"

#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>

#include "FunctionalUnit.h"
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

      // Build instruction dependencies
      // Find data dependencies
      if (!SchedHelper::needsScheduling(I))
        continue;

      auto &Ideps = deps[&I];

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

        Ideps.push_back(Idep);
      }

      // Find memory dependencies (Idep->I)
      for (auto &Idep : *(I.getParent())) {
        // If we reach I, stop
        if (&Idep == &I)
          break;

        // ignore instructions that aren't scheduled
        if (!SchedHelper::needsScheduling(Idep))
          continue;

        if (hasMemoryDependency(Idep, I) &&
            (std::find(Ideps.begin(), Ideps.end(), &Idep) == Ideps.end())) {
          Ideps.push_back(&Idep);
        }
      }
    }
  }
}

FunctionalUnit *FunctionHLS::getFU(Instruction &I) {
  // if (!SchedHelper::needsScheduling(I))
  //   report_fatal_error(
  //       "FunctionHLS::getFU called on non-schedulable instruction\n");
  if (insnToFUmap.find(&I) == insnToFUmap.end())
    return NULL;
  else
    return insnToFUmap[&I];
}

std::vector<Instruction *> &FunctionHLS::getDeps(Instruction &I) {
  if (!SchedHelper::needsScheduling(I))
    report_fatal_error(
        "FunctionHLS::getDeps called on non-schedulable instruction\n");
  return deps.at(&I);
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
    fu = new FunctionalUnit(AI->getName().str(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
  }

  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(&v)) {
    fu = new FunctionalUnit(GV->getName().str(), FUNCTIONAL_UNIT_NUM_MEM_PORTS);
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

  // Memories
  if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
    fu = findOrAllocateRAM(*(LI->getPointerOperand()));
  }

  if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
    fu = findOrAllocateRAM(*(SI->getPointerOperand()));
  }

  // These use the FPGA fabric
  if (isa<CastInst>(I) || isa<CmpInst>(I) || isa<GetElementPtrInst>(I) ||
      isa<SelectInst>(I)) {
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
  // All loads/stores after a call instruction are dependent upon it because
  // of possible side affects
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
