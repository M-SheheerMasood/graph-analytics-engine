#ifndef SOCIALMEDIA_SYSTEM_H
#define SOCIALMEDIA_SYSTEM_H

#include <iostream>
#include <vector>
#include "Heap.h"
using namespace std;

// Social media system that manages user feed using a single min-heap.
class SocialMediaSystem {
private:
    MinHeap feed;   // Min-heap storing post like counts

public:
    SocialMediaSystem();                                   // Constructor

    // Finds the array index of a post given its like count
    int findPostIndex(int value);

    // Builds a feed from a list of new post like counts
    void buildUndiscoveredFeed(vector<int> likes);

    // Updates a post's like count (increase in key value)
    void updatePostLikes(int oldLikes, int newLikes);

    // Removes all posts below a certain like threshold
    void clearLowEngagementPosts(int threshold);

    // Gets top N most liked posts
    vector<int> getTopNPosts(int N);

    // Decreases like count to simulate loss of engagement
    void decreasePostLikes(int currentLikes, int newLikes);
};

#endif
