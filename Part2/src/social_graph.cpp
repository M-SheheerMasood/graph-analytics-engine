#include "../include/social_graph.h"
#include <queue>
#include <algorithm>
#include <utility>
#include <vector>
#include <stack>

SocialGraph::SocialGraph() = default;

SocialGraph::SocialGraph(SocialGraph &&other) noexcept
    : adjList(std::move(other.adjList))
{
}

SocialGraph &SocialGraph::operator=(SocialGraph &&other) noexcept
{
    // TODO: Impliment the Move assignment operator
    if (this != &other)
    {
        adjList = std::move(other.adjList);
    }
    return *this;
}

// --- Vertex Management ---

void SocialGraph::addVertex(int node)
{
    // TODO
    if (adjList.find(node) == adjList.end())
    {
        adjList[node] = std::vector<int>();
    }
}

void SocialGraph::removeVertex(int node)
{
    // TODO
    int erased = adjList.erase(node);
    if (erased > 0)
    {
        for (auto &i : adjList)
        {
            auto it = std::find(i.second.begin(), i.second.end(), node);
            if (it != i.second.end())
            {
                i.second.erase(it);
            }
        }
    }
}

// --- Edge Management ---

void SocialGraph::addEdge(int from, int to, int /*weight*/)
{
    // TODO
    auto it1 = adjList.find(from);
    auto it2 = adjList.find(to);
    if (it1 != adjList.end() && it2 != adjList.end())
    {
        if (std::find(adjList[from].begin(), adjList[from].end(), to) == adjList[from].end())
        {
            adjList[from].push_back(to);
        }
    }
}

void SocialGraph::removeEdge(int from, int to)
{
    // TODO
    auto it1 = adjList.find(from);
    auto it2 = adjList.find(to);
    if (it1 != adjList.end() && it2 != adjList.end())
    {
        auto it3 = std::find(adjList[from].begin(), adjList[from].end(), to);
        if (it3 != adjList[from].end())
        {
            adjList[from].erase(it3);
        }
    }
}

// --- Graph Queries ---

bool SocialGraph::hasEdge(int from, int to) const
{
    // TODO
    auto it1 = adjList.find(from);
    if (it1 != adjList.end())
    {
        auto it2 = std::find(it1->second.cbegin(), it1->second.cend(), to);
        if (it2 != it1->second.cend())
        {
            return true;
        }
    }
    return false;
}

std::vector<int> SocialGraph::getAdjacent(int from) const
{
    // TODO
    auto it1 = adjList.find(from);
    if (it1 != adjList.end())
    {
        return it1->second;
    }
    return {};
}

std::optional<std::vector<int>> SocialGraph::findShortestPath(int start, int end) const
{
    // TODO
    std::vector<int> visited;
    std::queue<int> bfs;
    std::unordered_map<int, int> parent;
    auto it1 = adjList.find(start);
    auto it2 = adjList.find(end);
    int curr;
    if (it1 != adjList.end() && it2 != adjList.end())
    {
        visited.push_back(start);
        bfs.push(start);
        while (!bfs.empty())
        {
            curr = bfs.front();
            bfs.pop();
            if (curr == end)
            {
                std::vector<int> path;
                while (curr != start)
                {
                    path.push_back(curr);
                    curr = parent[curr];
                }
                path.push_back(start);
                std::reverse(path.begin(), path.end());
                return path;
            }
            auto it = adjList.find(curr);
            if (it != adjList.end())
            {
                for (int i = 0; i < it->second.size(); i++)
                {
                    if (std::find(visited.begin(), visited.end(), it->second[i]) == visited.end())
                    {
                        bfs.push(it->second[i]);
                        visited.push_back(it->second[i]);
                        parent[it->second[i]] = curr;
                    }
                }
            }
        }
    }
    return std::nullopt;
}

// --- Advanced Analysis Functions ---

std::vector<std::pair<int, int>> SocialGraph::findEchoChambers() const
{
    // TODO
    std::vector<std::pair<int, int>> pairs;
    for (auto &i : adjList)
    {
        for (int j = 0; j < i.second.size(); j++)
        {
            if (std::find(adjList.at(i.second[j]).begin(), adjList.at(i.second[j]).end(), i.first) != adjList.at(i.second[j]).end())
            {
                if (i.first < i.second[j])
                {
                    pairs.push_back({i.first, i.second[j]});
                }
            }
        }
    }
    return pairs;
}

std::unordered_map<int, double> SocialGraph::calculatePageRank(double damping, int iterations) const
{
    // TODO
    std::unordered_map<int, double> ranks;
    double N = double(adjList.size());
    for (auto &i : adjList)
    {
        ranks[i.first] = 1.0f / N;
    }
    for (int i = 0; i < iterations; i++)
    {
        std::unordered_map<int, double> newranks;
        double sink = 0.0f;
        for (auto &r : ranks)
        {
            if (adjList.at(r.first).empty())
            {
                sink += r.second;
            }
        }
        for (auto &j : adjList)
        {
            double sum = 0;
            for (auto &x : adjList)
            {
                auto it = std::find(x.second.begin(), x.second.end(), j.first);
                if (it != x.second.end())
                {
                    if (x.second.size() > 0)
                    {
                        sum += ranks[x.first] / x.second.size();
                    }
                }
            }
            newranks[j.first] = ((1.0f - damping) / N) + (damping * sum) + ((damping * sink) / N);
        }
        ranks = newranks;
    }
    return ranks;
}

std::vector<std::vector<int>> SocialGraph::findCommunities() const
{
    // TODO
    std::vector<std::vector<int>> comunities;
    std::unordered_map<int, bool> visited1;
    std::unordered_map<int, bool> visited2;
    std::vector<int> finish_stack;
    std::unordered_map<int, std::vector<int>> transposedadjList;
    std::vector<int> component;
    for (auto i : adjList)
    {
        if (!visited1[i.first])
        {
            kosaraju_dfs1(i.first, visited1, finish_stack);
        }
        for (auto j : i.second)
        {
            transposedadjList[j].push_back(i.first);
        }
    }
    while (finish_stack.size() > 0)
    {
        int u = finish_stack.back();
        finish_stack.pop_back();
        component.clear();
        if(!visited2[u])
        {
            kosaraju_dfs2(u, visited2, component, transposedadjList);
        }
        if (component.size() > 0)
        {
            comunities.push_back(component);
        }
    }
    return comunities;
}

void SocialGraph::kosaraju_dfs1(int u, std::unordered_map<int, bool> &visited, std::vector<int> &finish_stack) const
{
    // TODO
    visited[u] = true;
    if (adjList.find(u) != adjList.end())
    {
        for (auto i : adjList.at(u))
        {
            if (!visited[i])
            {
                kosaraju_dfs1(i, visited, finish_stack);
            }
        }
    }
    finish_stack.push_back(u);
}

void SocialGraph::kosaraju_dfs2(int u, std::unordered_map<int, bool> &visited, std::vector<int> &component, const std::unordered_map<int, std::vector<int>> &transposed_graph) const
{
    // TODO
    visited[u] = true;
    component.push_back(u);
    if (transposed_graph.find(u) != transposed_graph.end())
    {
        for (auto i : transposed_graph.at(u))
        {
            if (!visited[i])
            {
                kosaraju_dfs2(i, visited, component, transposed_graph);
            }
        }
    }
}