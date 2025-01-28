#include "AdderTreeBalancer.h"

using namespace llvm;

PreservedAnalyses AdderTreeBalancer::run(Function &F,
                                         FunctionAnalysisManager &AM) {
  // ADD CODE HERE
  // Add private helper functions if needed

  // This is some demo code that will print each basic block in the function
  // You can remove it
  for (auto &bb : F) {
    outs() << "Hi! I'm BasicBlock " << bb.getName() << " in function "
           << F.getName() << "\n";
  }

  // Return 'none' to indicate that no analyses are preserved.
  // This is overly conservative, but indicates to LLVM that all analyses
  // are potentially invalidated by running this pass.
  return PreservedAnalyses::none();
}

// This is the function that will be called by LLVM to register the pass.
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ATB_625", "v0.1", [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "ATB_625") {
                    FPM.addPass(AdderTreeBalancer());
                    return true;
                  }
                  return false;
                });
          }};
}