/*
 * GraphReader.h
 *
 *  Created on: Nov 10, 2016
 *      Author: jgoeders
 */

#ifndef SRC_GRAPHREADER_GRAPHREADER_H_
#define SRC_GRAPHREADER_GRAPHREADER_H_

#include <memory>
#include <string>

#include <rapidxml-1.13/rapidxml.hpp>

class NIGraph;
class NIGraphNode;

class NIGraphReader {
private:
  void parseNode(rapidxml::xml_node<> &node, NIGraph &graph);
  void parseEdge(rapidxml::xml_node<> &node, NIGraph &graph);

public:
  NIGraphReader();
  virtual ~NIGraphReader();

  NIGraph *parseGraphMlFile(std::string filePath);
};

#endif /* SRC_GRAPHREADER_GRAPHREADER_H_ */
