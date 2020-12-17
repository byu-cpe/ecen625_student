#include <llvm/IR/Function.h>
#include <map>
#include <vector>

#define FUNCTIONAL_UNIT_NUM_MEM_PORTS 2
#define FUNCTIONAL_UNIT_NUM_UNLIMITED 0

namespace llvm {

class FunctionalUnit {
public:
  FunctionalUnit(std::string name, int numUnits)
      : name(name), numUnits(numUnits) {}

private:
  std::string name;
  int numUnits;

  int latency;
  double delay;
};

class FunctionalUnits {
public:
  FunctionalUnits(Function &F);
  FunctionalUnit *getFU(Value *v);

private:
  FunctionalUnit *fabricFU;

  std::vector<FunctionalUnit *> FUs;
  std::map<Value *, FunctionalUnit *> valueToFUmap;

  FunctionalUnit *findOrAllocate(Value *v);
};

} // namespace llvm