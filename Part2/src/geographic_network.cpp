#include "../include/geographic_network.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <utility>
#include <limits>
#include <functional>

GeographicNetwork::GeographicNetwork() = default;

GeographicNetwork::GeographicNetwork(GeographicNetwork &&other) noexcept : adjList(std::move(other.adjList)) {}
GeographicNetwork &GeographicNetwork::operator=(GeographicNetwork &&other) noexcept
{
    // TODO
    // Impliment the Move assignment operator
    if (this != &other)
    {
        adjList = std::move(other.adjList);
    }
    return *this;
}

// --- Vertex Management ---
void GeographicNetwork::addVertex(int node)
{
    // TODO
    if (adjList.find(node) == adjList.end())
    {
        adjList[node] = std::vector<Edge>();
    }
}

void GeographicNetwork::removeVertex(int node)
{
    // TODO
    auto it = adjList.find(node);
    if (it != adjList.end())
    {
        for (int i = 0; i < it->second.size(); i++)
        {
            for (int j = 0; j < adjList[it->second[i].to].size(); j++)
            {
                if (adjList[it->second[i].to][j].to == node)
                {
                    adjList[it->second[i].to].erase(adjList[it->second[i].to].begin() + j);
                    break;
                }
            }
        }
        adjList.erase(node);
    }
}

// --- Edge Management ---
void GeographicNetwork::addEdge(int from, int to, int weight)
{
    // TODO
    auto it1 = adjList.find(from);
    auto it2 = adjList.find(to);
    if (it1 != adjList.end() && it2 != adjList.end())
    {
        bool flag1 = false;
        for (auto i : adjList[from])
        {
            if (i.to == to)
            {
                flag1 = true;
                break;
            }
        }
        bool flag2 = false;
        for (auto i : adjList[to])
        {
            if (i.to == from)
            {
                flag2 = true;
                break;
            }
        }
        if (!flag1 && !flag2)
        {
            Edge e;
            e.to = to;
            e.weight = weight;
            adjList[from].push_back(e);
            e.to = from;
            adjList[to].push_back(e);
        }
    }
}

void GeographicNetwork::removeEdge(int from, int to)
{
    // TODO
    if (hasEdge(from, to))
    {
        int idx = -1;
        for (int i = 0; i < adjList[from].size(); i++)
        {
            if (adjList[from][i].to == to)
            {
                idx = i;
                break;
            }
        }
        adjList[from].erase(adjList[from].begin() + idx);
        idx = -1;
        for (int i = 0; i < adjList[to].size(); i++)
        {
            if (adjList[to][i].to == from)
            {
                idx = i;
                break;
            }
        }
        adjList[to].erase(adjList[to].begin() + idx);
    }
}

// --- Graph Queries ---
bool GeographicNetwork::hasEdge(int from, int to) const
{
    // TODO
    auto it1 = adjList.find(from);
    auto it2 = adjList.find(to);
    if (it1 != adjList.end() && it2 != adjList.end())
    {
        bool flag1 = false;
        for (auto i : adjList.at(from))
        {
            if (i.to == to)
            {
                flag1 = true;
                break;
            }
        }
        bool flag2 = false;
        for (auto i : adjList.at(to))
        {
            if (i.to == from)
            {
                flag2 = true;
                break;
            }
        }
        if (flag1 && flag2)
        {
            return true;
        }
    }
    return false;
}

std::vector<int> GeographicNetwork::getAdjacent(int from) const
{
    // TODO
    auto it = adjList.find(from);
    std::vector<int> ret;
    if (it != adjList.end())
    {
        for (auto i : adjList.at(from))
        {
            ret.push_back(i.to);
        }
    }
    return ret;
}

