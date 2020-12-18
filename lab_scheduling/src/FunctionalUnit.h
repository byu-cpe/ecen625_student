#include <llvm/IR/Function.h>
#include <map>
#include <vector>

#define FUNCTIONAL_UNIT_NUM_MEM_PORTS 2
#define FUNCTIONAL_UNIT_NUM_UNLIMITED 0

namespace llvm {

class FunctionalUnit {
public:
  FunctionalUnit(std::string name, int numUnits)
      : name(name), numUnits(numUnits), initiationInterval(1) {}

  int getNumUnits() { return numUnits; }
  std::string getName() { return name; }
  int getInitiationInterval() { return initiationInterval; }

private:
  std::string name;
  int numUnits;
  int initiationInterval;
};

// class FunctionalUnits {
// public:
//   FunctionalUnits(Function &F);

// private:

// };

} // namespace llvm