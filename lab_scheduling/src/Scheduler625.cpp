#include "Scheduler625.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <regex>

#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Pass.h>
#include <llvm/Support/CommandLine.h>

#include "FunctionHLS.h"
#include "FunctionalUnit.h"
#include "SchedHelper.h"

using namespace llvm;

cl::opt<bool> ILPFlag("ILP", cl::desc("Use ILP scheduler instead of ASAP."));
cl::opt<bool> QuietFlag("quietSched", cl::desc("Only print error messages"),
                        cl::init(false));
cl::opt<double> PeriodOpt("period");

char Scheduler625::ID = 0;
static RegisterPass<Scheduler625> X("sched625", "HLS Scheduler for ECEN 625",
                                    false /* Only looks at CFG? */,
                                    true /* Analysis pass? */);

bool Scheduler625::runOnFunction(Function &F) {
  // FUs = new FunctionalUnits(F);
  fHLS = new FunctionHLS(F);

  clockPeriodConstraint = PeriodOpt;

  // Loop through basic blocks
  for (auto &bb : F.getBasicBlockList()) {
    if (ILPFlag)
      scheduleILP(bb);
    else
      scheduleASAP(bb);
  }

  std::error_code EC;

  std::string timingRptName = F.getName().str() + ".timing.rpt";
  std::string scheduleRptName = F.getName().str() + ".schedule.rpt";
  std::string ganttRptName = F.getName().str() + ".tex";

  raw_fd_ostream scheduleRpt(scheduleRptName, EC);
  if (EC.value())
    report_fatal_error("Could not open schedule rpt: " + scheduleRptName);

  raw_fd_ostream timingRpt(timingRptName, EC);
  if (EC.value())
    report_fatal_error("Could not open" + timingRptName);

  raw_fd_ostream ganttRpt(ganttRptName, EC);
  if (EC.value())
    report_fatal_error("Could not open" + ganttRptName);

  scheduleGlobal(F);
  reportSchedule(F, scheduleRpt);
  validateScheduleAndReportTiming(F, timingRpt);
  outputScheduleGantt(F, ganttRpt);

  scheduleRpt.close();
  timingRpt.close();
  ganttRpt.close();

  return false;
}

void Scheduler625::reportSchedule(Function &F, raw_fd_ostream &scheduleFile) {
  for (auto &bb : F) {
    scheduleFile << "Basic Block " << bb.getName() << "\n";
    for (int i = 0; i <= getMaxCycle(bb); i++) {
      scheduleFile << "  Cycle " << i << "\n";
      for (auto it : schedule) {
        if ((it.second == i) && (it.first->getParent() == &bb))
          scheduleFile << "      " << *(it.first) << "\n";
      }
      scheduleFile << "\n";
    }
    scheduleFile << "\n";
  }
}

void Scheduler625::validateScheduleAndReportTiming(Function &F,
                                                   raw_fd_ostream &timingRpt) {
  if (!QuietFlag)
    outs() << "\nValidating Schedule for Function " << F.getName() << "\n";
  for (auto &bb : F) {
    validateScheduleAndReportTiming(bb, timingRpt);
  }
}

