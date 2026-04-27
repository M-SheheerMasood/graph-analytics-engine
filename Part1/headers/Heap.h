#ifndef __MIN_HEAP_H
#define __MIN_HEAP_H

#include <iostream>
#include <climits> 
#include <cstdlib> 
#include <utility> 

using namespace std;

class MinHeap
{
    int* harr;      // Pointer to the array of elements in the heap
    int capacity;  // Maximum possible size of the min heap
    int heap_size; // Current number of elements in the min heap

    // Swaps two integer values by pointer
    void swap(int* x, int* y)
    {
        int temp = *x;
        *x = *y;
        *y = temp;
    }

public:
    // Constructor
    MinHeap(int cap);

    // Destructor
    ~MinHeap();

    // Copy Constructor
    MinHeap(const MinHeap& other);

    // Copy Assignment Operator
    MinHeap& operator=(const MinHeap& other);

    // Returns the index of the parent of a node at index i
    int parent(int i) {return (i - 1) / 2;}

    // Returns the index of the left child of a node at index i
    int left(int i) {return 2 * i + 1;}

    // Returns the index of the right child of a node at index i
    int right(int i) {return 2 * i + 2;}

    // Removes and returns the minimum element (root).
    int extractMin();

    // Decreases the value of the key at index i to new_val (which is smaler than harr[i]).
    void decreaseKey(int i, int new_val);

    // Increases the value of the key at index i to new_val (which is bigger than harr[i]).
    void increaseKey(int i, int new_val);

    // Returns the minimum key (key at root) from the min heap.
    int getMin() const;

    // Deletes a key stored at a given index i.
    void deleteKey(int i);

    // Inserts a new key 'k' into the min heap.
    void insertKey(int k);

    // Maintains the min-heap property for a subtree rooted at i.
    void minHeapify(int i);

    int size() const { return heap_size; }

    int elementAt(int i) const { return harr[i]; }
};

#endif // __MIN_HEAP_H

