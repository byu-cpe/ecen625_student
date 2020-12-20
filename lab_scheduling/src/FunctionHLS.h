#include <llvm/IR/Function.h>
#include <map>
#include <vector>

namespace llvm {

class Instruction;
class InstructionHLS;
class FunctionalUnit;

class FunctionHLS {
public:
  FunctionHLS(Function &F);

  FunctionalUnit *getFU(Instruction &I);
  std::vector<Instruction *> &getDeps(Instruction &I);

private:
  // Dependencies
  std::map<Instruction *, std::vector<Instruction *>> deps;

  // Functional Units
  FunctionalUnit *fabricFU;
  std::map<Value *, FunctionalUnit *> valToRAMmap;
  std::map<Instruction *, FunctionalUnit *> insnToFUmap;

  bool hasMemoryDependency(Instruction &I1, Instruction &I2);
  FunctionalUnit *allocate(Instruction &I);
  FunctionalUnit *findOrAllocateRAM(Value &v);
};

} // namespace llvm