
#include "../include/social_graph.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <chrono>
#include <iomanip> // For std::setprecision, std::left, std::setw
#include <vector>
#include <functional>
#include <string>

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
        std::cout << TIME << "Test results for SocialGraph:" << RESET << std::endl;
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

// Modified ASSERT to print failure message immediately
#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "\n" << FAIL << "  └> Assertion failed at line " << __LINE__ << ": " << (message) << RESET; \
        return 0; \
    }

// Helper: compare adjacency ignoring order
static bool adjacencyEquals(const SocialGraph& g, int node, const std::vector<int>& expected) {
    auto adj = g.getAdjacent(node);
    auto a = adj;
    auto b = expected;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

// Helper: compare two vectors for exact equality
static bool vectorEquals(const std::vector<int>& a, const std::vector<int>& b) {
    return a == b;
}

// Helper: compare communities as sets of sorted vectors (order of communities doesn't matter)
static bool communitiesEquals(const std::vector<std::vector<int>>& got, const std::vector<std::vector<int>>& expected) {
    std::vector<std::vector<int>> A = got;
    std::vector<std::vector<int>> B = expected;
    for (auto& v : A) std::sort(v.begin(), v.end());
    for (auto& v : B) std::sort(v.begin(), v.end());
    // Convert to comparable forms
    std::multiset<std::string> sa, sb;
    for (const auto& v : A) {
        std::string s;
        for (int x : v) { s += std::to_string(x) + ","; }
        sa.insert(s);
    }
    for (const auto& v : B) {
        std::string s;
        for (int x : v) { s += std::to_string(x) + ","; }
        sb.insert(s);
    }
    return sa == sb;
}

// --- All test functions (test_addVertex, test_addEdge, etc.) remain unchanged ---
// ... (They are identical to the previous version, so I'm omitting them for brevity) ...
// ... (The two new tests test_bfsWithCycle and test_pageRank_sinkNode are included) ...

int test_addVertex() {
    SocialGraph graph;
    graph.addVertex(1);
    TEST_ASSERT(graph.getAdjacent(1).empty(), "Single vertex should have no edges");
    graph.addVertex(2);
    graph.addVertex(3);
    TEST_ASSERT(graph.getAdjacent(2).empty(), "Vertex 2 should have no edges");
    TEST_ASSERT(graph.getAdjacent(3).empty(), "Vertex 3 should have no edges");
    graph.addVertex(1);
    TEST_ASSERT(graph.getAdjacent(1).empty(), "Duplicate vertex add should not change state");
    return 1;
}

int test_addEdge() {
    std::cout<<"IAM HERE 1\n";
    SocialGraph graph;
    std::cout<<"IAM HERE 2\n";
    graph.addVertex(1);
    std::cout<<"IAM HERE 3\n";
    graph.addVertex(2);
    graph.addVertex(3);
    std::cout<<"IAM HERE 4\n";
    graph.addEdge(1, 2);
    std::cout<<"IAM HERE 5\n";
    TEST_ASSERT(graph.hasEdge(1, 2), "Edge 1->2 should exist");
    std::cout<<"IAM HERE 6\n";
    TEST_ASSERT(!graph.hasEdge(2, 1), "Edge 2->1 should not exist (directed graph)");
    std::cout<<"IAM HERE 7\n";
    graph.addEdge(1, 3);
    std::cout<<"IAM HERE 8\n";
    TEST_ASSERT(adjacencyEquals(graph, 1, { 2, 3 }), "Vertex 1 should have 2 and 3 as adjacent nodes");
    std::cout<<"IAM HERE 9\n";
    return 1;
}

int test_noDuplicateEdges() {
    SocialGraph graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2);
    auto adjacent_before = graph.getAdjacent(1);
    TEST_ASSERT(adjacent_before.size() == 1, "Should have 1 edge initially");
    graph.addEdge(1, 2);
    auto adjacent_after = graph.getAdjacent(1);
    TEST_ASSERT(adjacent_after.size() == 1, "Should still have 1 edge after duplicate add");
    return 1;
}

