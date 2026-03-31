#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stddef.h>

struct Block {
    void *memory;
    struct Block *next;
};


struct Chunk {
    /**
     * if free, next contains the address of the next 
     * chunk in the list.
     */
    struct  Chunk *next;
};


struct PoolAllocator {
    size_t chunks_per_block; //number of chunks per block
    struct Chunk *alloc;   //allocator pointer
    struct Block *blocks; //allocated blocks
};

//Initiate the objectPool construct    
void
initPool(struct PoolAllocator *pool,size_t chunks);

//destroy the pool and return the chunks mem back
void
destroyPool(struct PoolAllocator *pool);

// Returns the first free chunk in the block.
// if none exists, allocate a new one
void *
allocatePool(struct PoolAllocator *pool, size_t size);

//puts the chunk at the front of the chunk list
void
deallocatePool(struct PoolAllocator *pool, void *chunk);

//allocate a new block
//return a chunk pointer for the beginning of the block
struct Chunk *
allocateBlock(struct PoolAllocator *pool,size_t chunkSize);


#endif //object_pool.h
