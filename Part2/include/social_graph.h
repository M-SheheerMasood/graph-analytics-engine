#ifndef SOCIAL_GRAPH_H
#define SOCIAL_GRAPH_H

#include "graph.h"
#include <unordered_map>
#include <vector>
#include <utility> // For std::pair

/**
 * @class SocialGraph
 * ... (doxygen comments as before) ...
 */
class SocialGraph final : public Graph {
public:
    // --- Constructor and Rule of Five ---
    SocialGraph();
    ~SocialGraph() override = default;
    SocialGraph(SocialGraph&& other) noexcept;
    SocialGraph& operator=(SocialGraph&& other) noexcept;
    SocialGraph(const SocialGraph&) = delete;
    SocialGraph& operator=(const SocialGraph&) = delete;

    // --- Overridden Interface Functions ---
    void addVertex(int node) override;
    void removeVertex(int node) override;
    void addEdge(int from, int to, int weight = 1) override;
    void removeEdge(int from, int to) override;
    bool hasEdge(int from, int to) const override;
    std::vector<int> getAdjacent(int from) const override;
    std::optional<std::vector<int>> findShortestPath(int start, int end) const override;

    // --- NEW: Advanced Network Analysis Functions for PA3 ---

    /**
     * @brief Detects mutual follow relationships, or "echo chambers".
     * An echo chamber is defined as a pair of users (A, B) where A follows B AND B follows A.
     * @return A vector of pairs, where each pair represents a mutual follow relationship.
     */
    std::vector<std::pair<int, int>> findEchoChambers() const;

    /**
     * @brief Calculates an influence score for each user using the PageRank algorithm.
     * PageRank assigns a score based not just on the number of followers, but on the
     * importance of those followers.
     * @param damping The damping factor (usually 0.85).
     * @param iterations The number of iterations to run the algorithm.
     * @return An unordered_map mapping each userID to their calculated PageRank score.
     */
    std::unordered_map<int, double> calculatePageRank(double damping = 0.85, int iterations = 10) const;

    /**
     * @brief Finds communities within the graph using the Kosaraju's algorithm for Strongly Connected Components (SCCs).
     * An SCC is a group of users where every user can reach every other user in the group through some path of follows.
     * @return A vector of vectors, where each inner vector is a community of userIDs.
     */
    std::vector<std::vector<int>> findCommunities() const;

private:
    // --- Private Helper Functions for Algorithms ---
    void kosaraju_dfs1(int u, std::unordered_map<int, bool>& visited, std::vector<int>& finish_stack) const;
    void kosaraju_dfs2(int u, std::unordered_map<int, bool>& visited, std::vector<int>& component, const std::unordered_map<int, std::vector<int>>& transposed_graph) const;

    // --- Core Data Structure ---
    std::unordered_map<int, std::vector<int>> adjList;
};

#endif // SOCIAL_GRAPH_H