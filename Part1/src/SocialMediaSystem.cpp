#include "SocialMediaSystem.h"

SocialMediaSystem::SocialMediaSystem()
    : feed(10) // initial capacity
{
}

int SocialMediaSystem::findPostIndex(int value)
{
    if (value >= 0)
    {
        for (int i = 0; i < feed.size(); i++)
        {
            if (value == feed.elementAt(i))
            {
                return i;
            }
        }
    }
    return -1;
}

void SocialMediaSystem::buildUndiscoveredFeed(vector<int> likes)
{
    for (int i = 0; i < likes.size(); i++)
    {
        if (likes[i] >= 0)
        {
            feed.insertKey(likes[i]);
        }
    }
}

void SocialMediaSystem::updatePostLikes(int oldLikes, int newLikes)
{
    int i = findPostIndex(oldLikes);
    if (i >= 0)
    {
        feed.increaseKey(i, newLikes);
    }
}

void SocialMediaSystem::clearLowEngagementPosts(int threshold)
{
    if (threshold >= 0)
    {
        while (feed.getMin() < threshold && feed.size() > 0)
        {
            feed.extractMin();
        }
    }
}

vector<int> SocialMediaSystem::getTopNPosts(int N)
{
    vector<int> topItems;
    if (N > 0)
    {
        MinHeap copy = feed;
        int run = min(N, copy.size());
        for (int i = 0; i < run; i++)
        {
            topItems.push_back(copy.extractMin());
        }
    }
    return topItems;
}

void SocialMediaSystem::decreasePostLikes(int currentLikes, int newLikes)
{
    int i = findPostIndex(currentLikes);
    if (i >= 0)
    {
        feed.decreaseKey(i, newLikes);
    }
}