// --- Core Algorithm: Dijkstra's ---
std::optional<std::vector<int>> GeographicNetwork::findShortestPath(int start, int end) const
{
    // TODO
    auto it1 = adjList.find(start);
    auto it2 = adjList.find(end);
    if (it1 != adjList.end() && it2 != adjList.end())
    {
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        std::unordered_map<int, int> dist;
        std::unordered_map<int, int> parent;
        dist[start] = 0;
        for (auto &i : adjList)
        {
            if (i.first != start)
            {
                dist[i.first] = std::numeric_limits<int>::max();
            }
        }
        pq.push({0, start});
        while (!pq.empty())
        {
            int dis = pq.top().first;
            int id = pq.top().second;
            pq.pop();
            if (dis > dist[id])
            {
                continue;
            }
            if (id == end)
            {
                break;
            }
            for (auto i : adjList.at(id))
            {
                if ((dist[id] + i.weight) < dist[i.to])
                {
                    dist[i.to] = dist[id] + i.weight;
                    parent[i.to] = id;
                    pq.push({dist[i.to], i.to});
                }
            }
        }
        if (dist[end] < std::numeric_limits<int>::max())
        {
            std::vector<int> path;
            int curr = end;
            while (curr != start)
            {
                path.push_back(curr);
                curr = parent[curr];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }
    }
    return std::nullopt;
}

// --- Core Algorithm: Kruskal's for MST ---
std::vector<GeographicNetwork::MstEdge> GeographicNetwork::calculateMinimumSpanningTree() const
{
    // TODO
    std::vector<GeographicNetwork::MstEdge> mst;
    if (adjList.size() > 0)
    {
        struct CompareEdge
        {
            bool operator()(GeographicNetwork::MstEdge &a, GeographicNetwork::MstEdge &b)
            {
                return a.weight > b.weight;
            }
        };
        std::priority_queue<GeographicNetwork::MstEdge, std::vector<GeographicNetwork::MstEdge>, CompareEdge> pq;
        for (auto &i : adjList)
        {
            for (int j = 0; j < i.second.size(); j++)
            {
                if (i.first < i.second[j].to)
                {
                    pq.push({i.first, i.second[j].to, i.second[j].weight});
                }
            }
        }
        std::unordered_map<int, int> mstid;
        for (auto &i : adjList)
        {
            mstid[i.first] = i.first;
        }
        while (!pq.empty())
        {
            GeographicNetwork::MstEdge top = pq.top();
            pq.pop();
            int id1 = mstid[top.u];
            int id2 = mstid[top.v];
            if (id1 != id2)
            {
                mst.push_back({top.u, top.v, top.weight});
                for (auto &i : mstid)
                {
                    if (i.second == id2)
                    {
                        mstid[i.first] = id1;
                    }
                }
            }
        }
    }
    return mst;
}

// --- Implementations of Advanced Analysis Functions ---

std::vector<int> GeographicNetwork::findCriticalNodes() const
{
    // TODO
    std::vector<int> critical;
    std::unordered_map<int, int> parent;
    std::unordered_map<int, int> disc;
    std::unordered_map<int, int> low;
    int time = 0;
    for (auto &i : adjList)
    {
        parent[i.first] = -1;
    }
    for (auto &i : adjList)
    {
        if (disc.find(i.first) == disc.end())
        {
            findAP_dfs(i.first, time, parent, disc, low, critical);
        }
    }
    std::sort(critical.begin(), critical.end());
    auto last = std::unique(critical.begin(), critical.end());
    critical.erase(last, critical.end());
    return critical;
}

void GeographicNetwork::findAP_dfs(int u, int &time, std::unordered_map<int, int> &parent, std::unordered_map<int, int> &disc, std::unordered_map<int, int> &low, std::vector<int> &result) const
{
    // TODO
    // This is a helper for find critical nodes, you do not need to write this if you wont be following this certain implimentation
    int children = 0;
    time++;
    disc[u] = time;
    low[u] = time;
    for (auto i : adjList.at(u))
    {
        if (i.to == parent[u])
        {
            continue;
        }
        if (disc.find(i.to) != disc.end())
        {
            if (disc[i.to] < low[u])
            {
                low[u] = disc[i.to];
            }
        }
        else
        {
            children++;
            parent[i.to] = u;
            findAP_dfs(i.to, time, parent, disc, low, result);
            if (low[i.to] < low[u])
            {
                low[u] = low[i.to];
            }
            if (parent[u] != -1 && low[i.to] >= disc[u])
            {
                result.push_back(u);
            }
        }
    }
    if (parent[u] == -1 && children > 1)
    {
        result.push_back(u);
    }
}

std::optional<std::vector<int>> GeographicNetwork::findPathWithMinEffort(int start, int end) const
{
    // TODO
    auto it1 = adjList.find(start);
    auto it2 = adjList.find(end);
    if (it1 != adjList.end() && it2 != adjList.end())
    {
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        std::unordered_map<int, int> dist;
        std::unordered_map<int, int> parent;
        dist[start] = 0;
        for (auto &i : adjList)
        {
            if (i.first != start)
            {
                dist[i.first] = std::numeric_limits<int>::max();
            }
        }
        pq.push({0, start});
        while (!pq.empty())
        {
            int d = pq.top().first;
            int id = pq.top().second;
            pq.pop();
            if (d > dist[id])
            {
                continue;
            }
            if (id == end)
            {
                break;
            }
            for (auto i : adjList.at(id))
            {
                int nd = i.weight;
                if (d > i.weight)
                {
                    nd = d;
                }
                if (nd < dist[i.to])
                {
                    dist[i.to] = nd;
                    parent[i.to] = id;
                    pq.push({nd, i.to});
                }
            }
        }
        if (dist[end] < std::numeric_limits<int>::max())
        {
            std::vector<int> path;
            int curr = end;
            while (curr != start)
            {
                path.push_back(curr);
                curr = parent[curr];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }
    }
    return std::nullopt;
}
int GeographicNetwork::findBestCity(int distanceThreshold) const
{
    // TODO
    int bC = -1;
    int minR = std::numeric_limits<int>::max();
    for (auto &node : adjList)
    {
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        std::unordered_map<int, int> dist;
        dist[node.first] = 0;
        for (auto &i : adjList)
        {
            if(i.first != node.first)
            {
                dist[i.first] = std::numeric_limits<int>::max();
            }
        }
        pq.push({0, node.first});
        while (!pq.empty())
        {
            int d = pq.top().first;
            int id = pq.top().second;
            pq.pop();
            if (d > dist[id])
            {
                continue;
            }
            for (auto i : adjList.at(id))
            {
                if ((dist[id] + i.weight) < dist[i.to])
                {
                    dist[i.to] = dist[id] + i.weight;
                    pq.push({dist[i.to], i.to});
                }
            }
        }
        int count = 0;
        for (auto d : dist)
        {
            if (d.first != node.first && d.second <= distanceThreshold)
            {
                count++;
            }
        }
        if (count < minR)
        {
            minR = count;
            bC = node.first;
        }
        else if (count == minR)
        {
            if (node.first > bC)
            {
                bC = node.first;
            }
        }
    }
    return bC;
}