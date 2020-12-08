#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "NIGraphReader.h"
#include "niGraph/NIGraph.h"
#include "niGraph/NIGraphEdge.h"
#include "utils.h"

using namespace std;

NIGraphReader::NIGraphReader() {
  // TODO Auto-generated constructor stub
}

NIGraphReader::~NIGraphReader() {
  // TODO Auto-generated destructor stub
}

void NIGraphReader::parseNode(rapidxml::xml_node<> &node, NIGraph &graph) {
  NIGraphNode *graphNode = new NIGraphNode();

  rapidxml::xml_attribute<> *attr;
  attr = node.first_attribute("id");

  string id = attr->value();
  graphNode->setId(id);

  rapidxml::xml_node<> *n = node.first_node();
  while (n) {
    assert(string(n->name()) == "data");

    attr = n->first_attribute();
    assert(string(attr->name()) == "key");

    std::string propName = attr->value();
    if (propName == "VertexId") {
      int vertexId = stoi(n->value());
      graphNode->setVertexId(vertexId);
    } else if (propName == "NodeUniqueId") {
      int nodeUniqueId = stoi(n->value());
      graphNode->setNodeUniqueId(nodeUniqueId);
    } else if (propName == "ThroughputCostIfRegistered") {
      int throughputCostIfReg = stoi(n->value());
      graphNode->setThroughputCostIfReg(throughputCostIfReg);
    } else if (propName == "LatencyCostIfRegistered") {
      int latencyCostIfReg = stoi(n->value());
      graphNode->setLatencyCostIfReg(latencyCostIfReg);
    } else if (propName == "RegisterCostIfRegistered") {
      int regCostIfReg = stoi(n->value());
      graphNode->setRegCostIfReg(regCostIfReg);
    } else if (propName == "IsRegistered") {
      bool isRegistered = boolStrToBool(n->value());
      graphNode->setIsRegistered(isRegistered);
    } else if (propName == "IsInputTerminal") {
      bool isInputTerm = boolStrToBool(n->value());
      graphNode->setIsInputTerminal(isInputTerm);
    } else if (propName == "IsOutputTerminal") {
      bool isOutputTerm = boolStrToBool(n->value());
      graphNode->setIsOutputTerminal(isOutputTerm);
    } else if (propName == "DisallowRegister") {
      bool disallowReg = boolStrToBool(n->value());
      graphNode->setDisallowReg(disallowReg);
    } else if (propName == "NodeType") {
      int nodeType = stoi(n->value());
      graphNode->setNodeType(nodeType);
    } else {
      cout << propName << endl;
      assert(false);
    }
    n = n->next_sibling();
  }

  graph.addNode(graphNode);
}

void NIGraphReader::parseEdge(rapidxml::xml_node<> &node, NIGraph &graph) {
  NIGraphEdge *graphEdge = new NIGraphEdge();

  string id = node.first_attribute("id")->value();
  graphEdge->setId(id);

  NIGraphNode *sourceNode =
      graph.findNodeById(node.first_attribute("source")->value());
  graphEdge->setSourceNode(sourceNode);

  NIGraphNode *destNode =
      graph.findNodeById(node.first_attribute("target")->value());
  graphEdge->setDestNode(destNode);

  rapidxml::xml_node<> *childNode = node.first_node();
  while (childNode) {
    std::string propName = childNode->first_attribute("key")->value();

    if (propName == "Delay") {
      int delay = stoi(childNode->value());
      graphEdge->setDelay(delay);
    } else if (propName == "IsFeedback") {
      bool feedback = boolStrToBool(childNode->value());
      graphEdge->setIsFeedback(feedback);
    }

    childNode = childNode->next_sibling();
  }

  graph.addEdge(graphEdge);
  //	<edge id="e21" source="n31" target="n32">
  //	      <data key="Delay">809</data>
  //	      <data key="IsFeedback">False</data>
  //	    </edge>
}

NIGraph *NIGraphReader::parseGraphMlFile(std::string filePath) {
  assert(fileExists(filePath));

  NIGraph *graph = new NIGraph();

  std::ifstream fin(filePath);

  rapidxml::xml_document<> doc;
  vector<char> buffer((std::istreambuf_iterator<char>(fin)),
                      std::istreambuf_iterator<char>());
  buffer.push_back('\0');
  doc.parse<0>(&buffer[0]);

  rapidxml::xml_node<> *node;
  rapidxml::xml_node<> *graphMlNode = doc.first_node();
  rapidxml::xml_node<> *graphNode = NULL;

  const std::string strKey = "key";
  const std::string strGraph = "graph";
  const std::string strNode = "node";
  const std::string strEdge = "edge";

  // Loop through graphML
  node = graphMlNode->first_node();
  while (node) {
    if (node->name() == strKey) {
      // ignore
    } else if (node->name() == strGraph) {
      assert(!graphNode); // ensure we only encounter 1 graph
      graphNode = node;
    } else {
      cout << node->name() << endl;
      assert(false);
    }
    node = node->next_sibling();
  }
  assert(graphNode);

  // Loop through graph node
  node = graphNode->first_node();
  while (node) {
    if (node->name() == strNode) {
      parseNode(*node, *graph);
    } else if (node->name() == strEdge) {
      parseEdge(*node, *graph);
    }
    node = node->next_sibling();
  }

  return graph;
}
