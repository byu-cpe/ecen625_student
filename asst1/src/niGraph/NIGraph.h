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
  std::vector<NIGraphNode *> nodes;
  std::map<std::string, NIGraphNode *> idMapNode;

  std::vector<NIGraphEdge *> edges;
  std::map<std::string, NIGraphEdge *> idMapEdge;

public:
  NIGraph();
  virtual ~NIGraph();

  bool nodeListIsTopological(NIGraphNodeList &list);

  void addNode(NIGraphNode *);
  void addEdge(NIGraphEdge *);

  std::string stats();

  int getNumNodes() { return nodes.size(); }
  NIGraphNode *findNodeById(std::string id);

  std::vector<NIGraphEdge *> const &getEdges() const { return edges; }
  std::vector<NIGraphNode *> const &getNodes() const { return nodes; }
};

#endif /* SRC_GRAPH_GRAPH_H_ */
