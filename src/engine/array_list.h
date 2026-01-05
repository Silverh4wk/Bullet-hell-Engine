#pragma once
#include "../types.h"

typedef struct Array_List
{
    size_t len;
    size_t capacity;
    size_t item_size;
    void * items;
    
}
    Array_List;

Array_List* arrayListCreate(size_t item_size, size_t init_capacity);
size_t arrayListAppend(Array_List* list, void* item);
void* arrayListGet(Array_List* list, size_t index);
uint8 arrayListRemove(Array_List* list, size_t index);
void arrayListDestroy(Array_List* list);
