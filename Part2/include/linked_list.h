#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstddef>     // For size_t
#include <functional>  // For std::function for the find method
#include <utility>     // For std::move

/**
 * @class LinkedList
 * @brief A generic, doubly-linked list template class.
 *
 * This class provides the core functionality for a doubly-linked list, capable
 * of storing any data type `T`. The entire implementation is in the header
 * to facilitate template instantiation by the compiler.
 */
template <typename T>
class LinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;

        // Use a template constructor to perfectly forward the value,
        // allowing for both copy and move construction of the data.
        template<typename... Args>
        Node(Args&&... args) : data(std::forward<Args>(args)...), prev(nullptr), next(nullptr) {}
    };

    // --- Constructor, Destructor, and Rule of Five ---
    LinkedList() : _head(nullptr), _tail(nullptr), _size(0) {}

    ~LinkedList() {
        clear();
    }
    
    // Move Constructor
    LinkedList(LinkedList&& other) noexcept 
        : _head(other._head), _tail(other._tail), _size(other._size) {
        other._head = other._tail = nullptr;
        other._size = 0;
    }

    // Move Assignment Operator
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            _head = other._head;
            _tail = other._tail;
            _size = other._size;
            other._head = other._tail = nullptr;
            other._size = 0;
        }
        return *this;
    }

    // Deleted copy operations to prevent accidental deep copies of complex lists.
    // The user of this class must be explicit if a copy is needed.
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    // --- Modifiers ---
    Node* push_back(const T& val) {
        Node* newNode = new Node(val);
        if (!_tail) {
            _head = _tail = newNode;
        } else {
            _tail->next = newNode;
            newNode->prev = _tail;
            _tail = newNode;
        }
        _size++;
        return newNode;
    }

    Node* push_front(const T& val) {
        Node* newNode = new Node(val);
        if (!_head) {
            _head = _tail = newNode;
        } else {
            _head->prev = newNode;
            newNode->next = _head;
            _head = newNode;
        }
        _size++;
        return newNode;
    }

    // A version of push_back that moves the value, avoiding a copy.
    Node* push_back(T&& val) {
        Node* newNode = new Node(std::move(val)); // Use move constructor for T
        if (!_tail) {
            _head = _tail = newNode;
        } else {
            _tail->next = newNode;
            newNode->prev = _tail;
            _tail = newNode;
        }
        _size++;
        return newNode;
    }

    void remove(Node* node) {
        if (!node) return;

        if (node->prev) {
            node->prev->next = node->next;
        } else { // Node is the head
            _head = node->next;
        }

        if (node->next) {
            node->next->prev = node->prev;
        } else { // Node is the tail
            _tail = node->prev;
        }

        delete node;
        _size--;
    }

    void clear() {
        Node* current = _head;
        while (current) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        _head = _tail = nullptr;
        _size = 0;
    }

    // --- Accessors and Finders ---
    Node* find(std::function<bool(const T&)> pred) const {
        Node* current = _head;
        while (current) {
            if (pred(current->data)) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    Node* head() const { return _head; }
    Node* tail() const { return _tail; }
    size_t size() const { return _size; }
    bool empty() const { return _size == 0; }

private:
    Node* _head;
    Node* _tail;
    size_t _size;
};

#endif // LINKED_LIST_H