void Scheduler625::validateScheduleAndReportTiming(BasicBlock &bb,
                                                   raw_fd_ostream &timingRpt) {
  if (!QuietFlag)
    outs() << "  BB \"" << bb.getName().str() << "\" of function \""
           << bb.getParent()->getName().str() << "\"...\n";

  // Check that all instructions have been assigned to a state
  if (!QuietFlag)
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

  if (!QuietFlag)
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
  if (!QuietFlag)
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
    }
    // Double check that initiation intervals are 1
    assert(FU->getInitiationInterval() == 1);
  }

  // Check critical paths
  if (!QuietFlag)
    outs() << "    Checking critical path\n";
  std::map<unsigned, std::vector<Instruction *>> insnsByStateNum;
  for (auto &I : bb) {
    if (!SchedHelper::needsScheduling(I))
      continue;
    unsigned stateNum = schedule.at(&I);
    insnsByStateNum[stateNum].push_back(&I);
  }

  std::map<Instruction *, double> delays;

  for (auto insnsForState : insnsByStateNum) {
    // Find longest delay in instructions for this state,
    // which is guaranteed to be a topologically sorted DAG

    // Initialize all nodes to their delay
    for (auto I : insnsForState.second)
      delays[I] = SchedHelper::getInsnDelay(*I);

    // Propagate delays
    for (auto I : insnsForState.second) {
      for (auto use = I->user_begin(), use_end = I->user_end(); use != use_end;
           use++) {
        if (Instruction *Iuse = dyn_cast<Instruction>(*use)) {
          if (!SchedHelper::needsScheduling(*Iuse))
            continue;
          if (schedule.at(Iuse) != schedule.at(I))
            continue;
          if (delays.find(Iuse) == delays.end())
            continue;
          delays[Iuse] = std::max(delays[Iuse],
                                  delays[I] + SchedHelper::getInsnDelay(*Iuse));
        }
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

  while (1) {
    path.push_front(I);

    // Check if there are dependencies
    bool found = false;
    bool depsInThisCycleExist = false;
    for (auto dep = I->op_begin(), dep_end = I->op_end(); dep != dep_end;
         dep++) {
      Instruction *Idep = dyn_cast<Instruction>(*dep);
      if (!Idep or !SchedHelper::needsScheduling(*Idep))
        continue;
      if (schedule.at(Idep) != schedule.at(I))
        continue;
      if (delays.find(Idep) == delays.end())
        continue;
      depsInThisCycleExist = true;
      if ((delays[Idep] + SchedHelper::getInsnDelay(*I)) == delays[I]) {
        I = Idep;
        found = true;
        break;
      }
    }
    if (!depsInThisCycleExist)
      break;
    assert(found);
  }

  // Write longest path to file
  timingRpt << "Basic block " << bb.getName().str() << "\n";
  timingRpt << "  Longest path: " << (*maxDelayIt).second << " ns\n";
  for (auto I : path) {
    std::string str;
    llvm::raw_string_ostream ss(str);
    ss << *I;
    timingRpt << "    (" << SchedHelper::getInsnDelay(*I) << "ns)  " << str
              << " \n";
  }

  if (((*maxDelayIt).second > clockPeriodConstraint) &&
      (clockPeriodConstraint != 0)) {
    errs() << "Timing violation in function " << bb.getParent()->getName()
           << " basic block " << bb.getName()
           << ".  Longest chained delay path is " << (*maxDelayIt).second
           << " ns, which violates the clock period of "
           << clockPeriodConstraint << " ns. "
           << "The violating path contains the following instructions:\n";
    for (auto I : path) {
      errs() << "  " << *I << " (delay " << SchedHelper::getInsnDelay(*I)
             << "ns)\n";
    }
    report_fatal_error("Invalid schedule");
  }
}

void Scheduler625::outputScheduleGantt(Function &F, raw_fd_ostream &ganttFile) {
  std::string funcName(F.getName());

  int numRows = 0;
  for (auto &bb : F) {
    numRows++;
    for (auto &I : bb) {
      if (!SchedHelper::needsScheduling(I))
        continue;
      numRows++;
    }
  }
  int paperHeight = 2.0 + numRows / 2.2;

  ganttFile << "\\documentclass[12pt]{article}\n";
  ganttFile << "\\usepackage{lscape}\n";
  ganttFile << "\\usepackage[margin=0.5in,paperwidth=12in,paperheight="
            << paperHeight
            << "in]{"
               "geometry}\n";
  ganttFile << "\\usepackage{pgfgantt}\n";
  ganttFile << "\\begin{document}\n";
  ganttFile << "\\section*{Schedule of "
            << std::regex_replace(funcName, std::regex("_"), "\\_") << "}\n";

  // Find max cycleNum
  int cycleNumEnd = 0;
  for (auto &bb : F) {
    cycleNumEnd = std::max(cycleNumEnd, globalBBStart[&bb] + getMaxCycle(bb));
  }
  ganttFile << "\\begin{ganttchart}["
            << "bar top shift = 0.1, bar height = 0.8,bar/.append "
               "style={fill=blue!50},hgrid,vgrid"
            << "]{0}{" << cycleNumEnd << "}\n";
  ganttFile << "\\gantttitlelist{1,...," << cycleNumEnd + 1 << "}{1} \\\\ \n";

  int elem = 0;
  std::map<Value *, int> elemMap;
  for (auto &bb : F) {
    elemMap[&bb] = elem;
    elem++;

    ganttFile << "\\ganttgroup{" << bb.getName().str() << "} {"
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

      const int Ilength = 60;
      if (str.length() > Ilength)
        str = str.substr(0, Ilength - 3) + "...";

      ganttFile << "\\ganttbar{" << str << "}{" << globalSchedule[&I] << "}{"
                << globalSchedule[&I] +
                       std::max(SchedHelper::getInsnLatency(I) - 1, 0)
                << "} \\\\ \n";

      // Add links
      elemMap[&I] = elem;
      for (auto dep : fHLS->getDeps(I)) {
        ganttFile << "\\ganttlink{elem" << elemMap[dep] << "}{elem" << elem
                  << "}\n";
      }
      elem++;
    }
  }

  for (auto &bb : F) {
    for (auto successor : successors(&bb)) {
      // ganttFile << "\\ganttlink{elem" << elemMap[&bb] << "}{elem"
      //        << elemMap[successor] << "}\n";
    }
  }

  ganttFile << "\\end{ganttchart}\n";
  ganttFile << "\\end{document}\n";
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