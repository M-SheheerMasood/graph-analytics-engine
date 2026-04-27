#include "../include/interaction_graph.h"
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <queue>

InteractionGraph::InteractionGraph() = default;

InteractionGraph::InteractionGraph(InteractionGraph &&other) noexcept
    : userToPostEdges(std::move(other.userToPostEdges)),
      postToUserEdges(std::move(other.postToUserEdges)) {}

InteractionGraph &InteractionGraph::operator=(InteractionGraph &&other) noexcept
{
    if (this != &other)
    {
        userToPostEdges = std::move(other.userToPostEdges);
        postToUserEdges = std::move(other.postToUserEdges);
    }
    return *this;
}

// --- Graph Management ---

void InteractionGraph::addVertex(int id, NodeType type)
{
    // TODO
    if (type == NodeType::USER)
    {
        if (userToPostEdges.find(id) == userToPostEdges.end())
        {
            userToPostEdges[id] = std::vector<Interaction>();
        }
    }
    else if (type == NodeType::POST)
    {
        if (postToUserEdges.find(id) == postToUserEdges.end())
        {
            postToUserEdges[id] = std::vector<Interaction>();
        }
    }
}

void InteractionGraph::removeVertex(int id, NodeType type)
{
    // TODO: Implement this function. This is the most complex one.
    // If you remove a USER, you must also clean up their interactions from the postToUserEdges map.
    // If you remove a POST, you must do the reverse from the userToPostEdges map
    if (type == NodeType::USER)
    {
        int erased = userToPostEdges.erase(id);
        if (erased > 0)
        {
            for (auto &i : postToUserEdges)
            {
                auto it = i.second.begin();
                while (it != i.second.end())
                {
                    if (it->targetID == id)
                    {
                        it = i.second.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }
            }
        }
    }
    else if (type == NodeType::POST)
    {
        int erased = postToUserEdges.erase(id);
        if (erased > 0)
        {
            for (auto &i : userToPostEdges)
            {
                auto it = i.second.begin();
                while (it != i.second.end())
                {
                    if (it->targetID == id)
                    {
                        it = i.second.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }
            }
        }
    }
}

void InteractionGraph::addInteraction(int userID, int postID, int weight)
{
    // TODO: Implement this function.
    // Remember to update BOTH maps to keep them in sync.
    addVertex(userID, NodeType::USER);
    addVertex(postID, NodeType::POST);
    bool flag = false;
    auto it = userToPostEdges[userID].begin();
    while (it != userToPostEdges[userID].end())
    {
        if (it->targetID == postID)
        {
            flag = true;
            break;
        }
        it++;
    }
    if (!flag)
    {
        userToPostEdges[userID].push_back({postID, weight});
    }
    flag = false;
    auto it2 = postToUserEdges[postID].begin();
    while (it2 != postToUserEdges[postID].end())
    {
        if (it2->targetID == userID)
        {
            flag = true;
            break;
        }
        it2++;
    }
    if (!flag)
    {
        postToUserEdges[postID].push_back({userID, weight});
    }
}

// --- Advanced Analysis Functions ---

std::vector<std::pair<int, double>> InteractionGraph::findSimilarUsers(int userID, int topN) const
{
    // TODO: Implement this function to calculate Jaccard Similarity(issay direct hint kiya doon ).
    std::vector<std::pair<int, double>> sim;
    if (userToPostEdges.find(userID) != userToPostEdges.end())
    {
        std::unordered_set<int> userposts;
        for (auto i : userToPostEdges.at(userID))
        {
            userposts.insert(i.targetID);
        }
        for (auto &i : userToPostEdges)
        {
            if (i.first != userID)
            {
                std::unordered_set<int> copy = userposts;
                std::unordered_set<int> otherposts;
                for (auto j : userToPostEdges.at(i.first))
                {
                    otherposts.insert(j.targetID);
                    copy.insert(j.targetID);
                }
                double js = double(userposts.size() + otherposts.size() - copy.size()) / double(copy.size());
                if (js > 0)
                {
                    sim.push_back({i.first, js});
                }
            }
        }
        if (topN > sim.size())
        {
            topN = sim.size();
        }
        std::vector<std::pair<int, double>> topsim;
        for (int i = 0; i < topN; i++)
        {
            std::pair<int, double> maxVal = {0, -10000.0};
            int idx = -1;
            for (int j = 0; j < sim.size(); j++)
            {
                if (sim[j].second > maxVal.second)
                {
                    maxVal = sim[j];
                    idx = j;
                }
            }
            if (idx != -1)
            {
                sim.erase(sim.begin() + idx);
                topsim.push_back(maxVal);
            }
        }
        return topsim;
    }
    return sim;
}

std::vector<int> InteractionGraph::recommendPosts(int userID, int topN) const
{
    // This implementation uses the top 5 most similar users to generate recommendations.
    // TODO
    std::vector<int> ret;
    std::vector<std::pair<int, double>> sim = findSimilarUsers(userID, 5);
    if (sim.size() > 0)
    {
        std::unordered_set<int> seenposts;
        std::unordered_map<int, double> recommend;
        for (int i = 0; i < userToPostEdges.at(userID).size(); i++)
        {
            seenposts.insert(userToPostEdges.at(userID).at(i).targetID);
        }
        for (auto &i : sim)
        {
            for (int j = 0; j < userToPostEdges.at(i.first).size(); j++)
            {
                if (seenposts.find(userToPostEdges.at(i.first).at(j).targetID) == seenposts.end())
                {
                    recommend[userToPostEdges.at(i.first).at(j).targetID] += userToPostEdges.at(i.first).at(j).weight * i.second;
                }
            }
        }
        std::vector<std::pair<int, double>> temp;
        for (auto i : recommend)
        {
            temp.push_back({i.first, i.second});
        }
        if (temp.size() < topN)
        {
            topN = temp.size();
        }
        for (int i = 0; i < topN; i++)
        {
            std::pair<int, double> maxVal = {0, -10000.0};
            int idx = -1;
            for (int j = 0; j < temp.size(); j++)
            {
                if (temp[j].second > maxVal.second)
                {
                    maxVal = temp[j];
                    idx = j;
                }
            }
            if (idx != -1)
            {
                temp.erase(temp.begin() + idx);
                ret.push_back(maxVal.first);
            }
        }
    }
    return ret;
}

std::unordered_map<int, double> InteractionGraph::calculateTrendScores(const std::unordered_map<int, double> &pageRanks) const
{
    // TODO: Implement this function to calculate trend scores based on PageRank.
    std::unordered_map<int, double> scores;
    for (auto &i : postToUserEdges)
    {
        double is = 0;
        for (int j = 0; j < i.second.size(); j++)
        {
            if (pageRanks.find(i.second[j].targetID) != pageRanks.end())
            {
                is += pageRanks.at(i.second[j].targetID) * i.second[j].weight;
            }
        }
        scores[i.first] = is;
    }
    return scores;
}

std::optional<std::vector<int>> InteractionGraph::getProcessingOrder() const
{
    // TODO: Implement Kahn's algorithm for topological sorting(aik aur hint lelo).
    std::unordered_map<int, int> postindegree;
    std::unordered_map<int, int> userindegree;
    std::queue<std::pair<int, NodeType>> queue;
    std::vector<int> ret;
    for (auto &i : postToUserEdges)
    {
        postindegree[i.first] = i.second.size();
        if (i.second.size() == 0)
        {
            queue.push({i.first, NodeType::POST});
        }
    }
    for (auto &i : userToPostEdges)
    {
        userindegree[i.first] = 0;
        queue.push({i.first, NodeType::USER});
    }
    while (!queue.empty())
    {
        int id = queue.front().first;
        NodeType type = queue.front().second;
        queue.pop();
        ret.push_back(id);
        if(type == NodeType::USER)
        {
            for (int i = 0; i < userToPostEdges.at(id).size(); i++)
            {
                postindegree[userToPostEdges.at(id)[i].targetID]--;
                if(postindegree[userToPostEdges.at(id)[i].targetID] <= 0)
                {
                    queue.push({userToPostEdges.at(id)[i].targetID, NodeType::POST});
                }
            }
        }
    }
    if(ret.size() == (userToPostEdges.size() + postToUserEdges.size()))
    {
        return ret;
    }
    return std::nullopt;
}