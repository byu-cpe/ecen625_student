/*
 * Graph.h
 *
 *  Created on: Nov 10, 2016
 *      Author: jgoeders
 */

#ifndef SRC_GRAPH_GRAPH_H_
#define SRC_GRAPH_GRAPH_H_

#include <deque>
#include <map>
#include <memory>
#include <vector>

#include "NIGraphEdge.h"
#include "NIGraphNode.h"

typedef std::deque<NIGraphNode *> NIGraphNodeList;

class NIGraph {
private:
  std::string ID;

  std::vector<std::unique_ptr<NIGraphNode>> nodes;
  std::vector<NIGraphNode *> nodesReadOnly;
  std::map<std::string, NIGraphNode *> idMapNode;

  std::vector<std::unique_ptr<NIGraphEdge>> edges;
  std::vector<NIGraphEdge *> edgesReadOnly;
  std::map<std::string, NIGraphEdge *> idMapEdge;

public:
  NIGraph(std::string ID);
  virtual ~NIGraph();

  bool nodeListIsTopological(NIGraphNodeList &list);

  void addNode(std::unique_ptr<NIGraphNode> node);
  void addEdge(std::unique_ptr<NIGraphEdge> edge);

  std::string stats();

  std::string &getID() { return ID; }
  int getNumNodes() { return nodes.size(); }
  NIGraphNode *findNodeById(std::string id);

  std::vector<NIGraphEdge *> const &getEdges() const { return edgesReadOnly; }
  std::vector<NIGraphNode *> const &getNodes() const { return nodesReadOnly; }
};

#endif /* SRC_GRAPH_GRAPH_H_ */
