#include "Scheduler625.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Pass.h>
#include <regex>

#include "FunctionHLS.h"
#include "FunctionalUnit.h"
#include "InstructionHLS.h"
#include "SchedHelper.h"

using namespace llvm;

cl::opt<bool> ILPFlag("ILP", cl::desc("Use ILP scheduler instead of ASAP."));

char Scheduler625::ID = 0;
static RegisterPass<Scheduler625> X("sched625", "HLS Scheduler for ECEN 625",
                                    false /* Only looks at CFG? */,
                                    true /* Analysis pass? */);

bool Scheduler625::runOnFunction(Function &F) {
  // FUs = new FunctionalUnits(F);
  fHLS = new FunctionHLS(F);

  // Loop through basic blocks
  for (auto &bb : F.getBasicBlockList()) {
    if (ILPFlag)
      scheduleILP(bb);
    else
      scheduleASAP(bb);
  }

  validateSchedule(F);
  scheduleGlobal(F);
  outputScheduleGantt(F);

  return false;
}

void Scheduler625::validateSchedule(Function &F) {
  for (auto &bb : F) {
    validateSchedule(bb);
  }
}

void Scheduler625::validateSchedule(BasicBlock &bb) {
  outs() << "Validating Schedule for BB \"" << bb.getName().str()
         << "\" of function \"" << bb.getParent()->getName().str() << "\"...\n";

  // Check that all instructions have been assigned to a state
  outs() << "  Checking that all instructions have been assigned to a state\n";
  for (auto &I : bb) {
    // InstructionNode *in = dag->getInstructionNode(&I);
    // bool mapped = map->getInsnIsMapped(in);
    if (schedule.find(&I) == schedule.end()) {
      errs() << "Instruction is not mapped to a state:\n" << I << "\n";
      report_fatal_error("Invalid schedule");
    }
  }

  outs() << "  Checking data dependencies\n";
  // Check data dependencies
  for (auto &I : bb) {
    InstructionHLS *Ihls = fHLS->getInsnHLS(I);

    unsigned stateNum = schedule[&I];

    for (auto Idep : Ihls->getDeps()) {
      unsigned stateNumDep = schedule[Idep];

      if (stateNum < (stateNumDep + SchedHelper::getInsnLatency(*Idep))) {
        errs() << "Data dependence violation.  "
                  "Instruction I=\""
               << I << "\" must be scheduled at least "
               << SchedHelper::getInsnLatency(*Idep)
               << " cycles after dependent instruction Idep=\"" << *Idep
               << "\".  Currently they are scheduled to states "
                  "(Idep, I) = ("
               << stateNumDep << ", " << stateNum << ").\n";
        report_fatal_error("Invalid schedule");
      }
    }
  }

  // Check Terminator
  int terminatorCycle = schedule[bb.getTerminator()];
  int lastCycle = getMaxCycle(bb);
  if (terminatorCycle < lastCycle) {
    errs() << "Terminating instruction \"" << *(bb.getTerminator())
           << "\" is scheduled at cycle " << terminatorCycle
           << ", which is earlier than the last cycle, " << lastCycle << "\n";
    report_fatal_error("Invalid schedule");
  }

  // Check usage of functional units
  outs() << "  Checking number of functional units used\n";
  // Build fuUsage, which is a mapping of functional unit to second map, which
  // maps the stateNum (int) it is used in, to the list of Instructions that use
  // the FU in that stateNum.
  std::map<FunctionalUnit *, std::map<unsigned, std::vector<Instruction *>>>
      fuUsage;

  for (auto &I : bb) {
    FunctionalUnit *FU = fHLS->getFU(I);
    if (!FU)
      continue;
    unsigned stateNum = schedule[&I];
    fuUsage[FU][stateNum].push_back(&I);
  }

  // Now go through each FU and check for violations:
  // 1. That # of FUs used of each type do not exceed number allocated
  // 2. That the initiation interval of each FU is not violated
  // Since every FU has initiation interval of 1, we actually
  // don't check for #2
  for (auto &fuMap : fuUsage) {
    FunctionalUnit *FU = fuMap.first;
    int numAllocated = FU->getNumUnits();
    if (numAllocated == FUNCTIONAL_UNIT_NUM_UNLIMITED)
      continue;

    for (auto &fuUsageMap : fuMap.second) {
      unsigned stateNum = fuUsageMap.first;
      auto fuUsageList = fuUsageMap.second;

      if (fuUsageList.size() > (unsigned)numAllocated) {
        errs() << "Functional unit violation. FU \"" << FU->getName()
               << "\", which has " << numAllocated << " instances, is used "
               << fuUsageList.size() << " times in state number " << stateNum
               << ".  The instructions which use the FU are:\n";
        for (auto fuUsageInst : fuUsageList) {
          errs() << "  " << *fuUsageInst << "\n";
        }
        report_fatal_error("Invalid schedule");
      }

      // Double check that initiation intervals are 1
      for (auto fuUsageInst : fuUsageList) {
        assert(FU->getInitiationInterval() == 1);
      }
    }
  }
}