int test_removeVertex() {
    SocialGraph graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 1);
    graph.removeVertex(2);
    TEST_ASSERT(graph.getAdjacent(2).empty(), "Removed vertex should have no adjacency");
    TEST_ASSERT(!graph.hasEdge(1, 2), "Edge 1->2 should be removed");
    TEST_ASSERT(!graph.hasEdge(2, 3), "Edge 2->3 should be removed");
    TEST_ASSERT(graph.hasEdge(3, 1), "Edge 3->1 should remain");
    return 1;
}

int test_removeEdge() {
    SocialGraph graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.removeEdge(1, 2);
    TEST_ASSERT(!graph.hasEdge(1, 2), "Edge 1->2 should be removed");
    TEST_ASSERT(graph.hasEdge(1, 3), "Edge 1->3 should remain");
    return 1;
}

int test_findShortestPath() {
    SocialGraph graph;
    for (int i = 1; i <= 4; i++) graph.addVertex(i);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);
    auto path = graph.findShortestPath(1, 4);
    TEST_ASSERT(path.has_value(), "Path from 1 to 4 should exist");
    TEST_ASSERT(vectorEquals(*path, std::vector<int>{1, 2, 3, 4}), "Path must be 1->2->3->4");
    auto self_path = graph.findShortestPath(1, 1);
    TEST_ASSERT(self_path.has_value(), "Path to self should exist");
    TEST_ASSERT(self_path->size() == 1, "Path to self should have length 1");
    graph.addVertex(5);
    auto no_path = graph.findShortestPath(1, 5);
    TEST_ASSERT(!no_path.has_value(), "No path should exist to disconnected vertex");
    return 1;
}

int test_findEchoChambers() {
    SocialGraph graph;
    for (int i = 1; i <= 6; i++) graph.addVertex(i);
    graph.addEdge(1, 2);
    graph.addEdge(2, 1);
    graph.addEdge(3, 4);
    graph.addEdge(4, 3);
    graph.addEdge(5, 6);
    auto chambers = graph.findEchoChambers();
    std::multiset<std::string> got_chambers;
    for (const auto& p : chambers) {
        int a = std::min(p.first, p.second);
        int b = std::max(p.first, p.second);
        got_chambers.insert(std::to_string(a) + "," + std::to_string(b));
    }
    std::multiset<std::string> expected_chambers = { "1,2", "3,4" };
    TEST_ASSERT(got_chambers == expected_chambers, "Should find both echo chamber pairs exactly");
    return 1;
}

int test_calculatePageRank() {
    SocialGraph graph;
    for (int i = 1; i <= 3; i++) graph.addVertex(i);
    graph.addEdge(2, 1);
    graph.addEdge(3, 1);
    auto ranks = graph.calculatePageRank(0.85, 10);
    TEST_ASSERT(ranks.size() == 3, "Should have 3 PageRank values");
    TEST_ASSERT(ranks[1] > ranks[2], "Node 1 should have higher rank than node 2");
    TEST_ASSERT(ranks[1] > ranks[3], "Node 1 should have higher rank than node 3");
    double sum = 0;
    for (const auto& pair : ranks) sum += pair.second;
    TEST_ASSERT(sum > 0.99 && sum < 1.01, "Sum of PageRanks should be approximately 1");
    return 1;
}

int test_findCommunities() {
    SocialGraph graph;
    for (int i = 1; i <= 5; i++) graph.addVertex(i);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 1);
    graph.addEdge(4, 5);
    graph.addEdge(5, 4);
    auto communities = graph.findCommunities();
    std::vector<std::vector<int>> expected_comm = { {1,2,3}, {4,5} };
    TEST_ASSERT(communitiesEquals(communities, expected_comm), "Should find communities {1,2,3} and {4,5}");
    return 1;
}

