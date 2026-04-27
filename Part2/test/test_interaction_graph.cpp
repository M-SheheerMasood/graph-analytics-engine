
#include "../include/interaction_graph.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip> // For std::setprecision, std::left, std::setw
#include <vector>
#include <functional>
#include <string>
#include <unordered_map>
#include <set>
#include <cmath> // For std::abs

// ANSI color codes from test_hash.cpp
#define RESET   "\033[0m"
#define FAIL    "\033[1;31m" // Bold Red
#define PASS    "\033[1;32m" // Bold Green
#define SKIP    "\033[1;33m" // Bold Yellow
#define TEST_NAME "\033[1;34m" // Bold Blue
#define HEADER  "\033[1;35m" // Bold Magenta
#define TIME    "\033[1;36m" // Bold Cyan
#define FINAL_SCORE "\033[1;42;30m" // Black on Green BG
#define BOLD    "\033[1m"

// Test result tracking
struct TestResults {
    int passed = 0;
    int failed = 0;
    double points_earned = 0.0;
    double total_points_possible = 0.0;

    void print_summary(double final_weight) const {
        double percentage = (total_points_possible > 0) ? (points_earned / total_points_possible) : 0.0;
        double final_mark = percentage * final_weight;

        std::cout << "\n" << SKIP << "----------------------------------------------" << RESET << std::endl;
        std::cout << TIME << "Test results for InteractionGraph:" << RESET << std::endl;
        std::cout << PASS << "Tests Passed: " << passed << "/" << (passed + failed) << RESET << std::endl;
        std::cout << PASS << "Raw Score: " << std::fixed << std::setprecision(1) << points_earned << "/" << total_points_possible << RESET << std::endl;
        std::cout << PASS << "Weighted Score: " << final_mark << "/" << final_weight << RESET << std::endl;
        std::cout << SKIP << "----------------------------------------------" << RESET << std::endl;
        
        std::cout << "\n" << HEADER << "========================================" << RESET << std::endl;
        std::cout << FINAL_SCORE << " FINAL SCORE: " << final_mark << "/" << (int)final_weight << " " << RESET << std::endl;
        std::cout << HEADER << "========================================" << RESET << std::endl;
    }
};

TestResults results;

#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "\n" << FAIL << "  └> Assertion failed at line " << __LINE__ << ": " << (message) << RESET; \
        return 0; \
    }

// Helper: compare unordered collections of ints
static bool intSetEquals(const std::vector<int>& a, const std::vector<int>& b) {
    std::multiset<int> sa(a.begin(), a.end());
    std::multiset<int> sb(b.begin(), b.end());
    return sa == sb;
}

// Helper: approximate double comparison
static bool approxEqual(double x, double y, double eps = 1e-6) {
    return std::abs(x - y) <= eps;
}

// --- Basic Tests ---

int test_constructor() {
    InteractionGraph graph1;
    // Empty graph should work

    InteractionGraph graph2;
    graph2.addVertex(1, InteractionGraph::NodeType::USER);
    graph2.addVertex(101, InteractionGraph::NodeType::POST);

    InteractionGraph graph3(std::move(graph2));
    // Move constructor should work
    TEST_ASSERT(true, "Constructor and move-constructor");

    return 1;
}

int test_addVertex() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER); // Add user
    graph.addVertex(2, InteractionGraph::NodeType::USER); // Add another user
    graph.addVertex(101, InteractionGraph::NodeType::POST); // Add post
    graph.addVertex(102, InteractionGraph::NodeType::POST); // Add another post

    graph.addVertex(1, InteractionGraph::NodeType::USER); // Duplicate should be handled
    TEST_ASSERT(true, "Add vertices (User and Post)");
    return 1;
}

int test_addInteraction() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);
    graph.addVertex(102, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 5); // User 1 likes post 101 with weight 5
    graph.addInteraction(1, 102, 3); // User 1 likes post 102 with weight 3
    graph.addInteraction(2, 101, 4); // User 2 likes post 101 with weight 4
    
    // Check non-existent interaction add
    graph.addInteraction(3, 101, 1); // User 3 doesn't exist
    graph.addInteraction(1, 103, 1); // Post 103 doesn't exist

    TEST_ASSERT(true, "Add interactions");
    return 1;
}

int test_removeVertex() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 5);
    graph.addInteraction(2, 101, 3);

    graph.removeVertex(1, InteractionGraph::NodeType::USER); // Remove user and their interactions
    
    // We can't easily check for the edge, but we can check via TrendScores
    std::unordered_map<int, double> pageRanks;
    pageRanks[1] = 1.0; // This user is removed
    pageRanks[2] = 1.0;
    
    auto trends = graph.calculateTrendScores(pageRanks);
    // Score should only be 1.0 * 3 = 3.0 (from user 2)
    TEST_ASSERT(approxEqual(trends[101], 3.0), "Trend score should not include removed user 1");

    return 1;
}

