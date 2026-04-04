#ifndef ARRAY_LIST_H
#define  ARRAY_LIST_H

#include "../types.h"


struct Array_List
{
    size_t len;
    size_t capacity;
    size_t item_size;
    void * items;
};

// allocates memory for the array
// return a pointer to the created array 
struct Array_List*
arrayListCreate(size_t item_size, size_t init_capacity);

//append an item to the list
size_t
arrayListAppend(struct Array_List* list, void* item);

//get an item from the list using its index
void*
arrayListGet(struct Array_List* list, size_t index);

//remove an item from the array 
uint8
arrayListRemove(struct Array_List* list, size_t index);

// the next append will start at 0
// ignoring the remaining elements in the list
// it will also overwrite the remaining data once it gets to that point 
void
arrayListClear(struct Array_List *list);

// destroy all items in the array, freeing their mem
// destroying the list itself and freeing its mem
void
arrayListDestroy(struct Array_List* list);


#endif
