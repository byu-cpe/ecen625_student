#include <llvm/Pass.h>

namespace llvm {

class Scheduler625 : public llvm::FunctionPass {
public:
  static char ID;
  Scheduler625() : FunctionPass(ID) {}

  bool runOnFunction(Function &F);
  void print(raw_ostream &OS, const Module *M) const;
  void getAnalysisUsage(AnalysisUsage &Info) const;

private:
};

} // namespace llvm