#include <map>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

namespace llvm {

class Instruction;
class FunctionHLS;
class FunctionalUnits;

class Scheduler625 : public PassInfoMixin<Scheduler625> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

private:
  std::map<Instruction *, int> schedule;

  std::map<Instruction *, int> globalSchedule;
  std::map<BasicBlock *, int> globalBBStart;

  FunctionalUnits *FUs;
  FunctionHLS *fHLS;

  double clockPeriodConstraint;

  void scheduleASAP(BasicBlock &bb);
  void scheduleILP(BasicBlock &bb);

  void scheduleGlobal(Function &F);
  void reportSchedule(Function &F, raw_fd_ostream &scheduleFile);
  void validateScheduleAndReportTiming(Function &F, raw_fd_ostream &timingFile);
  void validateScheduleAndReportTiming(BasicBlock &bb,
                                       raw_fd_ostream &timingFile);
  void outputScheduleGantt(Function &F, raw_fd_ostream &ganttFile);
  int getMaxCycle(BasicBlock &bb);
};

} // namespace llvm