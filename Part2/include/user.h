#ifndef USER_H
#define USER_H

#include <string>
#include "post_list.h" // A User has-a PostList.

/**
 * @class User
 * @brief Represents a user in the social media platform.
 *
 * This class holds all data for a single user, including their profile information
 * and a list of posts they have created. For PA3, it no longer manages follow
 * relationships directly; that responsibility is now handled by the SocialGraph.
 */
class User {
public:
    // --- Public Data Members ---
    int userID;
    std::string userName;
    std::string country; // NEW for PA3: Required for the GeographicNetwork
    std::string city;    // NEW for PA3: Required for the GeographicNetwork
    PostList posts;      // Each user owns a list of their created posts.

    // --- Constructor & Rule of Five ---
    /**
     * @brief Constructs a new User.
     * @param id The unique ID for the user.
     * @param name The user's username.
     * @param country The user's country.
     * @param city The user's city.
     */
    User(int id, const std::string& name, const std::string& country, const std::string& city);
    ~User() = default; // The PostList destructor handles cleanup.

    // Move semantics are essential for efficiently managing Users in the main LinkedList.
    User(User&& other) noexcept;
    User& operator=(User&& other) noexcept;

    // A proper copy constructor and assignment operator are needed if users are copied.
    User(const User& other);
    User& operator=(const User& other);

    // --- Methods ---
    /**
     * @brief Adds a post to this user's post list.
     * @param post A non-owning pointer to a Post object, which must have been
     *             allocated from the system's PostPool.
     */
    void addPost(Post* post); 
};

#endif // USER_H