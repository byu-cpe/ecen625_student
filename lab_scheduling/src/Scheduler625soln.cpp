#include "Scheduler625.h"

#include <cassert>
#include <llvm/IR/BasicBlock.h>
#include <lpsolve/lp_lib.h>

#include "FunctionHLS.h"
#include "FunctionalUnit.h"
#include "SchedHelper.h"

using namespace llvm;

void Scheduler625::scheduleASAP(BasicBlock &bb) {
  // ADD CODE HERE
}
void Scheduler625::scheduleILP(BasicBlock &bb) {
  lprec *lp;
  int i;
  int numCols;

  std::map<Instruction *, int> colNumMap;
  int colNumNOP;

  // Get variable indices for each instruction
  i = 1;
  for (auto &I : bb) {
    if (!SchedHelper::needsScheduling(I))
      continue;
    colNumMap[&I] = i;
    // outs() << i << ":" << I << "\n";
    i++;
  }
  // Create the NOP terminator
  colNumNOP = i;
  numCols = i;

  // Create an lp
  assert(lp = make_lp(0, numCols));

  double *coef;
  int *indices;

  // Constrain all variables to be integers
  for (i = 1; i <= numCols; i++) {
    assert(set_int(lp, i, TRUE));
  }
  //////////////// Constrain all variables to finish <= NOP ///////////
  // ADD CODE HERE

  //////////////// Add data/memory dependency constraints /////////////
  // ADD CODE HERE

  //////////////// Add resource constraints ///////////////////////////
  // ADD CODE HERE

  //////////////// Add timing constraints /////////////////////////////
  // ADD CODE HERE

  //////////////// Set objective function /////////////////////////////
  // ADD CODE HERE
  // use set_obj_fnex()
  // The ILP objective will be to minimize
  set_minim(lp);

  // Print LP for debugging if you like
  // write_LP(lp, stdout);

  // Solve LP
  set_verbose(lp, 1);
  int ret = solve(lp);

  if (ret != 0) {
    errs() << "LP solver returned: " << ret << "\n";
    report_fatal_error("LP solver could not find an optimal solution");
  }

  // Get solution
  double *variables = new double[numCols];
  get_variables(lp, variables);
  for (auto &I : bb) {
    if (!SchedHelper::needsScheduling(I))
      continue;

    schedule[&I] = variables[colNumMap.at(&I) - 1];
  }

  // Scheduler terminator in last cycle
  schedule[bb.getTerminator()] = getMaxCycle(bb);

  delete[] variables;
}