int test_moveSemantics() {
    SocialGraph graph1;
    graph1.addVertex(1);
    graph1.addVertex(2);
    graph1.addEdge(1, 2);
    SocialGraph graph2(std::move(graph1));
    TEST_ASSERT(graph2.hasEdge(1, 2), "Move constructor should transfer data");
    SocialGraph graph3;
    graph3 = std::move(graph2);
    TEST_ASSERT(graph3.hasEdge(1, 2), "Move assignment should transfer data");
    return 1;
}

int test_edgeCases() {
    SocialGraph graph;
    TEST_ASSERT(graph.getAdjacent(1).empty(), "getAdjacent on non-existent vertex should return empty list");
    TEST_ASSERT(!graph.hasEdge(1, 2), "hasEdge on non-existent vertices should return false");
    graph.removeVertex(999);
    graph.removeEdge(1, 2);
    graph.addEdge(1, 2);
    TEST_ASSERT(!graph.hasEdge(1, 2), "Edge add should fail if vertices don't exist");
    auto path = graph.findShortestPath(999, 1000);
    TEST_ASSERT(!path.has_value(), "Path between non-existent nodes should not exist");
    return 1;
}

int test_constructor() {
    SocialGraph graph1;
    TEST_ASSERT(graph1.getAdjacent(1).empty(), "Default constructor should create empty graph");
    TEST_ASSERT(!graph1.hasEdge(1, 2), "Empty graph should have no edges");
    auto ranks = graph1.calculatePageRank();
    TEST_ASSERT(ranks.empty(), "PageRank of empty graph should be empty");
    auto communities = graph1.findCommunities();
    TEST_ASSERT(communities.empty(), "Communities of empty graph should be empty");
    SocialGraph graph2;
    for (int i = 1; i <= 10; i++) {
        graph2.addVertex(i);
        if (i > 1) graph2.addEdge(i - 1, i);
    }
    SocialGraph graph3(std::move(graph2));
    TEST_ASSERT(graph3.hasEdge(1, 2), "Move constructor test 1");
    TEST_ASSERT(graph3.hasEdge(9, 10), "Move constructor test 2");
    TEST_ASSERT(graph3.getAdjacent(5).size() == 1, "Move constructor should preserve structure");
    SocialGraph graph4;
    graph4.addVertex(999);
    graph4 = std::move(graph3);
    TEST_ASSERT(graph4.hasEdge(1, 2), "Move assignment test 1");
    TEST_ASSERT(!graph4.hasEdge(999, 1000), "Move assignment should replace old data");
    return 1;
}

int test_edgeWeights() {
    SocialGraph graph;
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 2, 1);
    graph.addEdge(1, 3, 100);
    TEST_ASSERT(graph.hasEdge(1, 2), "Edge with weight 1 should exist");
    TEST_ASSERT(graph.hasEdge(1, 3), "Edge with weight 100 should exist");
    TEST_ASSERT(adjacencyEquals(graph, 1, { 2,3 }), "Adjacency of 1 should be {2, 3}");
    graph.addEdge(1, 2, 42);
    TEST_ASSERT(adjacencyEquals(graph, 1, { 2,3 }), "Adjacency should remain the same (no duplicate)");
    return 1;
}

