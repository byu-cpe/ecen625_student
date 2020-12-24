#include <llvm/IR/Instructions.h>

namespace llvm {

class SchedHelper {
public:
  static int getInsnLatency(Instruction &I);
  static double getInsnDelay(Instruction &I);
  static bool needsScheduling(Instruction &I);

private:
  static double targetClockPeriod;
};

} // namespace llvm