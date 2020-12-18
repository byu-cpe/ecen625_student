#include <llvm/IR/Function.h>

namespace llvm {

class InstructionHLS;
class FunctionalUnit;

class FunctionHLS {
public:
  FunctionHLS(Function &F);
  InstructionHLS *getInsnHLS(Instruction &I) {
    // if (insnHLSmap.find(&I) == insnHLSmap.end()) {
    //   errs() << I << "\n";
    //   report_fatal_error("getInsnHLS failed");
    // }

    return insnHLSmap[&I];
  }

  FunctionalUnit *getFU(Instruction &I);
  // Dependencies
  bool hasMemoryDependency(Instruction &I1, Instruction &I2);

private:
  Function *I;
  std::map<Instruction *, InstructionHLS *> insnHLSmap;

  // Functional Units
  FunctionalUnit *fabricFU;
  //   std::vector<FunctionalUnit *> FUs;
  //   std::map<Value *
  std::map<Value *, FunctionalUnit *> valToRAMmap;
  std::map<Instruction *, FunctionalUnit *> insnToFUmap;
  FunctionalUnit *allocate(Instruction &I);
  FunctionalUnit *findOrAllocateRAM(Value &v);
};

} // namespace llvm