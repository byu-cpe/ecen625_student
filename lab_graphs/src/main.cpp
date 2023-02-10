#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <libgen.h>
#include <map>
#include <regex>
#include <span>
#include <unistd.h>
#include <vector>

#include "niGraph/NIGraph.h"
#include "niGraphReader/NIGraphReader.h"
#include "solution.h"
#include "utils.h"

// Print a list of nodes
void printNodeList(const NIGraphNodeList &nodes) {
  const int NODES_PER_LINE = 10;

  int i = 0;
  for (auto nodeIt = nodes.begin(); nodeIt != nodes.end(); nodeIt++, i++) {
    NIGraphNode *node = *nodeIt;
    if (nodeIt != nodes.begin()) {
      std::cout << " -> ";
      if (i % NODES_PER_LINE == 0)
        std::cout << "\n";
    }
    std::cout << node->getId();
  }
  std::cout << "\n";
}

int main(int argc, char *argv[]) {
  auto args = std::span(argv, size_t(argc));

  const int pathBufSize = 512;

  // Get path to graphs
  auto exePath = std::filesystem::canonical("/proc/self/exe");
  auto asstDirPath = exePath.parent_path().parent_path();
  auto graphDirPath = asstDirPath / "niGraphs";
  auto dotDirPath = asstDirPath / "dot";

  std::cout << "Graph directory path:" << graphDirPath << "\n";

  // Get list of graphs in graph directory
  // std::string graphsGlob = graphDirPath / "DelayGraph_*.graphml";
  std::vector<std::filesystem::path> graphPaths;

  if (argc == 1) {
    for (const auto &entry :
         std::filesystem::directory_iterator(graphDirPath)) {
      graphPaths.push_back(entry);
    }
    std::cout << graphPaths.size() << " graphs found in specified directory\n";
  } else {
    for (int i = 1; i < argc; i++) {
      graphPaths.push_back(std::string("../niGraphs/DelayGraph_") +
                           std::string(args[i]) + ".graphml");
    }
    for (auto path : graphPaths) {
      if (!std::filesystem::exists(path)) {
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

    std::string graphPathStr = graphPath.string();
    std::regex_match(graphPathStr, matches, regex_str);
    assert(matches.size() == 2);
    int graphId = atoi(matches[1].str().c_str());
    graphPathsById[graphId] = graphPath;
  }

  // Create graph reader object
  NIGraphReader graphReader;

  // Open a file to write results out to
  std::ofstream fp;
  fp.open("results.csv");
  fp << "file_path,graph_size,runtime,longest_path\n";

  // Loop through the graphs
  for (auto graphPathIt : graphPathsById) {
    std::string ID = std::to_string(graphPathIt.first);
    auto graphPath = graphPathIt.second;

    // Print which graph we are working on
    std::cout << graphPath << "\n";

    // Parse the graph
    std::unique_ptr<NIGraph> graph =
        graphReader.parseGraphMlFile(ID, graphPath);

    // Time the topological sort
    timestamp_t t0 = get_timestamp();
    NIGraphNodeList sortedNodes = topologicalSort(*graph);
    timestamp_t t1 = get_timestamp();

    bool err = !graph->nodeListIsTopological(sortedNodes);

    // Find the longest path
    int delay = 0;
    std::map<NIGraphNode *, int> nodeDelays;
    NIGraphNodeList longestPath;
    if (!err) {
      delay = longestDelayPath(sortedNodes, longestPath, nodeDelays);
    }

    // Create a DOT file (only if we aren't doing all the graphs)
    if (argc > 1) {
      std::string dotPath = dotDirPath / ("graph" + graph->getID() + ".dot");
      std::string pdfPath = dotDirPath / ("graph" + graph->getID() + ".pdf");
      createDOT(*graph, dotPath, longestPath, nodeDelays);
      std::string stdout =
          exec(std::string("dot ") + dotPath + " -Tpdf -o " + pdfPath);
      std::cout << stdout;
    }

    // Write the results out to the file
    if (!err) {
      unsigned long graphSize =
          graph->getNodes().size() + graph->getEdges().size();
      fp << graphPath << "," << graphSize << ","
         << (t1 - t0) / MICROSECS_PER_SEC << "," << delay << "\n";
    }

    if (argc > 1) {
      std::cout << "Topological sort of nodes:\n";
      printNodeList(sortedNodes);
      std::cout << "\nLongest path:\n";
      printNodeList(longestPath);
    }

    if (err)
      return -1;

    // break;
  }

  // Close the file
  fp.close();

  return 0;
}