int test_thoroughRemovalChecks() {
    SocialGraph graph;
    for (int i = 1; i <= 5; i++) graph.addVertex(i);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);
    graph.addEdge(4, 5);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 5);
    graph.addEdge(5, 1);
    graph.removeVertex(3);
    TEST_ASSERT(!graph.hasEdge(2, 3), "Edge 2->3 should be removed");
    TEST_ASSERT(!graph.hasEdge(3, 4), "Edge 3->4 should be removed");
    TEST_ASSERT(!graph.hasEdge(1, 3), "Edge 1->3 should be removed");
    TEST_ASSERT(!graph.hasEdge(3, 5), "Edge 3->5 should be removed");
    TEST_ASSERT(graph.hasEdge(1, 2), "Edge 1->2 should remain");
    TEST_ASSERT(graph.hasEdge(2, 4), "Edge 2->4 should remain");
    TEST_ASSERT(graph.hasEdge(4, 5), "Edge 4->5 should remain");
    TEST_ASSERT(graph.hasEdge(5, 1), "Edge 5->1 should remain");
    auto adj1 = graph.getAdjacent(1);
    TEST_ASSERT(std::find(adj1.begin(), adj1.end(), 3) == adj1.end(), "Vertex 3 should not be in adjacency of 1");
    auto adj2 = graph.getAdjacent(2);
    TEST_ASSERT(std::find(adj2.begin(), adj2.end(), 3) == adj2.end(), "Vertex 3 should not be in adjacency of 2");
    TEST_ASSERT(adj2.size() == 1, "Vertex 2 should have only 1 edge");
    graph.removeEdge(1, 2);
    graph.removeEdge(5, 1);
    TEST_ASSERT(!graph.hasEdge(1, 2), "Edge 1->2 should be removed");
    TEST_ASSERT(!graph.hasEdge(5, 1), "Edge 5->1 should be removed");
    TEST_ASSERT(graph.hasEdge(2, 4), "Edge 2->4 should still remain");
    return 1;
}

int test_duplicateOperations() {
    SocialGraph graph;
    for (int i = 0; i < 100; i++) {
        graph.addVertex(1);
    }
    TEST_ASSERT(graph.getAdjacent(1).empty(), "Multiple adds of same vertex should work");
    graph.addVertex(2);
    for (int i = 0; i < 100; i++) {
        graph.addEdge(1, 2);
    }
    TEST_ASSERT(graph.getAdjacent(1).size() == 1, "Duplicate edge adds should not create duplicates");
    for (int i = 0; i < 10; i++) {
        graph.removeEdge(1, 2);
    }
    TEST_ASSERT(!graph.hasEdge(1, 2), "Multiple removes should work");
    for (int i = 0; i < 10; i++) {
        graph.removeVertex(1);
    }
    TEST_ASSERT(graph.getAdjacent(1).empty(), "Vertex 1 should be gone after multiple removes");
    return 1;
}

int test_largeGraphOperations() {
    SocialGraph graph;
    const int NUM_NODES = 1000;
    for (int i = 1; i <= NUM_NODES; i++) {
        graph.addVertex(i);
    }
    for (int i = 1; i < NUM_NODES; i++) {
        graph.addEdge(i, i + 1);
    }
    for (int i = 1; i <= NUM_NODES; i += 10) {
        if (i + 50 <= NUM_NODES) {
            graph.addEdge(i, i + 50);
        }
    }
    TEST_ASSERT(graph.hasEdge(1, 2), "Chain edge test 1");
    TEST_ASSERT(graph.hasEdge(500, 501), "Chain edge test 2");
    TEST_ASSERT(graph.hasEdge(999, 1000), "Chain edge test 3");
    auto path = graph.findShortestPath(1, 1000);
    TEST_ASSERT(path.has_value(), "Path should exist in large graph");
    TEST_ASSERT(path->front() == 1, "Path should start at 1");
    TEST_ASSERT(path->back() == 1000, "Path should end at 1000");
    for (int i = 100; i <= 200; i++) {
        graph.removeVertex(i);
    }
    auto path2 = graph.findShortestPath(50, 250);
    TEST_ASSERT(!path2.has_value(), "Path should not exist through removed nodes");
    auto path3 = graph.findShortestPath(1, 99);
    TEST_ASSERT(path3.has_value(), "Path should exist around removed nodes");
    return 1;
}

