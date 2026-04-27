#include "../include/post_pool.h"

PostPool::PostPool(size_t block_size) 
    : block_size(block_size), current_block_index(0), alloc_count(0), reuse_count(0) {
    // Allocate the first block immediately upon creation.
    allocateBlock();
}

PostPool::~PostPool() {
    purge();
}

Post* PostPool::allocPost() {
    // Priority 1: Reuse a post from the free list if available.
    if (!free_list.empty()) {
        Post* reusedPost = free_list.back();
        free_list.pop_back();
        reuse_count++;
        
        // Reset the post to a clean, default state before handing it out.
        *reusedPost = Post(); 
        return reusedPost;
    }
    
    // Priority 2: Allocate from the current block.
    // If the current block is full, we need a new one.
    if (current_block_index >= block_size) {
        allocateBlock();
    }
    
    // Get the address of the next available Post in the current block.
    Post* newPost = &blocks.back()[current_block_index];
    current_block_index++;
    
    // Ensure the post is in a clean state.
    *newPost = Post();
    return newPost;
}

void PostPool::freePost(Post* p) {
    if (!p) return;
    
    // Add the pointer to the free list for future recycling.
    // The memory is not deallocated.
    free_list.push_back(p);
}

size_t PostPool::totalAllocations() const {
    return alloc_count;
}

size_t PostPool::reuseCount() const {
    return reuse_count;
}

void PostPool::purge() {
    // Deallocate each block of memory we've requested.
    for (Post* block : blocks) {
        delete[] block;
    }
    
    // Clear the vectors to reset the pool's state.
    blocks.clear();
    free_list.clear();
    
    // Reset counters.
    current_block_index = 0;
    alloc_count = 0;
    reuse_count = 0;
}

void PostPool::allocateBlock() {
    // Allocate a contiguous array of Post objects on the heap.
    Post* newBlock = new Post[block_size];
    blocks.push_back(newBlock);
    
    // Reset the index to the beginning of our new block.
    current_block_index = 0;
    alloc_count++;
}