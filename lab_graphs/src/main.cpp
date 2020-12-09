/*
 * 522r_asst1.cpp
 *
 *  Created on: Jan 6, 2017
 *      Author: jgoeders
 */

#include <cstdlib>
#include <iostream>
#include <libgen.h>
#include <map>
#include <regex>
#include <unistd.h>
#include <vector>

#include "niGraph/NIGraph.h"
#include "niGraphReader/NIGraphReader.h"
#include "utils.h"

#define BUF_SIZE 512

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
void createDOT(const NIGraph &graph, const std::string outputPath,
               NIGraphNodeList &longestPath,
               std::map<NIGraphNode *, int> &nodeDelays) {
  // add code here

}

// Perform a topological sort of the graph
// graph - Input graph
// Return value - List of nodes sorted in topological order
NIGraphNodeList topologicalSort(const NIGraph &graph) {
  NIGraphNodeList q;

  // add code here
 

  return q;
}

// Find the longest delay path based on a topological sorting of nodes
// topolSortedNodes - List of nodes, sorted in topological order
// longestPath - Return list of nodes on longest path, in topological order
// nodeDelays - Return map, nodeDelays[node] = longest path to node
// Return value - Total delay value of longest path
int longestDelayPath(const NIGraphNodeList &topolSortedNodes,
                     NIGraphNodeList &longestPath,
                     std::map<NIGraphNode *, int> &nodeDelays) {
  // add code here
   return 0;
}

int main(int argc, char *argv[]) {

  // Get path to graphs
  char buf[BUF_SIZE];
  readlink("/proc/self/exe", buf, BUF_SIZE);

  char *buildDirPath = dirname(buf);
  std::string asstDirPath = dirname(buildDirPath);
  std::string graphDirPath = asstDirPath + "/niGraphs";
  std::string dotDirPath = asstDirPath + "/dot";

  std::cout << graphDirPath << "\n";

  // Get list of graphs in graph directory
  std::string graphsGlob = graphDirPath + "/DelayGraph_*.graphml";
  std::vector<std::string> graphPaths;

  if (argc == 1) {
    graphPaths = glob(graphsGlob);
    std::cout << graphPaths.size() << " graphs found in specified directory\n";
  } else {
    for (int i = 1; i < argc; i++) {
      graphPaths.push_back(std::string("../niGraphs/DelayGraph_") + argv[i] +
                           ".graphml");
    }
    for (auto path : graphPaths) {
      if (!fileExists(path)) {
        std::cerr << path << " does not exist.\n";
        return -1;
      }
    }
  }

  // Check that graphs were found
  if (graphPaths.size() == 0) {
    std::cout << "No graphs found in specified directory\n";
    exit(1);
  } else {
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

  // Open a file to write results out to
  std::ofstream fp;
  fp.open("results.txt");

  // Loop through the graphs
  for (auto graphPathIt : graphPathsById) {
    std::string ID = std::to_string(graphPathIt.first);
    auto graphPath = graphPathIt.second;

    // Print which graph we are working on
    std::cout << graphPath << "\n";

    // Parse the graph
    NIGraph *graph = graphReader.parseGraphMlFile(ID, graphPath);

    // Time the topological sort
    timestamp_t t0 = get_timestamp();
    NIGraphNodeList sortedNodes = topologicalSort(*graph);
    timestamp_t t1 = get_timestamp();

    bool err = !graph->nodeListIsTopological(sortedNodes);

    // Find the longest path
    int delay;
    std::map<NIGraphNode *, int> nodeDelays;
    NIGraphNodeList longestPath;
    if (!err) {
      delay = longestDelayPath(sortedNodes, longestPath, nodeDelays);
    }

    // Create a DOT file (only if we aren't doing all the graphs)
    if (argc > 1) {
      std::string dotPath = dotDirPath + "/graph" + graph->getID() + ".dot";
      std::string pdfPath = dotDirPath + "/graph" + graph->getID() + ".pdf";
      createDOT(*graph, dotPath, longestPath, nodeDelays);
      std::string stdout =
          exec(std::string("dot ") + dotPath + " -Tpdf -o " + pdfPath);
      std::cout << stdout;
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
