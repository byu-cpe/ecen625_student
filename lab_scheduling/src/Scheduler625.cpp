#include "Scheduler625.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Pass.h>
#include <regex>

#include "FunctionHLS.h"
#include "FunctionalUnit.h"
#include "SchedHelper.h"

using namespace llvm;

cl::opt<bool> ILPFlag("ILP", cl::desc("Use ILP scheduler instead of ASAP."));
cl::opt<double> PeriodOpt("period");

char Scheduler625::ID = 0;
static RegisterPass<Scheduler625> X("sched625", "HLS Scheduler for ECEN 625",
                                    false /* Only looks at CFG? */,
                                    true /* Analysis pass? */);

bool Scheduler625::runOnFunction(Function &F) {
  // FUs = new FunctionalUnits(F);
  fHLS = new FunctionHLS(F);

  clockPeriodConstraint = PeriodOpt;
  SchedHelper::setTargetClockPeriod(clockPeriodConstraint);

  // Loop through basic blocks
  for (auto &bb : F.getBasicBlockList()) {
    if (ILPFlag)
      scheduleILP(bb);
    else
      scheduleASAP(bb);
  }

  printSchedule(F);
  scheduleGlobal(F);
  validateSchedule(F);
  outputScheduleGantt(F);

  return false;
}

void Scheduler625::printSchedule(Function &F) {
  outs() << "\nSchedule of function " << F.getName() << "\n";
  for (auto &bb : F) {
    outs() << "  Basic Block " << bb.getName() << "\n";
    for (int i = 0; i <= getMaxCycle(bb); i++) {
      outs() << "   Cycle " << i << "\n";
      for (auto it : schedule) {
        if ((it.second == i) && (it.first->getParent() == &bb))
          outs() << "      " << *(it.first) << "\n";
      }
    }
  }
}

void Scheduler625::validateSchedule(Function &F) {
  outs() << "\nValidating Schedule for Function " << F.getName() << "\n";
  for (auto &bb : F) {
    validateSchedule(bb);
  }
}

