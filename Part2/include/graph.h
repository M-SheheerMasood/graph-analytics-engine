#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <optional> // For modern C++ optional return values

/**
 * @class Graph
 * @brief An abstract base class (interface) for a graph data structure.
 * ... (doxygen comments as before) ...
 */
class Graph {
public:
    virtual ~Graph() = default;

    // --- Vertex Management ---
    /**
     * @brief Adds a new vertex (node) to the graph.
     * @param node The ID of the node to add.
     */
    virtual void addVertex(int node) = 0;

    /**
     * @brief Removes a vertex and all of its associated edges from the graph.
     * @param node The ID of the node to remove.
     */
    virtual void removeVertex(int node) = 0;


    // --- Edge Management ---
    virtual void addEdge(int from, int to, int weight = 1) = 0;
    virtual void removeEdge(int from, int to) = 0;
    
    // --- Graph Queries ---
    virtual bool hasEdge(int from, int to) const = 0;
    virtual std::vector<int> getAdjacent(int from) const = 0;
    virtual std::optional<std::vector<int>> findShortestPath(int start, int end) const = 0;
};

#endif // GRAPH_H