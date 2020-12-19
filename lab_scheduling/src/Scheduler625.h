#include <llvm/Pass.h>
#include <map>

namespace llvm {

class Instruction;
class FunctionHLS;
class FunctionalUnits;

class Scheduler625 : public llvm::FunctionPass {
public:
  static char ID;
  Scheduler625() : FunctionPass(ID) {}

  bool runOnFunction(Function &F);

private:
  std::map<Instruction *, int> schedule;
  // std::map<BasicBlock *, int> maxCycleNum;

  std::map<Instruction *, int> globalSchedule;
  std::map<BasicBlock *, int> globalBBStart;

  FunctionalUnits *FUs;
  FunctionHLS *fHLS;

  double clockPeriodConstraint;

  void scheduleASAP(BasicBlock &bb);
  void scheduleILP(BasicBlock &bb);

  void scheduleGlobal(Function &F);
  void printSchedule(Function &F);
  void validateSchedule(Function &F);
  void validateSchedule(BasicBlock &bb);
  void outputScheduleGantt(Function &F);
  int getMaxCycle(BasicBlock &bb);
};

} // namespace llvm