void Scheduler625::outputScheduleGantt(Function &F) {

  std::string funcName = F.getName();
  // StringRef fileName =
  StringRef fileName = funcName + ".tex";

  std::error_code EC;
  raw_fd_ostream myfile(fileName, EC);

  // myfile.open();
  myfile << "\\documentclass[12pt]{article}\n";
  myfile << "\\usepackage{lscape}\n";
  myfile << "\\usepackage[margin=0.5in,paperwidth=10in,paperheight=30in]{"
            "geometry}\n";
  myfile << "\\usepackage{pgfgantt}\n";
  myfile << "\\begin{document}\n";
  // myfile << "\\begin{landscape}\n";
  myfile << "\\section*{Schedule of "
         << std::regex_replace(funcName, std::regex("_"), "\\_") << "}\n";

  // Find max cycleNum
  int cycleNumEnd = 0;
  for (auto &bb : F) {
    cycleNumEnd = std::max(cycleNumEnd, globalBBStart[&bb] + getMaxCycle(bb));
  }
  myfile << "\\begin{ganttchart}["
         << "bar top shift = 0.1, bar height = 0.8"
         << "]{0}{" << cycleNumEnd << "}\n";
  myfile << "\\gantttitlelist{1,...," << cycleNumEnd + 1 << "}{1} \\\\ \n";

  int elem = 0;
  std::map<Value *, int> elemMap;
  for (auto &bb : F) {
    elemMap[&bb] = elem;
    elem++;

    myfile << "\\ganttgroup{" << bb.getName().str() << "} {"
           << globalBBStart[&bb] << "} {"
           << globalBBStart[&bb] + getMaxCycle(bb) << "} \\\\ \n";

    // Print each instruction in the basic block
    for (auto &I : bb) {
      if (!SchedHelper::needsScheduling(I))
        continue;
      std::string str;
      llvm::raw_string_ostream ss(str);
      ss << I;
      str = std::regex_replace(str, std::regex("%"), "\\%");
      str = std::regex_replace(str, std::regex("_"), "\\_");

      const int Ilength = 50;
      if (str.length() > Ilength)
        str = str.substr(0, Ilength - 3) + "...";

      myfile << "\\ganttbar{" << str << "}{" << globalSchedule[&I] << "}{"
             << globalSchedule[&I] +
                    std::max(SchedHelper::getInsnLatency(I) - 1, 0)
             << "} \\\\ \n";

      // Add links
      elemMap[&I] = elem;
      for (auto dep : fHLS->getInsnHLS(I)->getDeps()) {
        myfile << "\\ganttlink{elem" << elemMap[dep] << "}{elem" << elem
               << "}\n";
      }
      elem++;
    }
  }

  // Add basic block links
  for (auto &bb : F) {
    for (auto successor : successors(&bb)) {
      // myfile << "\\ganttlink{elem" << elemMap[&bb] << "}{elem"
      //        << elemMap[successor] << "}\n";
    }
  }

  myfile << "\\end{ganttchart}\n";
  // myfile << "\\end{landscape}\n";
  myfile << "\\end{document}\n";

  myfile.close();
}

void Scheduler625::scheduleGlobal(Function &F) {
  int cycleNum = 0;
  for (auto &bb : F) {
    globalBBStart[&bb] = cycleNum;
    for (auto &I : bb) {
      globalSchedule[&I] = cycleNum + schedule[&I];
    }
    cycleNum += getMaxCycle(bb) + 1;
  }
}

int Scheduler625::getMaxCycle(BasicBlock &bb) {
  int maxCycle = 0;
  for (auto &I : bb) {
    maxCycle =
        std::max(maxCycle, schedule[&I] +
                               std::max(SchedHelper::getInsnLatency(I) - 1, 0));
  }
  return maxCycle;
}