#include "../include/post_list.h"
#include <iostream>
#include <vector> // Used for deep copy logic
#include <utility> // for std::swap

PostList::PostList() : head(nullptr) {}

PostList::~PostList() {
    clear();
}

void PostList::clear() {
    PostNode* current = head;
    while (current) {
        PostNode* nextNode = current->next;
        // CRITICAL: We ONLY delete the Node wrapper. The Post it points to
        // is owned by the PostPool and is not touched here.
        delete current;
        current = nextNode;
    }
    head = nullptr;
}

// --- Rule of Five Implementation ---

// Copy Constructor: Creates a new list of nodes pointing to the SAME posts.
PostList::PostList(const PostList& other) : head(nullptr) {
    if (!other.head) return;

    // We do a deep copy of the list structure, but the pointers contained
    // within are shallow copies, as the PostPool is the single owner of the posts.
    std::vector<Post*> postsToCopy;
    PostNode* current = other.head;
    while (current) {
        postsToCopy.push_back(current->post);
        current = current->next;
    }

    // Insert posts in reverse order to maintain the original list order
    // (since addPost adds to the head).
    for (auto it = postsToCopy.rbegin(); it != postsToCopy.rend(); ++it) {
        addPost(*it);
    }
}

// Copy Assignment Operator (using copy-and-swap idiom)
PostList& PostList::operator=(const PostList& other) {
    if (this != &other) {
        PostList temp(other); // Use copy constructor
        std::swap(head, temp.head); // Swap our head with the temporary's
    }
    return *this;
} // `temp` is destroyed here, freeing our old memory

// Move Constructor
PostList::PostList(PostList&& other) noexcept : head(other.head) {
    // Steal the head pointer and leave the other object in a valid, empty state.
    other.head = nullptr;
}

// Move Assignment Operator
PostList& PostList::operator=(PostList&& other) noexcept {
    if (this != &other) {
        clear(); // Free our own resources first
        head = other.head;
        other.head = nullptr;
    }
    return *this;
}

// --- Method Implementations ---

// This method correctly accepts a pointer from the PostPool.
void PostList::addPost(Post* p) {
    if (!p) return;
    PostNode* newNode = new PostNode(p);
    newNode->next = head;
    head = newNode;
}

// This method returns the Post pointer for the caller (UserManager) to manage.
Post* PostList::removePost(int postID) {
    PostNode* current = head;
    PostNode* prev = nullptr;
    while (current) {
        if (current->post && current->post->postID == postID) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            Post* removedPost = current->post;
            delete current; // Delete the wrapper node ONLY.
            return removedPost; // Return the pointer to the caller.
        }
        prev = current;
        current = current->next;
    }
    return nullptr; // Post not found
}

Post* PostList::findPost(int postID) const {
    PostNode* current = head;
    while (current) {
        if (current->post && current->post->postID == postID) {
            return current->post;
        }
        current = current->next;
    }
    return nullptr;
}

void PostList::displayPosts() const {
    PostNode* current = head;
    std::cout << "  Posts: ";
    if (!current) {
        std::cout << "None." << std::endl;
        return;
    }
    while (current) {
        if (current->post) {
            std::cout << "[ID: " << current->post->postID 
                      << ", Cat: " << current->post->category << "]";
            if (current->next) std::cout << ", ";
        }
        current = current->next;
    }
    std::cout << std::endl;
}