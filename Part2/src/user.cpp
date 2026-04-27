#include "../include/user.h"
#include <utility> // for std::move

// Constructor updated for PA3 to include new location data.
User::User(int id, const std::string& name, const std::string& country, const std::string& city)
    : userID(id), 
      userName(name), 
      country(country), 
      city(city) {
    // The `posts` member is default-constructed.
}

// --- Rule of Five Implementation ---

// Copy Constructor
User::User(const User& other) 
    : userID(other.userID), 
      userName(other.userName), 
      country(other.country), 
      city(other.city), 
      posts(other.posts) { // Invokes PostList's copy constructor
}

// Copy Assignment Operator
User& User::operator=(const User& other) {
    if (this != &other) {
        userID = other.userID;
        userName = other.userName;
        country = other.country;
        city = other.city;
        posts = other.posts; // Invokes PostList's copy assignment
    }
    return *this;
}

// Move Constructor
User::User(User&& other) noexcept
    : userID(other.userID), 
      userName(std::move(other.userName)), 
      country(std::move(other.country)), 
      city(std::move(other.city)),
      posts(std::move(other.posts)) { // Invokes PostList's move constructor
}

// Move Assignment Operator
User& User::operator=(User&& other) noexcept {
    if (this != &other) {
        userID = other.userID;
        userName = std::move(other.userName);
        country = std::move(other.country);
        city = std::move(other.city);
        posts = std::move(other.posts); // Invokes PostList's move assignment
    }
    return *this;
}

// --- Method Implementation ---

// This method now correctly forwards the Post pointer to the PostList.
// It no longer creates a Post or violates the PostPool's ownership.
void User::addPost(Post* post) {
    if (post) {
        posts.addPost(post);
    }
}