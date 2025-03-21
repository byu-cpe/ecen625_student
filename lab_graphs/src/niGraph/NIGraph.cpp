/*
 * Graph.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: jgoeders
 */

#include "NIGraph.h"

#include <cassert>
#include <set>
#include <sstream>
#include <string>

using namespace std;

NIGraph::NIGraph(std::string ID) : ID(ID) {
  // TODO Auto-generated constructor stub
}

NIGraph::~NIGraph() {
  for (auto &node : nodes) {
    delete node;
  }

  for (auto &edge : edges) {
    delete edge;
  }
}

void NIGraph::addNode(NIGraphNode *node) {
  string id = node->getId();
  assert(idMapNode.find(id) == idMapNode.end());

  idMapNode[id] = node;

  nodesReadOnly.push_back(node);
  nodes.push_back(node);
}

void NIGraph::addEdge(NIGraphEdge *edge) {
  string id = edge->getId();
  assert(idMapEdge.find(id) == idMapEdge.end());

  idMapEdge[id] = edge;

  edge->getSourceNode()->addOutEdge(edge);
  edge->getDestNode()->addInEdge(edge);

  edgesReadOnly.push_back(edge);
  edges.push_back(edge);
}

std::string NIGraph::stats() {
  stringstream s;

  s << "Number of nodes: " << getNumNodes() << endl;

  size_t numOutEdges = 0;
  size_t numInEdges = 0;
  for (auto &node : nodes) {
    numOutEdges += node->getOutEdges().size();
    numInEdges += node->getInEdges().size();
  }
  s << "Avg num out edges: " << ((double)numOutEdges / (double)getNumNodes())
    << endl;
  s << "Avg num in edges: " << ((double)numInEdges / (double)getNumNodes())
    << endl;

  return s.str();
}

NIGraphNode *NIGraph::findNodeById(std::string id) {
  assert(idMapNode.find(id) != idMapNode.end());

  return idMapNode[id];
}

bool NIGraph::nodeListIsTopological(NIGraphNodeList &list) {
  if (list.size() != nodes.size()) {
    std::cerr << "Graph has " << nodes.size()
              << " nodes, but topological list has " << list.size()
              << " nodes.\n";
    return false;
  }

  std::set<NIGraphNode *> verify_visited;
  for (auto node : list) {
    for (auto e : node->getOutEdges()) {
      if (e->getIsFeedback())
        continue;
      if (verify_visited.find(e->getDestNode()) != verify_visited.end()) {
        std::cerr << "Node " << node->getId() << " connects to node "
                  << e->getDestNode()->getId()
                  << ", but is later in the sorted list.\n";
        return false;
      }
    }
    verify_visited.insert(node);
  }
  return true;
}
