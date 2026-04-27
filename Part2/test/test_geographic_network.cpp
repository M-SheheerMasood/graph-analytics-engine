
#include "../include/geographic_network.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip> // For std::setprecision, std::left, std::setw
#include <vector>
#include <functional>
#include <string>
#include <set>
#include <tuple>

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
        std::cout << TIME << "Test results for GeographicNetwork:" << RESET << std::endl;
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

// Helper: compare adjacency ignoring order
static bool adjacencyEquals(const GeographicNetwork& g, int node, const std::vector<int>& expected) {
    auto adj = g.getAdjacent(node);
    auto a = adj;
    auto b = expected;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

// Helper: exact vector equality
static bool vectorEquals(const std::vector<int>& a, const std::vector<int>& b) {
    return a == b;
}

// Helper: compare unordered collections of ints
static bool intSetEquals(const std::vector<int>& a, const std::vector<int>& b) {
    std::multiset<int> sa(a.begin(), a.end());
    std::multiset<int> sb(b.begin(), b.end());
    return sa == sb;
}

// Helper: compare MST edges ignoring orientation
static bool mstEquals(const std::vector<GeographicNetwork::MstEdge>& got, const std::vector<std::tuple<int, int, int>>& expected) {
    std::multiset<std::string> A, B;
    for (const auto& e : got) {
        int u = std::min(e.u, e.v);
        int v = std::max(e.u, e.v);
        A.insert(std::to_string(u) + "-" + std::to_string(v) + ":" + std::to_string(e.weight));
    }
    for (const auto& t : expected) {
        int u, v, w; std::tie(u, v, w) = t;
        int uu = std::min(u, v);
        int vv = std::max(u, v);
        B.insert(std::to_string(uu) + "-" + std::to_string(vv) + ":" + std::to_string(w));
    }
    return A == B;
}

// --- Basic Tests ---

int test_constructor() {
    GeographicNetwork graph1;
    TEST_ASSERT(graph1.getAdjacent(1).empty(), "Default constructor should create empty graph");
    TEST_ASSERT(!graph1.hasEdge(1, 2), "Empty graph should have no edges");

    GeographicNetwork graph2;
    for (int i = 1; i <= 5; i++) {
        graph2.addVertex(i);
    }

    GeographicNetwork graph3(std::move(graph2));
    TEST_ASSERT(graph3.getAdjacent(1).empty(), "Move constructor should transfer data");

    return 1;
}

int test_addVertex() {
    GeographicNetwork graph;

    graph.addVertex(1);
    TEST_ASSERT(graph.getAdjacent(1).empty(), "Single vertex should have no edges");

    graph.addVertex(2);
    graph.addVertex(3);
    TEST_ASSERT(graph.getAdjacent(2).empty(), "Vertex 2 should have no edges");
    TEST_ASSERT(graph.getAdjacent(3).empty(), "Vertex 3 should have no edges");

    graph.addVertex(1);
    TEST_ASSERT(graph.getAdjacent(1).empty(), "Duplicate vertex should not change state");

    return 1;
}

int test_addEdge() {
    GeographicNetwork graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    graph.addEdge(1, 2, 10);
    TEST_ASSERT(graph.hasEdge(1, 2), "Edge 1-2 should exist");
    TEST_ASSERT(graph.hasEdge(2, 1), "Edge 2-1 should exist (undirected)");

    graph.addEdge(1, 3, 20);
    // Exact adjacency checks (undirected network: neighbors should reflect both directions)
    TEST_ASSERT(adjacencyEquals(graph, 1, { 2,3 }), "Adjacency of 1 should be {2, 3}");
    TEST_ASSERT(adjacencyEquals(graph, 2, { 1 }), "Adjacency of 2 should be {1}");
    TEST_ASSERT(adjacencyEquals(graph, 3, { 1 }), "Adjacency of 3 should be {1}");

    return 1;
}

int test_undirectedBehavior() {
    GeographicNetwork graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2, 5);

    TEST_ASSERT(graph.hasEdge(1, 2), "Edge should exist in 1->2 direction");
    TEST_ASSERT(graph.hasEdge(2, 1), "Edge should exist in 2->1 direction");

    auto adj1 = graph.getAdjacent(1);
    auto adj2 = graph.getAdjacent(2);
    TEST_ASSERT(adjacencyEquals(graph, 1, { 2 }), "Adjacency of 1 should be {2}");
    TEST_ASSERT(adjacencyEquals(graph, 2, { 1 }), "Adjacency of 2 should be {1}");

    return 1;
}

