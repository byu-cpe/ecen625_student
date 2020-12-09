/*
 * GraphNode.h
 *
 *  Created on: Nov 10, 2016
 *      Author: jgoeders
 */

#ifndef SRC_GRAPH_GRAPHNODE_H_
#define SRC_GRAPH_GRAPHNODE_H_

#include <string>
#include <iostream>
#include <vector>

class NIGraphEdge;

class NIGraphNode {
private:
	std::string id;
	int vertexId;
	int nodeUniqueId;
	int throughputCostIfReg;
	int latencyCostIfReg;
	int registerCostIfReg;
	bool isRegistered;
	bool isInputTerminal;
	bool isOutputTerminal;
	bool disallowReg;
	int nodeType;

	std::vector<NIGraphEdge*> outEdges;
	std::vector<NIGraphEdge*> inEdges;



public:
	NIGraphNode();
	virtual ~NIGraphNode();

	void setId(std::string idStr) { id = idStr; }
	std::string getId() { return id; }

	void setVertexId(int id) { vertexId = id; }

	void addOutEdge(NIGraphEdge* edge);
	void addInEdge(NIGraphEdge* edge);

	std::vector<NIGraphEdge*> & getOutEdges() { return outEdges; }
	std::vector<NIGraphEdge*> & getInEdges() { return inEdges; }

	void setNodeUniqueId(int id) { nodeUniqueId = id; }

	void setThroughputCostIfReg(int cost) { throughputCostIfReg = cost; }

	void setIsRegistered( bool isReg) { isRegistered = isReg; }

	void setIsInputTerminal(bool isInput) { isInputTerminal = isInput; }

	void setIsOutputTerminal(bool isOutput) { isOutputTerminal = isOutput; }

	void setLatencyCostIfReg(int cost) { latencyCostIfReg = cost; }

	void setRegCostIfReg(int cost) { registerCostIfReg = cost; }

	void setDisallowReg(bool disallow) { disallowReg = disallow; }

	void setNodeType(int type) { nodeType = type; }
};

#endif /* SRC_GRAPH_GRAPHNODE_H_ */
