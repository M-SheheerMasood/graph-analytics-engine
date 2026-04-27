#ifndef POST_H
#define POST_H

#include <string>

// A simple struct to hold all data related to a social media post.
// This is a plain old data (POD) type. It has no complex logic.
struct Post {
    int postID;
    std::string category;
    int views;
    std::string content; // Optional content field

    // Default constructor
    Post() : postID(0), views(0) {}

    // Parameterized constructor for convenience
    Post(int id, const std::string& cat, int v) 
        : postID(id), category(cat), views(v) {}
};

#endif // POST_H