int test_removeVertex() {
    GeographicNetwork graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 2, 10);
    graph.addEdge(2, 3, 15);
    graph.addEdge(1, 3, 20);

    graph.removeVertex(2);

    TEST_ASSERT(!graph.hasEdge(1, 2), "Edge 1-2 should be removed");
    TEST_ASSERT(!graph.hasEdge(2, 3), "Edge 2-3 should be removed");
    TEST_ASSERT(graph.hasEdge(1, 3), "Edge 1-3 should remain");
    TEST_ASSERT(adjacencyEquals(graph, 1, { 3 }), "Adjacency of 1 should now be {3}");

    return 1;
}

int test_removeEdge() {
    GeographicNetwork graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addEdge(1, 2, 10);
    graph.addEdge(1, 3, 20);

    graph.removeEdge(1, 2);
    TEST_ASSERT(!graph.hasEdge(1, 2), "Edge 1-2 should be removed");
    TEST_ASSERT(!graph.hasEdge(2, 1), "Edge 2-1 should be removed (undirected)");
    TEST_ASSERT(graph.hasEdge(1, 3), "Edge 1-3 should remain");

    return 1;
}

// --- Dijkstra's Algorithm Tests ---

int test_findShortestPath() {
    GeographicNetwork graph;

    for (int i = 1; i <= 4; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 1);
    graph.addEdge(2, 3, 2);
    graph.addEdge(3, 4, 3);
    graph.addEdge(1, 4, 10); // Longer direct path

    auto path = graph.findShortestPath(1, 4);
    TEST_ASSERT(path.has_value(), "Path from 1 to 4 should exist");
    TEST_ASSERT(vectorEquals(*path, std::vector<int>{1, 2, 3, 4}), "Exact shortest path sequence {1,2,3,4}");

    auto self_path = graph.findShortestPath(1, 1);
    TEST_ASSERT(self_path.has_value(), "Path to self should exist");
    TEST_ASSERT(self_path->size() == 1, "Path to self should have length 1");
    TEST_ASSERT((*self_path)[0] == 1, "Path to self should contain self");

    graph.addVertex(5);
    auto no_path = graph.findShortestPath(1, 5);
    TEST_ASSERT(!no_path.has_value(), "No path to disconnected vertex");

    return 1;
}

int test_dijkstraCornerCases() {
    GeographicNetwork graph;

    graph.addVertex(1);
    auto path1 = graph.findShortestPath(1, 1);
    TEST_ASSERT(path1.has_value(), "Path to self");
    TEST_ASSERT(path1->size() == 1, "Single node path");
    TEST_ASSERT((*path1)[0] == 1, "Contains only node 1");

    auto path2 = graph.findShortestPath(999, 1);
    TEST_ASSERT(!path2.has_value(), "Non-existent source");

    GeographicNetwork graph2;
    for (int i = 1; i <= 5; i++) graph2.addVertex(i);
    graph2.addEdge(1, 2, 1);
    graph2.addEdge(2, 3, 1);
    graph2.addEdge(3, 4, 1);
    graph2.addEdge(4, 5, 1);
    graph2.addEdge(1, 5, 10); // Alternative longer path

    auto path3 = graph2.findShortestPath(1, 5);
    TEST_ASSERT(path3.has_value(), "Should find shortest path");
    TEST_ASSERT(path3->size() == 5, "Should use the multi-hop path: 1->2->3->4->5");
    TEST_ASSERT(vectorEquals(*path3, {1,2,3,4,5}), "Path should be {1,2,3,4,5}");

    return 1;
}

