#include <llvm/IR/Instructions.h>

namespace llvm {

class SchedHelper {
public:
  static int getInsnLatency(Instruction &I);
  static bool needsScheduling(Instruction &I);
};

} // namespace llvm