int test_bfsCornerCases() {
    SocialGraph graph;
    graph.addVertex(1);
    auto path1 = graph.findShortestPath(1, 1);
    TEST_ASSERT(path1.has_value(), "Path to self should exist");
    TEST_ASSERT(path1->size() == 1, "Path to self should have length 1");
    TEST_ASSERT((*path1)[0] == 1, "Path to self should contain only self");
    auto path2 = graph.findShortestPath(999, 1);
    TEST_ASSERT(!path2.has_value(), "Path from non-existent source should not exist");
    auto path3 = graph.findShortestPath(1, 999);
    TEST_ASSERT(!path3.has_value(), "Path to non-existent destination should not exist");
    for (int i = 1; i <= 10; i++) graph.addVertex(i);
    for (int i = 1; i < 5; i++) graph.addEdge(i, i + 1);
    graph.addEdge(5, 10);
    graph.addEdge(1, 10);
    auto path4 = graph.findShortestPath(1, 10);
    TEST_ASSERT(path4.has_value(), "Shortest path should exist");
    TEST_ASSERT(path4->size() == 2, "Should find shortest path (direct edge 1->10)");
    TEST_ASSERT(vectorEquals(*path4, {1, 10}), "Path should be {1, 10}");
    graph.addVertex(20);
    graph.addVertex(21);
    graph.addEdge(20, 21);
    auto path5 = graph.findShortestPath(1, 20);
    TEST_ASSERT(!path5.has_value(), "Path to disconnected component should not exist");
    return 1;
}

int test_pageRankCornerCases() {
    SocialGraph graph1;
    auto ranks1 = graph1.calculatePageRank();
    TEST_ASSERT(ranks1.empty(), "PageRank of empty graph should be empty");
    SocialGraph graph2;
    graph2.addVertex(1);
    auto ranks2 = graph2.calculatePageRank();
    TEST_ASSERT(ranks2.size() == 1, "Single node should have rank");
    TEST_ASSERT(ranks2[1] > 0.99 && ranks2[1] < 1.01, "Single node should have rank ~1");
    SocialGraph graph3;
    graph3.addVertex(1);
    graph3.addVertex(2);
    graph3.addVertex(3);
    auto ranks3 = graph3.calculatePageRank();
    TEST_ASSERT(ranks3.size() == 3, "All nodes should have ranks");
    TEST_ASSERT(std::abs(ranks3[1] - ranks3[2]) < 0.01, "Disconnected nodes should have equal rank");
    TEST_ASSERT(std::abs(ranks3[2] - ranks3[3]) < 0.01, "Disconnected nodes should have equal rank");
    SocialGraph graph4;
    for (int i = 1; i <= 10; i++) graph4.addVertex(i);
    for (int i = 2; i <= 10; i++) {
        graph4.addEdge(i, 1);
    }
    auto ranks4 = graph4.calculatePageRank(0.85, 20);
    for (int i = 2; i <= 10; i++) {
        TEST_ASSERT(ranks4[1] > ranks4[i], "Hub node should have higher rank");
    }
    auto ranks5a = graph4.calculatePageRank(0.5, 10);
    auto ranks5b = graph4.calculatePageRank(0.99, 10);
    TEST_ASSERT(ranks5a.size() == ranks5b.size(), "Different damping factors should give same number of ranks");
    return 1;
}

int test_communitiesCornerCases() {
    SocialGraph graph1;
    auto comm1 = graph1.findCommunities();
    TEST_ASSERT(comm1.empty(), "Empty graph should have no communities");
    SocialGraph graph2;
    for (int i = 1; i <= 5; i++) graph2.addVertex(i);
    auto comm2 = graph2.findCommunities();
    TEST_ASSERT(comm2.size() == 5, "All isolated nodes should form separate communities");
    SocialGraph graph3;
    for (int i = 1; i <= 10; i++) graph3.addVertex(i);
    for (int i = 1; i < 10; i++) {
        graph3.addEdge(i, i + 1);
    }
    graph3.addEdge(10, 1);
    auto comm3 = graph3.findCommunities();
    TEST_ASSERT(comm3.size() == 1, "Cycle should form one community");
    TEST_ASSERT(comm3[0].size() == 10, "Community should have all nodes");
    SocialGraph graph4;
    for (int i = 1; i <= 10; i++) graph4.addVertex(i);
    for (int i = 1; i < 10; i++) {
        graph4.addEdge(i, i + 1);
    }
    auto comm4 = graph4.findCommunities();
    TEST_ASSERT(comm4.size() == 10, "Chain without cycle should have 10 separate communities (SCCs)");
    SocialGraph graph5;
    for (int i = 1; i <= 20; i++) graph5.addVertex(i);
    for (int c = 0; c < 5; c++) {
        int base = c * 4 + 1;
        graph5.addEdge(base, base + 1);
        graph5.addEdge(base + 1, base + 2);
        graph5.addEdge(base + 2, base + 3);
        graph5.addEdge(base + 3, base);
    }
    auto comm5 = graph5.findCommunities();
    TEST_ASSERT(comm5.size() == 5, "Should find 5 separate communities");
    return 1;
}