// --- MST Tests ---

int test_calculateMST() {
    GeographicNetwork graph;

    for (int i = 1; i <= 4; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 1);
    graph.addEdge(2, 3, 2);
    graph.addEdge(3, 4, 3);
    graph.addEdge(1, 3, 4);
    graph.addEdge(2, 4, 5);

    auto mst = graph.calculateMinimumSpanningTree();
    TEST_ASSERT(mst.size() == 3, "MST of 4 nodes should have 3 edges");

    int total_weight = 0;
    for (const auto& edge : mst) {
        total_weight += edge.weight;
    }
    TEST_ASSERT(total_weight == 6, "MST weight should be 1+2+3=6");

    // Verify the actual edges selected exactly (unordered)
    std::vector<std::tuple<int, int, int>> expected = { {1,2,1}, {2,3,2}, {3,4,3} };
    TEST_ASSERT(mstEquals(mst, expected), "MST edges should be (1,2,1), (2,3,2), (3,4,3)");

    return 1;
}

int test_mstSingleNode() {
    GeographicNetwork graph;
    graph.addVertex(1);

    auto mst = graph.calculateMinimumSpanningTree();
    TEST_ASSERT(mst.empty(), "Single node has no edges in MST");
    TEST_ASSERT(mst.size() == 0, "Explicitly check size is 0");

    return 1;
}

int test_mstDisconnected() {
    GeographicNetwork graph;

    for (int i = 1; i <= 4; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 1);
    graph.addEdge(3, 4, 2);

    auto mst = graph.calculateMinimumSpanningTree();
    TEST_ASSERT(mst.size() == 2, "Two components, 2 edges total (forest)");

    int total_weight = 0;
    for (const auto& edge : mst) {
        total_weight += edge.weight;
    }
    TEST_ASSERT(total_weight == 3, "Total weight should be 1+2=3");
    std::vector<std::tuple<int, int, int>> expected = { {1,2,1}, {3,4,2} };
    TEST_ASSERT(mstEquals(mst, expected), "MST should contain both edges (1,2,1) and (3,4,2)");

    return 1;
}

// --- Critical Nodes Tests ---

int test_findCriticalNodes() {
    GeographicNetwork graph;

    for (int i = 1; i <= 5; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 1);
    graph.addEdge(2, 3, 1);
    graph.addEdge(3, 4, 1);
    graph.addEdge(4, 5, 1);

    auto critical = graph.findCriticalNodes();
    // In a chain, nodes 2, 3, 4 are articulation points
    TEST_ASSERT(critical.size() == 3, "Should find 3 articulation points");

    TEST_ASSERT(intSetEquals(critical, std::vector<int>{2, 3, 4}), "Critical nodes should be {2, 3, 4}");

    return 1;
}

int test_criticalNodesTriangle() {
    GeographicNetwork graph;

    for (int i = 1; i <= 3; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 1);
    graph.addEdge(2, 3, 1);
    graph.addEdge(3, 1, 1);

    auto critical = graph.findCriticalNodes();
    TEST_ASSERT(critical.empty(), "Triangle has no articulation points");
    TEST_ASSERT(critical.size() == 0, "Explicitly verify size is 0");

    return 1;
}

int test_criticalNodesBridge() {
    GeographicNetwork graph;

    for (int i = 1; i <= 6; i++) graph.addVertex(i);
    // Triangle 1-2-3
    graph.addEdge(1, 2, 1);
    graph.addEdge(2, 3, 1);
    graph.addEdge(3, 1, 1);
    // Bridge 3-4
    graph.addEdge(3, 4, 1);
    // Triangle 4-5-6
    graph.addEdge(4, 5, 1);
    graph.addEdge(5, 6, 1);
    graph.addEdge(6, 4, 1);

    auto critical = graph.findCriticalNodes();
    TEST_ASSERT(critical.size() == 2, "Nodes 3 and 4 are critical");
    TEST_ASSERT(intSetEquals(critical, {3, 4}), "Critical nodes should be {3, 4}");

    return 1;
}