void Scheduler625::validateSchedule(BasicBlock &bb) {
  outs() << "  BB \"" << bb.getName().str() << "\" of function \""
         << bb.getParent()->getName().str() << "\"...\n";

  // Check that all instructions have been assigned to a state
  outs()
      << "    Checking that all instructions have been assigned to a state\n";
  for (auto &I : bb) {
    bool needsScheduling = SchedHelper::needsScheduling(I);
    bool scheduled = schedule.find(&I) != schedule.end();
    if (needsScheduling && !scheduled) {
      errs() << "Instruction is not mapped to a state:\n" << I << "\n";
      report_fatal_error("Invalid schedule");
    } else if (!needsScheduling && scheduled) {
      errs() << "Instruction should not be scheduled:\n" << I << "\n";
      report_fatal_error("Invalid schedule");
    }
  }

  outs() << "    Checking data dependencies\n";
  // Check data dependencies
  for (auto &I : bb) {
    if (!SchedHelper::needsScheduling(I))
      continue;

    unsigned stateNum = schedule.at(&I);

    for (auto Idep : fHLS->getDeps(I)) {
      unsigned stateNumDep = schedule.at(Idep);

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
  int terminatorCycle = schedule.at(bb.getTerminator());
  int lastCycle = getMaxCycle(bb);
  if (terminatorCycle < lastCycle) {
    errs() << "Terminating instruction \"" << *(bb.getTerminator())
           << "\" is scheduled at cycle " << terminatorCycle
           << ", which is earlier than the last cycle, " << lastCycle << "\n";
    report_fatal_error("Invalid schedule");
  }

  // Check usage of functional units
  outs() << "    Checking number of functional units used\n";
  // Build fuUsage, which is a mapping of functional unit to second map, which
  // maps the stateNum (int) it is used in, to the list of Instructions that use
  // the FU in that stateNum.
  std::map<FunctionalUnit *, std::map<unsigned, std::vector<Instruction *>>>
      fuUsage;

  for (auto &I : bb) {
    FunctionalUnit *FU = fHLS->getFU(I);
    if (!FU)
      continue;
    unsigned stateNum = schedule.at(&I);
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

  if (clockPeriodConstraint == 0)
    return;

  // Check critical paths
  outs() << "    Checking critical path\n";
  std::map<unsigned, std::vector<Instruction *>> insnsByStateNum;
  for (auto &I : bb) {
    if (!SchedHelper::needsScheduling(I))
      continue;
    unsigned stateNum = schedule.at(&I);
    insnsByStateNum[stateNum].push_back(&I);
  }

  for (auto insnsForState : insnsByStateNum) {
    // Find longest delay in instructions for this state,
    // which is guaranteed to be a topologically sorted DAG
    std::map<Instruction *, double> delays;

    // Initialize all nodes to 0
    for (auto I : insnsForState.second) {
      delays[I] = 0;
    }

    // Propagate delays
    for (auto I : insnsForState.second) {
      for (auto use = I->user_begin(), use_end = I->user_end(); use != use_end;
           use++) {
        if (Instruction *Iuse = dyn_cast<Instruction>(*use)) {
          if (delays.find(Iuse) == delays.end())
            continue;
          delays[Iuse] =
              std::max(delays[Iuse], delays[I] + SchedHelper::getInsnDelay(*I));
        }
      }
    }

    // Find max delay
    auto maxDelayIt =
        std::max_element(delays.begin(), delays.end(),
                         [](const std::pair<Instruction *, double> a,
                            const std::pair<Instruction *, double> b) {
                           return a.second < b.second;
                         });

    // Find path
    std::list<Instruction *> path;
    Instruction *I = (*maxDelayIt).first;

    // outs() << (*maxDelayIt).first << "\n";
    // outs() << (*maxDelayIt).second << "\n";

    while (1) {
      // outs() << "I:" << *I << "\n";
      path.push_front(I);

      // Check if there are dependencies
      bool found = false;
      bool depsInThisCycleExist = false;
      for (auto dep = I->op_begin(), dep_end = I->op_end(); dep != dep_end;
           dep++) {
        // outs() << "  dep: " << **dep << "\n";
        Instruction *Idep = dyn_cast<Instruction>(*dep);
        if (!Idep)
          continue;
        if (delays.find(Idep) == delays.end())
          continue;
        depsInThisCycleExist = true;
        if ((delays[Idep] + SchedHelper::getInsnDelay(*Idep)) == delays[I]) {
          I = Idep;
          found = true;
          break;
        }
      }
      if (!depsInThisCycleExist)
        break;
      assert(found);
    }

    if ((*maxDelayIt).second > clockPeriodConstraint) {
      errs() << "Timing violation in function " << bb.getParent()->getName()
             << " basic block " << bb.getName()
             << ".  Longest chained delay path is " << (*maxDelayIt).second
             << " ns, which violates the clock period of "
             << clockPeriodConstraint << " ns. "
             << "The violating path contains the following instructions:\n";
      // errs() << path.size() << "\n";
      for (auto I : path) {
        errs() << "  " << *I << " (delay " << SchedHelper::getInsnDelay(*I)
               << "ns)\n";
      }
      report_fatal_error("Invalid schedule");
    }
  }
}

void Scheduler625::outputScheduleGantt(Function &F) {
  std::string funcName = F.getName();
  // StringRef fileName =
  StringRef fileName = funcName + ".tex";

  outs() << "\n  Creating " << fileName << "\n";

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
      for (auto dep : fHLS->getDeps(I)) {
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
      if (schedule.find(&I) != schedule.end())
        globalSchedule[&I] = cycleNum + schedule.at(&I);
    }
    cycleNum += getMaxCycle(bb) + 1;
  }
}

int Scheduler625::getMaxCycle(BasicBlock &bb) {
  int maxCycle = 0;
  for (auto &I : bb) {
    if (schedule.find(&I) == schedule.end())
      continue;
    maxCycle =
        std::max(maxCycle, schedule.at(&I) +
                               std::max(SchedHelper::getInsnLatency(I) - 1, 0));
  }
  return maxCycle;
}