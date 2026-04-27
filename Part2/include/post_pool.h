#ifndef POST_POOL_H
#define POST_POOL_H

#include "post.h"
#include <vector>
#include <cstddef> // for size_t

/**
 * @class PostPool
 * @brief A custom memory manager for Post objects.
 *
 * This class pre-allocates memory in large blocks (arenas) to avoid the overhead
 * of frequent `new` and `delete` calls. It significantly improves performance
 * and reduces memory fragmentation by recycling freed Post objects.
 */
class PostPool {
public:
    /**
     * @brief Constructs a PostPool.
     * @param block_size The number of Post objects to allocate in each new memory block.
     */
    explicit PostPool(size_t block_size = 4096);

    /**
     * @brief Destructor that deallocates all memory blocks.
     */
    ~PostPool();

    // Delete copy operations to ensure the pool is not accidentally duplicated.
    PostPool(const PostPool&) = delete;
    PostPool& operator=(const PostPool&) = delete;

    /**
     * @brief Allocates a Post object from the pool.
     * Prioritizes reusing a previously freed object before allocating a new one.
     * @return A pointer to a clean, initialized Post object.
     */
    Post* allocPost();

    /**
     * @brief Returns a Post object to the pool for future reuse.
     * This does NOT deallocate memory; it adds the pointer to a free list.
     * @param p A pointer to the Post object to be freed.
     */
    void freePost(Post* p);

    // --- Analytics ---
    size_t totalAllocations() const;
    size_t reuseCount() const;

private:
    /**
     * @brief Releases all allocated memory blocks and resets the pool's state.
     */
    void purge();

    /**
     * @brief Allocates a new block of memory for Posts.
     */
    void allocateBlock();

    std::vector<Post*> blocks;       // Stores pointers to the start of each memory block.
    std::vector<Post*> free_list;    // Stores pointers to recycled Posts available for reuse.
    
    size_t block_size;               // How many Posts per block.
    size_t current_block_index;      // The next available index in the current (last) block.
    
    // Counters for performance analysis
    size_t alloc_count;              // How many blocks have been allocated.
    size_t reuse_count;              // How many times a Post was recycled.
};

#endif // POST_POOL_H