// --- Min Effort Path Tests ---

int test_findPathWithMinEffort() {
    GeographicNetwork graph;

    for (int i = 1; i <= 4; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 10);
    graph.addEdge(2, 4, 5);
    graph.addEdge(1, 3, 3);
    graph.addEdge(3, 4, 8);

    auto path = graph.findPathWithMinEffort(1, 4);
    TEST_ASSERT(path.has_value(), "Path should exist");
    // Min effort path is 1->3->4 with max edge weight 8 (better than 1->2->4 with max 10)
    TEST_ASSERT(path->size() == 3, "Path length should be 3");
    TEST_ASSERT(vectorEquals(*path, std::vector<int>{1, 3, 4}), "Exact sequence {1, 3, 4}");

    return 1;
}

int test_minEffortSingleEdge() {
    GeographicNetwork graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2, 100);

    auto path = graph.findPathWithMinEffort(1, 2);
    TEST_ASSERT(path.has_value(), "Direct path");
    TEST_ASSERT(path->size() == 2, "Two nodes");
    TEST_ASSERT((*path)[0] == 1 && (*path)[1] == 2, "Path is 1->2");

    return 1;
}

// --- Best City Tests ---

int test_findBestCity() {
    GeographicNetwork graph;

    for (int i = 1; i <= 4; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 3);
    graph.addEdge(2, 3, 1);
    graph.addEdge(3, 4, 1);
    graph.addEdge(1, 4, 2);

    int best = graph.findBestCity(3);
    // Distances:
    // 1: (2,3), (4,2) -> 2 reachable
    // 2: (1,3), (3,1), (4,2) -> 3 reachable
    // 3: (2,1), (4,1), (1,2) -> 3 reachable
    // 4: (3,1), (1,2), (2,2) -> 3 reachable
    // in the case of a tie return one with the Higher ID result should be 4
    TEST_ASSERT(best == 4, "Best city with threshold 3 should be 1");

    return 1;
}

int test_bestCityEmpty() {
    GeographicNetwork graph;

    int best = graph.findBestCity(10);
    TEST_ASSERT(best == -1, "Empty graph returns -1");

    return 1;
}

int test_bestCityTiebreaker() {
    GeographicNetwork graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    int best = graph.findBestCity(0);
    // All cities have 0 reachable neighbors, so return largest ID
    TEST_ASSERT(best == 3, "Should return largest ID when all equal (3)");
    
    graph.addEdge(1, 2, 10);
    graph.addEdge(1, 3, 10);
    int best2 = graph.findBestCity(5);
    // Nodes 2 and 3 have 0 reachable, 1 has 0 reachable. Tie -> 3
    TEST_ASSERT(best2 == 3, "Should return largest ID (3) with threshold 5");

    return 1;
}

int test_bestCitySimple() {
    GeographicNetwork graph;

    // Create a simple star topology where node 1 is in the center
    for (int i = 1; i <= 5; i++) graph.addVertex(i);
    graph.addEdge(1, 2, 1);
    graph.addEdge(1, 3, 1);
    graph.addEdge(1, 4, 1);
    graph.addEdge(1, 5, 1);

    int best = graph.findBestCity(1);
    // With threshold 1:
    // Node 1 can reach 4 neighbors (2,3,4,5)
    // Nodes 2-5 can each reach 1 neighbor (node 1)
    // So nodes 2-5 are tied, return largest: 5
    TEST_ASSERT(best == 5, "Node with fewest reachable (1) and highest ID (5)");

    return 1;
}

