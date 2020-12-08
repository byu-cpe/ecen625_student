/*
 * GraphNode.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: jgoeders
 */

#include "NIGraphNode.h"

#include <cassert>
#include <algorithm>


using namespace std;

NIGraphNode::NIGraphNode() {
	// TODO Auto-generated constructor stub

}

NIGraphNode::~NIGraphNode() {
	// TODO Auto-generated destructor stub
}

void NIGraphNode::addOutEdge(NIGraphEdge* edge) {
	assert(std::find(outEdges.begin(), outEdges.end(), edge) == outEdges.end());
	outEdges.push_back(edge);
}

void NIGraphNode::addInEdge(NIGraphEdge* edge) {
	assert(std::find(inEdges.begin(), inEdges.end(), edge) == inEdges.end());
	inEdges.push_back(edge);
}
