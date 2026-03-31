#include "../object_pool.h"
#include <stdlib.h>


void
initPool(struct PoolAllocator *pool, size_t chunk_size, size_t chunks_per_block)
{
    pool->alloc = NULL;
    pool->blocks = NULL;
    pool->chunk_size = chunk_size; 
    pool->chunks_per_block = chunks_per_block ;
}


void *
allocatePool(struct PoolAllocator *pool) {
 
  // No chunks left in the current block, or no any block
  // exists yet. Allocate a new one, passing the chunk size:
 
 if (pool->alloc == NULL) {
    pool->alloc = allocateBlock(pool);
    if (pool->alloc == NULL) return NULL;
}
 
  // The return value is the current position of
  // the allocation pointer:
 
  struct Chunk *freeChunk = pool->alloc;
 
  // Advance (bump) the allocation pointer to the next chunk.
  //
  // When no chunks left, the `mAlloc` will be set to `nullptr`, and
  // this will cause allocation of a new block on the next request:
 
  pool->alloc = pool->alloc->next;
 
  return freeChunk;
}

void
deallocatePool(struct PoolAllocator* pool,void *chunk) {
    
    // The freed chunk's next pointer points to the
    // current allocation pointer:
    
    ((struct Chunk*)chunk)->next = pool->alloc;

// And the allocation pointer is now set
// to the returned (free) chunk:
    
    pool->alloc = (struct Chunk*)(chunk);
}


struct Chunk *
allocateBlock(struct PoolAllocator *pool)
{
    // make sure of proper alignment 
    size_t align = sizeof(void*);
    size_t actualSize = (pool->chunk_size + align - 1) & ~(align - 1);

    // check to see if chunk can hold the next pointer
    if (actualSize < sizeof(struct Chunk)) {
        actualSize = sizeof(struct Chunk);
    }

    size_t blockSize = pool->chunks_per_block * actualSize;

    void *memory = malloc(blockSize);
    if (!memory) return NULL;

    struct Block *newBlock = malloc(sizeof(struct Block));
    if (!newBlock) {
        free(memory);
        return NULL;
    }

    // track this allocated block
    newBlock->memory = memory;
    newBlock->next = pool->blocks;
    pool->blocks = newBlock;

    struct Chunk *block_begin = (struct Chunk*)memory;
    struct Chunk *chunk = block_begin;

    // link all chunks
    for (size_t i = 0; i < pool->chunks_per_block - 1; ++i) {
        chunk->next = (struct Chunk*)((char*)chunk + actualSize);
        chunk = chunk->next;
    }

    // last chunk ends the list
    chunk->next = NULL;

    return block_begin;
}



void
destroyPool(struct PoolAllocator *pool)
{
    struct Block *block = pool->blocks;
    while (block) {
        struct Block *next = block->next;
        free(block->memory); // free the whole block
        free(block);         // free the block tracker itself
        block = next;
    }
    pool->alloc = NULL;
    pool->blocks = NULL;
}