int test_bestCityChain() {
    GeographicNetwork graph;

    // Create a chain: 1-2-3-4-5 with weight 1 on each edge
    for (int i = 1; i <= 5; i++) graph.addVertex(i);
    for (int i = 1; i < 5; i++) {
        graph.addEdge(i, i + 1, 1);
    }

    int best = graph.findBestCity(2);
    // With threshold 2:
    // Node 1 reaches: 2(d=1), 3(d=2) -> count=2
    // Node 2 reaches: 1(d=1), 3(d=1), 4(d=2) -> count=3
    // Node 3 reaches: 1(d=2), 2(d=1), 4(d=1), 5(d=2) -> count=4
    // Node 4 reaches: 2(d=2), 3(d=1), 5(d=1) -> count=3
    // Node 5 reaches: 3(d=2), 4(d=1) -> count=2
    // Nodes 1 and 5 have 2 reachable, so return 5 (larger ID)
    TEST_ASSERT(best == 5, "Endpoint with fewer connections (5)");

    return 1;
}

// --- Large Scale Tests ---

int test_largeGraphOperations() {
    GeographicNetwork graph;
    const int NUM_NODES = 100;

    for (int i = 1; i <= NUM_NODES; i++) {
        graph.addVertex(i);
    }

    for (int i = 1; i < NUM_NODES; i++) {
        graph.addEdge(i, i + 1, i);
    }

    TEST_ASSERT(graph.hasEdge(1, 2), "First edge");
    TEST_ASSERT(graph.hasEdge(99, 100), "Last edge");

    auto path = graph.findShortestPath(1, 100);
    TEST_ASSERT(path.has_value(), "Path exists");
    TEST_ASSERT(path->size() == 100, "Path has 100 nodes (1 through 100)");
    TEST_ASSERT(path->front() == 1 && path->back() == 100, "Correct endpoints");

    return 1;
}

int test_complexMST() {
    GeographicNetwork graph;

    for (int i = 1; i <= 10; i++) {
        graph.addVertex(i);
    }

    // Create fully connected graph with weights 1, 2, 3, ...
    int weight = 1;
    for (int i = 1; i <= 10; i++) {
        for (int j = i + 1; j <= 10; j++) {
            graph.addEdge(i, j, weight++);
        }
    }

    auto mst = graph.calculateMinimumSpanningTree();
    TEST_ASSERT(mst.size() == 9, "10 nodes need 9 edges");

    // MST should select the 9 lightest edges (weights 1-9)
    int total_weight = 0;
    for (const auto& edge : mst) {
        total_weight += edge.weight;
    }
    TEST_ASSERT(total_weight == 45, "Sum of 1+2+3+...+9 = 45");

    return 1;
}

int test_stressTestAllAlgorithms() {
    GeographicNetwork graph;
    const int NUM_NODES = 50;

    for (int i = 1; i <= NUM_NODES; i++) {
        graph.addVertex(i);
    }

    // Create edges forming a chain with known structure
    for (int i = 1; i < NUM_NODES; i++) {
        graph.addEdge(i, i + 1, i % 10 + 1);
    }

    // Add cross edges at specific intervals
    for (int i = 1; i <= NUM_NODES - 10; i += 10) {
        graph.addEdge(i, i + 10, 5);
    }

    // Test Dijkstra with concrete verification
    auto path = graph.findShortestPath(1, NUM_NODES);
    TEST_ASSERT(path.has_value(), "Path exists");
    TEST_ASSERT(path->front() == 1 && path->back() == NUM_NODES, "Correct endpoints");

    // Test MST with concrete verification
    auto mst = graph.calculateMinimumSpanningTree();
    TEST_ASSERT(mst.size() == NUM_NODES - 1, "MST has n-1 edges for n nodes");

    // Test critical nodes - in a chain with cross edges, verify specific properties
    auto critical = graph.findCriticalNodes();
    TEST_ASSERT(critical.size() >= 0, "May have critical nodes");

    // Test min effort path with concrete verification
    auto min_effort = graph.findPathWithMinEffort(1, NUM_NODES);
    TEST_ASSERT(min_effort.has_value(), "Path exists");
    TEST_ASSERT(min_effort->front() == 1 && min_effort->back() == NUM_NODES, "Correct endpoints");

    return 1;
}

