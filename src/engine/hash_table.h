// Simple hash table implemented in C.

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stddef.h>


typedef struct HashTable HashTable;


//hash table iterator
typedef struct HashTableIterator{
    const char* key;  // current key
    void* value;      // current value

    // Don't use these fields directly.
    HashTable* _table;       // reference to hash table being iterated
    size_t _index;    // current index into HashTableEntries
} HashTableIterator;

HashTable* hashTableCreate(void);


void hashTableDestroy(HashTable* table);


void* hashTableGet(HashTable* table, const char* key);

// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when ht_destroy is
// called). Return address of copied key, or NULL if out of memory.

const char* hashTableSet(HashTable* table, const char* key, void* value);


size_t hashTableLength(HashTable* table);

// Return new hash table iterator (for use with ht_next).
HashTableIterator hashTableIterator(HashTable* table);

// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call ht_set during iteration.
bool hashTableNext(HashTableIterator* it);


#endif
