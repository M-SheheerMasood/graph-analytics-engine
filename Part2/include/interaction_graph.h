#ifndef INTERACTION_GRAPH_H
#define INTERACTION_GRAPH_H

#include <vector>
#include <unordered_map>
#include <utility> // For std::pair
#include<optional>

/**
 * @class InteractionGraph
 * @brief A weighted, directed, bipartite graph modeling User-Post interactions.
 *
 * This graph has two sets of vertices: Users and Posts. Edges only exist from
 * a User to a Post, representing an interaction like a "like" or "comment".
 * The weight of the edge signifies the strength of the interaction.
 */
class InteractionGraph {
public:
    // Enum to specify the type of vertex being added or removed.
    enum class NodeType { USER, POST };

    // Struct to represent a weighted interaction.
    struct Interaction {
        int targetID; // ID of the user or post on the other side of the edge
        int weight;   // Strength of interaction (e.g., 1 for like, 5 for comment)
    };

    // --- Constructor and Rule of Five ---
    InteractionGraph();
    ~InteractionGraph() = default;
    InteractionGraph(InteractionGraph&& other) noexcept;
    InteractionGraph& operator=(InteractionGraph&& other) noexcept;
    InteractionGraph(const InteractionGraph&) = delete;
    InteractionGraph& operator=(const InteractionGraph&) = delete;

    // --- Graph Management ---
    void addVertex(int id, NodeType type);
    void removeVertex(int id, NodeType type);
    void addInteraction(int userID, int postID, int weight);

    // --- Core and Advanced Analysis Functions ---

    /**
     * @brief Finds users with similar tastes to a given user based on their interactions.
     * Similarity is calculated using the Jaccard Index on the sets of posts they've interacted with.
     * @param userID The ID of the user to find similar users for.
     * @param topN The number of similar users to return.
     * @return A vector of pairs {similarUserID, similarityScore}, sorted by score.
     */
    std::vector<std::pair<int, double>> findSimilarUsers(int userID, int topN) const;

    /**
     * @brief Recommends posts to a user based on the activity of similar users.
     * Implements a basic collaborative filtering algorithm.
     * @param userID The ID of the user to generate recommendations for.
     * @param topN The number of recommended posts to return.
     * @return A vector of post IDs, sorted by recommendation strength.
     */
    std::vector<int> recommendPosts(int userID, int topN) const;

    /**
     * @brief Calculates a "trend score" for every post.
     * The score is a weighted sum of interactions, where interactions from more
     * influential users (determined by their PageRank) contribute more.
     * @param pageRanks A map of {userID -> pageRankScore} from the SocialGraph.
     * @return A map of {postID -> trendScore}, which can be sorted to find trending posts.
     */
    std::unordered_map<int, double> calculateTrendScores(const std::unordered_map<int, double>& pageRanks) const;
    /**
     * @brief Calculates a valid processing order for posts and their user dependencies.
     * This function treats the bipartite graph as a Directed Acyclic Graph (DAG) where
     * posts depend on users who interact with them. It uses a topological sort
     * to find an order where users are processed before the posts they interact with.
     * @return A vector of node IDs (both users and posts) in a valid topological order.
     *         Returns std::nullopt if the logic implies a cycle (which shouldn't happen
     *         in this bipartite model but is good practice to handle).
     */
    std::optional<std::vector<int>> getProcessingOrder() const;


private:
    // Two adjacency lists to represent the bipartite graph structure.
    // Maps a user ID to the posts they've interacted with.
    std::unordered_map<int, std::vector<Interaction>> userToPostEdges;
    
    // Maps a post ID to the users who have interacted with it (the inverse graph).
    // This is crucial for efficient recommendations and trend analysis.
    std::unordered_map<int, std::vector<Interaction>> postToUserEdges;
};

#endif // INTERACTION_GRAPH_H