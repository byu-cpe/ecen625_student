/*
 * AdderTreeBalancer.h
 *
 *  Created on: Feb 11, 2017
 *      Author: jgoeders
 */

#ifndef LLVM_LIB_TRANSFORMS_LEGUP_ADDERTREEBALANCER_H_
#define LLVM_LIB_TRANSFORMS_LEGUP_ADDERTREEBALANCER_H_

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/Pass.h>

#include <deque>

using namespace llvm;

namespace legup {

class AdderTreeBalancer : public BasicBlockPass {
  public:
    AdderTreeBalancer();
    virtual ~AdderTreeBalancer();

    static char ID;

  private:
  public:
    bool runOnBasicBlock(BasicBlock &BB);
};
}
#endif /* LLVM_LIB_TRANSFORMS_LEGUP_ADDERTREEBALANCER_H_ */
