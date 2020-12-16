#include "llvm/IR/Function.h"

namespace llvm {

class FunctionalUnit {

private:
  int latency;
  double delay;
  int numUnits;
};

class FunctionalUnits {
public:
  FunctionalUnits(Function &F);
};

} // namespace llvm