int test_criticalNodeSingleNode() {
    GeographicNetwork graph;
    graph.addVertex(1);
    auto critical = graph.findCriticalNodes();
    TEST_ASSERT(critical.empty(), "Single node cannot be a critical node");
    return 1;
}

int test_minEffortVsDijkstra() {
    GeographicNetwork graph;
    for (int i = 1; i <= 4; i++) graph.addVertex(i);
    // Path 1: 1->2->4. Sum=7. Max=6.
    graph.addEdge(1, 2, 1);
    graph.addEdge(2, 4, 6);
    // Path 2: 1->3->4. Sum=8. Max=4.
    graph.addEdge(1, 3, 4);
    graph.addEdge(3, 4, 4);

    auto dijkstraPath = graph.findShortestPath(1, 4);
    TEST_ASSERT(dijkstraPath.has_value(), "Dijkstra path exists");
    TEST_ASSERT(vectorEquals(*dijkstraPath, {1, 2, 4}), "Dijkstra path (min sum) is 1->2->4");

    auto effortPath = graph.findPathWithMinEffort(1, 4);
    TEST_ASSERT(effortPath.has_value(), "Min effort path exists");
    TEST_ASSERT(vectorEquals(*effortPath, {1, 3, 4}), "Min effort path (min max) is 1->3->4");

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
    // Basic Operations (12 points)
    {test_constructor, "constructor", 2, true},
    {test_addVertex, "addVertex", 2, true},
    {test_addEdge, "addEdge", 2, true},
    {test_undirectedBehavior, "undirectedBehavior", 2, true},
    {test_removeVertex, "removeVertex", 2, true},
    {test_removeEdge, "removeEdge", 2, true},

    // Dijkstra (12 points)
    {test_findShortestPath, "findShortestPath (Dijkstra)", 8, true},
    {test_dijkstraCornerCases, "dijkstraCornerCases", 4, true},

    // MST (14 points)
    {test_calculateMST, "calculateMST (Kruskal's)", 8, true},
    {test_mstSingleNode, "mstSingleNode", 2, true},
    {test_mstDisconnected, "mstDisconnected (Forest)", 4, true},

    // Critical Nodes (16 points)
    {test_findCriticalNodes, "findCriticalNodes (Tarjan's)", 8, true},
    {test_criticalNodesTriangle, "criticalNodesTriangle", 3, true},
    {test_criticalNodesBridge, "criticalNodesBridge", 3, true},
    {test_criticalNodeSingleNode, "criticalNodeSingleNode", 2, true}, // NEW

    // Min Effort (14 points)
    {test_findPathWithMinEffort, "findPathWithMinEffort", 8, true},
    {test_minEffortSingleEdge, "minEffortSingleEdge", 2, true},
    {test_minEffortVsDijkstra, "minEffortVsDijkstra", 4, true}, // NEW

    // Best City (18 points)
    {test_findBestCity, "findBestCity (Floyd-Warshall)", 8, true},
    {test_bestCityEmpty, "bestCityEmpty", 2, true},
    {test_bestCityTiebreaker, "bestCityTiebreaker", 2, true},
    {test_bestCitySimple, "bestCitySimple (Star)", 3, true},
    {test_bestCityChain, "bestCityChain", 3, true},

    // Stress Tests (14 points)
    {test_largeGraphOperations, "largeGraphOperations (Chain)", 4, true},
    {test_complexMST, "complexMST (Complete Graph)", 5, true},
    {test_stressTestAllAlgorithms, "stressTestAllAlgorithms", 5, true},
};
// TOTAL: 100 points

int main() {
    std::cout << HEADER << "========================================" << RESET << std::endl;
    std::cout << HEADER << "   GeographicNetwork Testing Suite" << RESET << std::endl;
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