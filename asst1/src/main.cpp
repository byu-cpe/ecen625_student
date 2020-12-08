/*
 * 522r_asst1.cpp
 *
 *  Created on: Jan 6, 2017
 *      Author: jgoeders
 */

#include <cstdlib>
#include <iostream>
#include <map>
#include <regex>
#include <vector>

#include "niGraph/NIGraph.h"
#include "niGraphReader/NIGraphReader.h"
#include "utils.h"

// Print a list of nodes
void printNodeList(const NIGraphNodeList &nodes) {
  int i = 0;
  for (auto nodeIt = nodes.begin(); nodeIt != nodes.end(); nodeIt++, i++) {
    NIGraphNode *node = *nodeIt;
    if (nodeIt != nodes.begin()) {
      std::cout << " -> ";
      if (i % 10 == 0)
        std::cout << "\n";
    }
    std::cout << node->getId();
  }
  std::cout << "\n";
}

// Create a DOT file of the graph
// - graph: The graph to output in DOT format
// - outputPath: The path of the output file
void createDOT(const NIGraph &graph, const std::string outputPath) {
  // add code here

  void createDOTSoln(const NIGraph &graph, const std::string outputPath);
  createDOTSoln(graph, outputPath);
}

// Perform a topological sort of the graph
// graph - Input graph
// Return value - List of nodes sorted in topological order
NIGraphNodeList topologicalSort(const NIGraph &graph) {
  NIGraphNodeList q;

  // add code here
  NIGraphNodeList topologicalSortSoln(const NIGraph &graph);
  q = topologicalSortSoln(graph);

  return q;
}

// Find the longest delay path based on a topological sorting of nodes
// topolSortedNodes - List of nodes, sorted in topological order
// longestPath - Return value, list of nodes on longest path, in topological
// order Return value - Total delay value of longest path
int longestDelayPath(const NIGraphNodeList &topolSortedNodes,
                     NIGraphNodeList &longestPath) {
  // add code here
}

int main(int argc, char *argv[]) {
  // Parse arguments
  assert(argc == 1 || argc == 3);

  //

  // Get list of graphs in graph directory
  std::string graphsGlob = "../niGraphs/DelayGraph_*.graphml";
  std::vector<std::string> graphPaths = glob(graphsGlob);

  // Check that graphs were found
  if (graphPaths.size() == 0) {
    std::cout << "No graphs found in specified directory\n";
    exit(1);
  } else {
    std::cout << graphPaths.size() << " graphs found in specified directory\n";
  }

  // Store graphs in a map by graph #
  // Use a regex to extract the graph # from the path string
  // A map structure will automatically keep the values sorted by key
  std::map<int, std::string> graphPathsById;
  for (auto graphPath : graphPaths) {
    std::regex regex_str(".*DelayGraph_(\\d+)\\.graphml");
    std::smatch matches;

    std::regex_match(graphPath, matches, regex_str);
    assert(matches.size() == 2);
    int graphId = atoi(matches[1].str().c_str());
    graphPathsById[graphId] = graphPath;
  }

  // Create graph reader object
  NIGraphReader graphReader;

  // Parse graph 0, and create a DOT file
  NIGraph *graph0 = graphReader.parseGraphMlFile(graphPathsById[0]);
  createDOT(*graph0, "graph0.dot");
  delete graph0;

  // Open a file to write results out to
  std::ofstream fp;
  fp.open("results.txt");

  // Loop through the graphs
  for (auto graphPathIt : graphPathsById) {
    auto graphPath = graphPathIt.second;

    // Print which graph we are working on
    std::cout << graphPath << "\n";

    // Parse the graph
    NIGraph *graph = graphReader.parseGraphMlFile(graphPath);

    // Time the topological sort
    timestamp_t t0 = get_timestamp();
    NIGraphNodeList sortedNodes = topologicalSort(*graph);
    timestamp_t t1 = get_timestamp();

    bool err = !graph->nodeListIsTopological(sortedNodes);

    // Find the longest path
    int delay;
    if (!err) {
      NIGraphNodeList longestPath;
      delay = longestDelayPath(sortedNodes, longestPath);
    }

    // Write the results out to the file
    if (!err) {
      int graphSize = graph->getNodes().size() + graph->getEdges().size();
      fp << graphPath << " " << graphSize << " "
         << (t1 - t0) / MICROSECS_PER_SEC << " " << delay << "\n";
    }

    // printNodeList(sortedNodes);
    delete graph;

    if (err)
      return -1;

    // break;
  }

  // Close the file
  fp.close();

  return 0;
}
