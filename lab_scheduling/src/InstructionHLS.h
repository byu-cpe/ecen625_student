#include <llvm/IR/Instruction.h>

namespace llvm {

class FunctionHLS;

class InstructionHLS {

public:
  InstructionHLS(FunctionHLS &F, Instruction &I);
  std::vector<Instruction *> &getDeps() { return deps; }

private:
  Instruction &I;
  std::vector<Instruction *> deps;
};

} // namespace llvm