int test_echoChamberCornerCases() {
    SocialGraph graph1;
    for (int i = 1; i <= 5; i++) graph1.addVertex(i);
    for (int i = 1; i < 5; i++) graph1.addEdge(i, i + 1);
    auto chambers1 = graph1.findEchoChambers();
    TEST_ASSERT(chambers1.empty(), "No mutual follows should give no echo chambers");
    SocialGraph graph2;
    for (int i = 1; i <= 10; i++) graph2.addVertex(i);
    for (int i = 1; i <= 10; i++) {
        for (int j = i + 1; j <= 10; j++) {
            graph2.addEdge(i, j);
            graph2.addEdge(j, i);
        }
    }
    auto chambers2 = graph2.findEchoChambers();
    TEST_ASSERT(chambers2.size() == 45, "Complete mutual graph should have 45 pairs (10 choose 2)");
    SocialGraph graph3;
    graph3.addVertex(1);
    graph3.addEdge(1, 1);
    auto chambers3 = graph3.findEchoChambers();
    TEST_ASSERT(chambers3.empty(), "Self loops should not count as echo chambers");
    SocialGraph graph4;
    for (int i = 1; i <= 10; i++) graph4.addVertex(i);
    for (int i = 1; i < 10; i++) {
        graph4.addEdge(i, i + 1);
        graph4.addEdge(i + 1, i);
    }
    auto chambers4 = graph4.findEchoChambers();
    TEST_ASSERT(chambers4.size() == 9, "Chain of mutual follows should have 9 pairs");
    return 1;
}

int test_largeScaleCommunities() {
    SocialGraph graph;
    const int NUM_COMMUNITIES = 10;
    const int NODES_PER_COMMUNITY = 500;
    for (int c = 0; c < NUM_COMMUNITIES; c++) {
        int base = c * NODES_PER_COMMUNITY;
        for (int i = 0; i < NODES_PER_COMMUNITY; i++) {
            graph.addVertex(base + i);
        }
        for (int i = 0; i < NODES_PER_COMMUNITY; i++) {
            graph.addEdge(base + i, base + ((i + 1) % NODES_PER_COMMUNITY));
        }
    }
    auto communities = graph.findCommunities();
    TEST_ASSERT(communities.size() == NUM_COMMUNITIES, "Should find 10 communities");
    for (const auto& comm : communities) {
        TEST_ASSERT(comm.size() == NODES_PER_COMMUNITY, "Each community should have 500 nodes");
    }
    return 1;
}

int test_stressTestAllAlgorithms() {
    SocialGraph graph;
    const int NUM_NODES = 2000;
    for (int i = 1; i <= NUM_NODES; i++) {
        graph.addVertex(i);
    }
    for (int i = 1; i < 500; i++) {
        graph.addEdge(i, i + 1);
    }
    for (int i = 500; i < 1000; i++) {
        graph.addEdge(i, 500);
    }
    for (int i = 1000; i < 1500; i++) {
        graph.addEdge(i, 1000 + ((i + 1) % 500));
    }
    for (int i = 1500; i < 1600; i++) {
        for (int j = i + 1; j < 1600; j++) {
            graph.addEdge(i, j);
            graph.addEdge(j, i);
        }
    }
    auto path = graph.findShortestPath(1, 499);
    TEST_ASSERT(path.has_value(), "BFS should find path");
    auto ranks = graph.calculatePageRank(0.85, 10);
    TEST_ASSERT(ranks.size() == NUM_NODES, "PageRank should calculate for all nodes");
    auto communities = graph.findCommunities();
    TEST_ASSERT(!communities.empty(), "Should find communities");
    auto chambers = graph.findEchoChambers();
    TEST_ASSERT(!chambers.empty(), "Should find echo chambers");
    return 1;
}