int test_removePost() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);
    graph.addVertex(102, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 5);
    graph.addInteraction(1, 102, 3);

    graph.removeVertex(101, InteractionGraph::NodeType::POST); // Remove post and its interactions
    
    // Check recommendations, should only find 102 (which user 1 saw)
    auto recs = graph.recommendPosts(1, 1); // Will be empty, but check internals
    TEST_ASSERT(recs.empty(), "No recommendations for self");
    
    // Check similar users
    auto similar = graph.findSimilarUsers(1, 1);
    TEST_ASSERT(similar.empty(), "No other users to be similar to");

    return 1;
}

// --- Similarity Tests ---

int test_findSimilarUsers() {
    InteractionGraph graph;

    for (int i = 1; i <= 5; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::USER);
    }
    for (int i = 101; i <= 105; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::POST);
    }

    // User 1 and 2 have similar tastes
    graph.addInteraction(1, 101, 1);
    graph.addInteraction(1, 102, 1);
    graph.addInteraction(1, 103, 1);

    graph.addInteraction(2, 101, 1);
    graph.addInteraction(2, 102, 1);

    // User 3 has different taste
    graph.addInteraction(3, 104, 1);
    graph.addInteraction(3, 105, 1);

    auto similar = graph.findSimilarUsers(1, 2);
    TEST_ASSERT(!similar.empty(), "Should find at least one similar user");
    TEST_ASSERT(similar[0].first == 2, "User 2 should be most similar");
    // Exact Jaccard similarity between user1 {101,102,103} and user2 {101,102} is 2/3
    TEST_ASSERT(approxEqual(similar[0].second, 2.0 / 3.0), "Jaccard index should be 2/3");

    return 1;
}

int test_similarUsersJaccardCalculation() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);
    graph.addVertex(102, InteractionGraph::NodeType::POST);

    // Perfect overlap
    graph.addInteraction(1, 101, 1);
    graph.addInteraction(1, 102, 1);
    graph.addInteraction(2, 101, 1);
    graph.addInteraction(2, 102, 1);

    auto similar = graph.findSimilarUsers(1, 1);
    TEST_ASSERT(similar.size() == 1, "Should find 1 similar user");
    TEST_ASSERT(approxEqual(similar[0].second, 1.0), "Jaccard similarity should be ~1.0");

    return 1;
}

int test_similarUsersNoOverlap() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);
    graph.addVertex(102, InteractionGraph::NodeType::POST);

    // No overlap
    graph.addInteraction(1, 101, 1);
    graph.addInteraction(2, 102, 1);

    auto similar = graph.findSimilarUsers(1, 1);
    TEST_ASSERT(similar.empty(), "No overlap => no similar users returned");

    return 1;
}

// --- Recommendation Tests ---

int test_recommendPosts() {
    InteractionGraph graph;

    for (int i = 1; i <= 10; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::USER);
    }
    for (int i = 101; i <= 110; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::POST);
    }

    // Users 2-6 are similar to user 1
    for (int user = 1; user <= 6; user++) {
        graph.addInteraction(user, 101, 1);
        graph.addInteraction(user, 102, 1);
    }

    // Similar users also liked posts 103-105
    for (int user = 2; user <= 6; user++) {
        graph.addInteraction(user, 103, 1);
        graph.addInteraction(user, 104, 1);
        graph.addInteraction(user, 105, 1);
    }

    auto recommendations = graph.recommendPosts(1, 3);
    TEST_ASSERT(recommendations.size() <= 3, "Should return at most 3 recommendations");
    TEST_ASSERT(!recommendations.empty(), "Should find some recommendations");

    // The expected recommended set is {103,104,105}
    TEST_ASSERT(intSetEquals(recommendations, std::vector<int>{103, 104, 105}), "Recommendations should be {103, 104, 105}");

    return 1;
}

int test_recommendPostsNoSimilarUsers() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 1);

    auto recommendations = graph.recommendPosts(1, 5);
    TEST_ASSERT(recommendations.empty(), "No similar users, no recommendations");

    return 1;
}

int test_recommendPostsAlreadyInteracted() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);
    graph.addVertex(102, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 1);
    graph.addInteraction(2, 101, 1);
    graph.addInteraction(2, 102, 1);

    auto recommendations = graph.recommendPosts(1, 5);
    TEST_ASSERT(!recommendations.empty(), "Should recommend post 102");
    TEST_ASSERT(recommendations.size() == 1, "Should only recommend post 102");
    TEST_ASSERT(recommendations[0] == 102, "Should recommend post 102");

    return 1;
}

// --- Trend Score Tests ---

