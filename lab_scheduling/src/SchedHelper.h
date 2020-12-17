#include <llvm/IR/Instructions.h>

namespace llvm {

class SchedHelper {
public:
  static int getInsnLatency(Instruction &I);
};

} // namespace llvm