/*
 * GraphNode.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: jgoeders
 */

#include "NIGraphNode.h"

#include <algorithm>
#include <cassert>

using namespace std;

NIGraphNode::NIGraphNode()
    : vertexId(0), nodeUniqueId(0), throughputCostIfReg(0), latencyCostIfReg(0),
      registerCostIfReg(0), isRegistered(false), isInputTerminal(false),
      isOutputTerminal(false), disallowReg(false), nodeType(0) {}

NIGraphNode::~NIGraphNode() {}

void NIGraphNode::addOutEdge(NIGraphEdge *edge) {
  assert(std::find(outEdges.begin(), outEdges.end(), edge) == outEdges.end());
  outEdges.push_back(edge);
}

void NIGraphNode::addInEdge(NIGraphEdge *edge) {
  assert(std::find(inEdges.begin(), inEdges.end(), edge) == inEdges.end());
  inEdges.push_back(edge);
}