int test_calculateTrendScores() {
    InteractionGraph graph;

    for (int i = 1; i <= 3; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::USER);
    }
    for (int i = 101; i <= 102; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::POST);
    }

    graph.addInteraction(1, 101, 5);
    graph.addInteraction(2, 101, 3);
    graph.addInteraction(3, 102, 2);

    std::unordered_map<int, double> pageRanks;
    pageRanks[1] = 0.5; // User 1 is influential
    pageRanks[2] = 0.3;
    pageRanks[3] = 0.2;

    auto trendScores = graph.calculateTrendScores(pageRanks);
    TEST_ASSERT(trendScores.size() == 2, "Should have scores for 2 posts");
    // Expected: post 101 score = 0.5*5 + 0.3*3 = 2.5 + 0.9 = 3.4
    //           post 102 score = 0.2*2 = 0.4
    TEST_ASSERT(approxEqual(trendScores[101], 3.4), "Post 101 score should be 3.4");
    TEST_ASSERT(approxEqual(trendScores[102], 0.4), "Post 102 score should be 0.4");

    return 1;
}

int test_trendScoresEmptyPageRanks() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);
    graph.addInteraction(1, 101, 5);

    std::unordered_map<int, double> emptyPageRanks;

    auto trendScores = graph.calculateTrendScores(emptyPageRanks);
    // With empty pageRanks, trend should be 0 for posts that exist
    TEST_ASSERT(trendScores.size() == 1, "Should have score for 1 post");
    TEST_ASSERT(approxEqual(trendScores[101], 0.0), "No PageRank data => 0 score");

    return 1;
}

int test_trendScoresHighInfluence() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 1);
    graph.addInteraction(2, 101, 10); // High weight but low influence

    std::unordered_map<int, double> pageRanks;
    pageRanks[1] = 0.9; // Very influential
    pageRanks[2] = 0.1; // Not influential

    auto trendScores = graph.calculateTrendScores(pageRanks);
    // Expected: trend = 0.9*1 + 0.1*10 = 0.9 + 1 = 1.9
    TEST_ASSERT(approxEqual(trendScores[101], 1.9), "Score should be (0.9*1 + 0.1*10) = 1.9");

    return 1;
}

// --- Topological Sort Tests ---

int test_getProcessingOrder() {
    InteractionGraph graph;

    for (int i = 1; i <= 3; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::USER);
    }
    for (int i = 101; i <= 102; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::POST);
    }

    graph.addInteraction(1, 101, 1);
    graph.addInteraction(2, 101, 1);
    graph.addInteraction(3, 102, 1);

    auto order = graph.getProcessingOrder();
    TEST_ASSERT(order.has_value(), "Should have valid order");
    TEST_ASSERT(order->size() == 5, "Order should have 3 users + 2 posts");

    // Users should come before posts they interact with
    std::unordered_map<int, size_t> positions;
    for (size_t i = 0; i < order->size(); i++) {
        positions[(*order)[i]] = i;
    }

    TEST_ASSERT(positions[1] < positions[101], "User 1 must come before post 101");
    TEST_ASSERT(positions[2] < positions[101], "User 2 must come before post 101");
    TEST_ASSERT(positions[3] < positions[102], "User 3 must come before post 102");

    return 1;
}

int test_processingOrderEmptyGraph() {
    InteractionGraph graph;

    auto order = graph.getProcessingOrder();
    TEST_ASSERT(order.has_value(), "Empty graph has valid order");
    TEST_ASSERT(order->empty(), "Order is empty");

    return 1;
}

int test_processingOrderNoInteractions() {
    InteractionGraph graph;

    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);

    auto order = graph.getProcessingOrder();
    TEST_ASSERT(order.has_value(), "Should have valid order");
    TEST_ASSERT(order->size() == 3, "All nodes present");

    return 1;
}

// --- Edge Cases ---

int test_largeScaleInteractions() {
    InteractionGraph graph;

    const int NUM_USERS = 100;
    const int NUM_POSTS = 100;

    for (int i = 1; i <= NUM_USERS; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::USER);
    }
    for (int i = 1001; i <= 1000 + NUM_POSTS; i++) {
        graph.addVertex(i, InteractionGraph::NodeType::POST);
    }

    // Each user interacts with 10 posts
    for (int user = 1; user <= NUM_USERS; user++) {
        for (int post = 1001; post < 1011; post++) {
            graph.addInteraction(user, post, user % 5 + 1);
        }
    }

    auto similar = graph.findSimilarUsers(1, 10);
    TEST_ASSERT(!similar.empty(), "Should find similar users");

    auto recommendations = graph.recommendPosts(1, 5);
    TEST_ASSERT(recommendations.empty(), "No recommendations, all similar users saw same posts");

    std::unordered_map<int, double> pageRanks;
    for (int i = 1; i <= NUM_USERS; i++) {
        pageRanks[i] = 1.0 / NUM_USERS;
    }
    auto trendScores = graph.calculateTrendScores(pageRanks);
    TEST_ASSERT(!trendScores.empty(), "Should have trend scores");

    return 1;
}

