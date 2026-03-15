#include "../hash_table.h"


#include <assert.h>
#include "../../types.h"
#include <stdlib.h>
#include <string.h>

//(slot may be filled or empty).
typedef struct HashTableEntry{
    const char* key;  // key is NULL if this slot is empty
    void* value;
} HashTableEntry;


struct HashTable {
    HashTableEntry* entries;  // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
};

#define INITIAL_CAPACITY 16  // must not be zero

HashTable* hashTableCreate(void) {
    // Allocate space for hash table struct.
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (table == NULL) {
        return NULL;
    }
    table->length = 0;
    table->capacity = INITIAL_CAPACITY;

    // Allocate (zero'd) space for entry buckets.
    table->entries = (HashTableEntry*) calloc(table->capacity, sizeof(HashTableEntry));
    if (table->entries == NULL) {
        free(table); // error, free table before we return!
        return NULL;
    }
    return table;
}

void hashTableDestroy(HashTable* table) {
    // First free allocated keys.
    for (size_t i = 0; i < table->capacity; i++) {
        free((void*)table->entries[i].key);
    }

    // Then free entries array and table itself.
    free(table->entries);
    free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hashKey(const char* key) {
    uint64 hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
	//XoRing the byte of the input  before multiplying it with the FNV prime value 
	 hash ^= (uint64)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* hashTableGet(HashTable* table, const char* key) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64 hash = hashKey(key);
    size_t index = (size_t)(hash & (uint64)(table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {
            // Found key, return value.
            return table->entries[index].value;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= table->capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }
    return NULL;
}

// Internal function to set an entry (without expanding table).
static const char* hashTableSetEntry(HashTableEntry* entries, size_t capacity,
        const char* key, void* value, size_t* plength) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hashKey(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            // Found key (it already exists), update value.
            entries[index].value = value;
            return entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;
    return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool hashTableExpand(HashTable* table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false;  // overflow (capacity would be too big)
    }
    HashTableEntry* new_entries = (HashTableEntry*)calloc(new_capacity, sizeof(HashTableEntry));
    if (new_entries == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        HashTableEntry entry = table->entries[i];
        if (entry.key != NULL) {
            hashTableSetEntry(new_entries, new_capacity, entry.key,
                         entry.value, NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

const char* hashTableSet(HashTable* table, const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }

    // If length will exceed half of current capacity, expand it.
    if (table->length >= table->capacity / 2) {
        if (!hashTableExpand(table)) {
            return NULL;
        }
    }

    // Set entry and update length.
    return hashTableSetEntry(table->entries, table->capacity, key, value,
                        &table->length);
}

size_t hashTableLength(HashTable* table) {
    return table->length;
}

HashTableIterator hashTableIterator(HashTable* table) {
    HashTableIterator it;
    it._table = table;
    it._index = 0;
    return it;
}

bool hashTableNext(HashTableIterator* it) {
    // Loop till we've hit end of entries array.
    HashTable* table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            HashTableEntry entry = table->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}

