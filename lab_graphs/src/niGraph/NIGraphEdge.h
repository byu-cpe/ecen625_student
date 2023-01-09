/*
 * GraphEdge.h
 *
 *  Created on: Nov 13, 2016
 *      Author: jgoeders
 */

#ifndef SRC_GRAPH_GRAPHEDGE_H_
#define SRC_GRAPH_GRAPHEDGE_H_

#include <string>

class NIGraphNode;

class NIGraphEdge {
private:
  std::string id;

  NIGraphNode *sourceNode;
  NIGraphNode *destNode;

  int delay;
  bool isFeedback;

public:
  NIGraphEdge();
  virtual ~NIGraphEdge();

  void setId(std::string idStr) { id = idStr; }
  std::string getId() { return id; }

  void setSourceNode(NIGraphNode *node) { sourceNode = node; }
  NIGraphNode *getSourceNode() { return sourceNode; }

  void setDestNode(NIGraphNode *node) { destNode = node; }
  NIGraphNode *getDestNode() { return destNode; }

  void setDelay(int delayVal) { delay = delayVal; }
  int getDelay() { return delay; }

  void setIsFeedback(bool feedback) { isFeedback = feedback; }
  bool getIsFeedback() { return isFeedback; }
};

#endif /* SRC_GRAPH_GRAPHEDGE_H_ */
