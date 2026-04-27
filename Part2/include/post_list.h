#ifndef POST_LIST_H
#define POST_LIST_H

#include "post.h" // Contains the definition of Post

// Forward declaration of PostPool to be used in removePost
class PostPool; 

/**
 * @struct PostNode
 * @brief A node in the PostList, containing a pointer to a Post.
 */
struct PostNode {
    Post* post;
    PostNode* next;
    PostNode(Post* p) : post(p), next(nullptr) {}
};

/**
 * @class PostList
 * @brief A singly-linked list to manage posts created by a user.
 *
 * This class ONLY manages the lifecycle of the PostNode wrappers. It holds
 * non-owning pointers to Post objects, whose memory is managed by the PostPool.
 */
class PostList {
public:
    PostList();
    ~PostList();

    // Rule of Five to manage the list nodes correctly.
    PostList(const PostList& other);
    PostList& operator=(const PostList& other);
    PostList(PostList&& other) noexcept;
    PostList& operator=(PostList&& other) noexcept;

    /**
     * @brief Adds a post to the beginning of the list.
     * @param p A non-owning pointer to a Post object allocated from the PostPool.
     */
    void addPost(Post* p); 
    
    /**
     * @brief Removes a post from the list.
     * This method removes the wrapper node but does NOT free the Post object itself.
     * The caller is responsible for returning the Post pointer to the PostPool.
     * @param postID The ID of the post to remove.
     * @return A pointer to the removed Post object if found, otherwise nullptr.
     */
    Post* removePost(int postID); 

    /**
     * @brief Finds a post in the list by its ID.
     * @param postID The ID of the post to find.
     * @return A non-owning pointer to the Post if found, otherwise nullptr.
     */
    Post* findPost(int postID) const;

    /**
     * @brief Displays all posts in the list to the console. (For debugging)
     */
    void displayPosts() const;
    
    // Publicly accessible head for easier traversal in UserManager's export function
    PostNode* head;

private:
    /**
     * @brief Deletes all PostNode wrappers in the list.
     */
    void clear();
};

#endif // POST_LIST_H