int test_recommendPosts_noOverlap() {
    InteractionGraph graph;
    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 1);
    graph.addInteraction(2, 101, 1);

    auto recs = graph.recommendPosts(1, 5);
    TEST_ASSERT(recs.empty(), "Should not recommend posts user 1 already saw");
    
    return 1;
}

int test_topologicalSort_multipleRoots() {
    InteractionGraph graph;
    graph.addVertex(1, InteractionGraph::NodeType::USER);
    graph.addVertex(2, InteractionGraph::NodeType::USER);
    graph.addVertex(101, InteractionGraph::NodeType::POST);
    graph.addVertex(102, InteractionGraph::NodeType::POST);

    graph.addInteraction(1, 101, 1);
    graph.addInteraction(2, 102, 1);

    auto order = graph.getProcessingOrder();
    TEST_ASSERT(order.has_value(), "Valid order must exist");
    TEST_ASSERT(order->size() == 4, "Order must contain all 4 nodes");

    std::unordered_map<int, size_t> positions;
    for (size_t i = 0; i < order->size(); i++) {
        positions[(*order)[i]] = i;
    }
    TEST_ASSERT(positions[1] < positions[101], "User 1 must come before post 101");
    TEST_ASSERT(positions[2] < positions[102], "User 2 must come before post 102");

    return 1;
}


// Test registry
struct TestCase {
    std::function<int()> func;
    std::string name;
    double weight; // Points this test is worth
    bool enabled;
};

std::vector<TestCase> all_tests = {
    // Basic Operations (10 points)
    {test_constructor, "constructor", 2, true},
    {test_addVertex, "addVertex", 2, true},
    {test_addInteraction, "addInteraction", 2, true},
    {test_removeVertex, "removeVertex (User)", 2, true},
    {test_removePost, "removeVertex (Post)", 2, true},

    // Similarity (25 points)
    {test_findSimilarUsers, "findSimilarUsers", 15, true},
    {test_similarUsersJaccardCalculation, "similarUsers (Jaccard=1.0)", 5, true},
    {test_similarUsersNoOverlap, "similarUsers (Jaccard=0.0)", 5, true},

    // Recommendations (25 points)
    {test_recommendPosts, "recommendPosts (Collab Filter)", 15, true},
    {test_recommendPostsNoSimilarUsers, "recommendPosts (No Similar)", 3, true},
    {test_recommendPostsAlreadyInteracted, "recommendPosts (No New Posts)", 4, true},
    {test_recommendPosts_noOverlap, "recommendPosts (No Overlap)", 3, true}, // NEW

    // Trend Scores (20 points)
    {test_calculateTrendScores, "calculateTrendScores", 10, true},
    {test_trendScoresEmptyPageRanks, "trendScores (Empty PageRank)", 5, true},
    {test_trendScoresHighInfluence, "trendScores (Influence vs Weight)", 5, true},

    // Topological Sort (20 points)
    {test_getProcessingOrder, "getProcessingOrder (Kahn's)", 10, true},
    {test_processingOrderEmptyGraph, "processingOrder (Empty)", 3, true},
    {test_processingOrderNoInteractions, "processingOrder (No Edges)", 3, true},
    {test_topologicalSort_multipleRoots, "processingOrder (Multi-Root)", 4, true}, // NEW
    
    // Large Scale (Implicitly tested by recommend/similar)
    {test_largeScaleInteractions, "largeScaleInteractions", 0, true}, // 0 points, just a sanity check
};
// TOTAL: 100 points

int main() {
    std::cout << HEADER << "========================================" << RESET << std::endl;
    std::cout << HEADER << "   InteractionGraph Testing Suite" << RESET << std::endl;
    std::cout << HEADER << "========================================" << RESET << std::endl;
    std::cout << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (auto& test : all_tests) {
        if (!test.enabled) continue;

        results.total_points_possible += test.weight;
        
        // Print test name in Bold Blue, left-aligned
        std::cout << TEST_NAME << "Testing " << std::left << std::setw(35) << test.name << ": " << RESET;
        std::cout.flush(); // Force output before running test

        int result = test.func(); // Run the test

        if (result == 1) {
            std::cout << PASS << "Passed!" << RESET << " (" << test.weight << " pts)" << std::endl;
            results.passed++;
            results.points_earned += test.weight;
        } else {
            // Fail message (from ASSERT) is printed first
            std::cout << " " << FAIL << "Failed!" << RESET << " (0 pts)" << std::endl;
            results.failed++;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Print the final summary, weighting to 40 marks
    results.print_summary(40.0);
    
    std::cout << "\n" << TIME << "Test completed in: " << duration.count() << "ms" << RESET << std::endl;

    // NEW LINE:
    return (int)results.points_earned;
}