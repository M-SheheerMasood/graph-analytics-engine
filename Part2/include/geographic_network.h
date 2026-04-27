#ifndef GEOGRAPHIC_NETWORK_H
#define GEOGRAPHIC_NETWORK_H

#include "graph.h"
#include <vector>
#include <unordered_map>
#include <string>

/**
 * @class GeographicNetwork
 * @brief A concrete implementation of the Graph interface for modeling a physical network.
 *
 * This graph represents a network of users connected based on their physical location.
 * Properties:
 * - Undirected: An edge between A and B is a two-way connection.
 * - Weighted: The edge weight represents the network latency (cost) between two locations.
 * - Representation: Uses an Adjacency List to store weighted edges.
 */
class GeographicNetwork final : public Graph {
private:
    // A helper struct to represent a weighted edge in the adjacency list.
    struct Edge {
        int to;     // The destination vertex
        int weight; // The latency/cost of the edge
    };

public:
    // A helper struct for the return type of the MST calculation.
    struct MstEdge {
        int u, v, weight;
    };

    // --- Constructor and Rule of Five ---
    GeographicNetwork();
    ~GeographicNetwork() override = default;

    // Move semantics for efficient resource management.
    GeographicNetwork(GeographicNetwork&& other) noexcept;
    GeographicNetwork& operator=(GeographicNetwork&& other) noexcept;

    // Prevent costly copies.
    GeographicNetwork(const GeographicNetwork&) = delete;
    GeographicNetwork& operator=(const GeographicNetwork&) = delete;

    // --- Overridden Interface Functions ---
    void addVertex(int node) override;
    void removeVertex(int node) override;
    void addEdge(int from, int to, int weight) override;
    void removeEdge(int from, int to) override;
    bool hasEdge(int from, int to) const override;
    std::vector<int> getAdjacent(int from) const override;
    std::optional<std::vector<int>> findShortestPath(int start, int end) const override;

    // --- Core Specialized Algorithm ---
    std::vector<MstEdge> calculateMinimumSpanningTree() const;

    // --- Advanced Network Analysis Functions ---

    /**
     * @brief Finds all articulation points (critical nodes) in the network.
     * An articulation point is a vertex whose removal would disconnect the network.
     * @return A vector of node IDs that are critical points of failure.
     */
    std::vector<int> findCriticalNodes() const;

    /**
     * @brief Finds a path from start to end with the minimum "effort".
     * The effort of a path is defined by the maximum weight of any single edge in that path.
     * This function should find a path that minimizes this maximum edge weight.
     * @param start The starting node ID.
     * @param end The destination node ID.
     * @return An std::optional containing the path with the minimum effort, 
     *         or std::nullopt if no path exists.
     */
    std::optional<std::vector<int>> findPathWithMinEffort(int start, int end) const;
     /**
     * @brief Finds the city with the smallest number of neighbors reachable within a given distance threshold.
     * This is useful for finding a well-connected yet central location for a new resource.
     * @param distanceThreshold The maximum path cost to consider a city "reachable".
     * @return The ID of the optimal city. Returns -1 if the graph is empty.
     */
    int findBestCity(int distanceThreshold) const;


private:
    std::unordered_map<int, std::vector<Edge>> adjList;
    
    // Private Helper

    void findAP_dfs(int u, int& time, std::unordered_map<int, int>& parent, 
                    std::unordered_map<int, int>& disc, std::unordered_map<int, int>& low, 
                    std::vector<int>& result) const;
};

#endif // GEOGRAPHIC_NETWORK_H