#ifndef SOLUTION_H
#define SOLUTION_H

#include "niGraph/NIGraph.h"

// Create a DOT file of the graph
// - graph: The graph to output in DOT format
// - outputPath: The path of the output file
void createDOT(const NIGraph &graph, const std::string outputPath,
               NIGraphNodeList &longestPath,
               std::map<NIGraphNode *, int> &nodeDelays);

// Perform a topological sort of the graph
// graph - Input graph
// Return value - List of nodes sorted in topological order
NIGraphNodeList topologicalSort(const NIGraph &graph);

// Find the longest delay path based on a topological sorting of nodes
// topolSortedNodes - List of nodes, sorted in topological order
// longestPath - Return list of nodes on longest path, in topological order
// nodeDelays - Return map, nodeDelays[node] = longest path to node
// Return value - Total delay value of longest path
int longestDelayPath(const NIGraphNodeList &topolSortedNodes,
                     NIGraphNodeList &longestPath,
                     std::map<NIGraphNode *, int> &nodeDelays);

#endif /* SOLUTION_H */
