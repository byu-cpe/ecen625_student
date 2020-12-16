#include "FunctionalUnit.h"

using namespace llvm;

FunctionalUnits::FunctionalUnits(Function &F) {}

// FU_t getFU(Instruction &I) {
//   FU_t ret;
//   if (isa<LoadInst>(I)) {
//     ret = {2, 0, 1};
//     return ret;
//   } else if (isa<StoreInst>(I)) {
//     ret = {1, 0, 1};
//     return ret;
//   } else if (isa<ZExtInst>(I) || isa<TruncInst>(I) || isa<ReturnInst>(I)) {
//     ret = {0, 0, 0};
//     return ret;
//   } else if (isa<BinaryOperator>(I)) {
//     ret = {0, 3, 0};
//     return ret;
//   }
//   errs() << I << "\n";
//   assert(false);
// }