int test_bfsWithCycle()
{
    SocialGraph graph;
    for (int i = 1; i <= 3; i++) graph.addVertex(i);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(1, 3);
    auto path = graph.findShortestPath(1, 3);
    TEST_ASSERT(path.has_value(), "Path 1->3 should exist");
    TEST_ASSERT(path->size() == 2, "Path should be shortest direct path");
    TEST_ASSERT(vectorEquals(*path, {1, 3}), "Path should be {1, 3}, not {1, 2, 3}");
    return 1;
}

int test_pageRank_sinkNode()
{
    SocialGraph graph;
    for (int i = 1; i <= 3; i++) graph.addVertex(i);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    auto ranks = graph.calculatePageRank(0.85, 10);
    TEST_ASSERT(ranks.size() == 3, "All 3 nodes should have ranks");
    double sum = ranks[1] + ranks[2] + ranks[3];
    TEST_ASSERT(sum > 0.99 && sum < 1.01, "Sum of ranks should be ~1.0 even with sink");
    TEST_ASSERT(ranks[3] > ranks[1], "Sink node 3 should get rank from node 2");
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
    // Basic Operations (18 points)
    {test_constructor, "constructor", 2, true},
    {test_addVertex, "addVertex", 2, true},
    {test_removeVertex, "removeVertex", 2, true},
    {test_addEdge, "addEdge", 2, true},
    {test_noDuplicateEdges, "noDuplicateEdges", 2, true},
    {test_removeEdge, "removeEdge", 2, true},
    {test_edgeWeights, "edgeWeights (unweighted)", 2, true},
    {test_thoroughRemovalChecks, "thoroughRemovalChecks", 2, true},
    {test_duplicateOperations, "duplicateOperations", 2, true},

    // Algorithms (48 points)
    {test_findShortestPath, "findShortestPath (BFS)", 8, true},
    {test_bfsCornerCases, "bfsCornerCases", 2, true},
    {test_bfsWithCycle, "bfsWithCycle", 3, true}, // NEW

    {test_findEchoChambers, "findEchoChambers", 8, true},
    {test_echoChamberCornerCases, "echoChamberCornerCases", 2, true},

    {test_calculatePageRank, "calculatePageRank", 8, true},
    {test_pageRankCornerCases, "pageRankCornerCases", 2, true},
    {test_pageRank_sinkNode, "pageRank_sinkNode", 3, true}, // NEW

    {test_findCommunities, "findCommunities (Kosaraju)", 10, true},
    {test_communitiesCornerCases, "communitiesCornerCases", 2, true},

    // Semantics & Edge Cases (4 points)
    {test_moveSemantics, "moveSemantics", 2, true},
    {test_edgeCases, "edgeCases", 2, true},

    // Large Scale (30 points)
    {test_largeGraphOperations, "largeGraphOperations (BFS)", 10, true},
    {test_largeScaleCommunities, "largeScaleCommunities", 10, true},
    {test_stressTestAllAlgorithms, "stressTestAllAlgorithms", 10, true},
};
//TOTAL: 100 points

int main() {
    std::cout << HEADER << "========================================" << RESET << std::endl;
    std::cout << HEADER << "   SocialGraph Testing Suite" << RESET << std::endl;
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

    // Print the final summary, weighting to 30 marks
    results.print_summary(30.0);
    
    std::cout << "\n" << TIME << "Test completed in: " << duration.count() << "ms" << RESET << std::endl;

    // NEW LINE:
    return (int)results.points_earned;
}

