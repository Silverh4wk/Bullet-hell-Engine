
#include "../array_list.h"

#include <stdlib.h>

#include "../../helpers.h"



struct Array_List *
arrayListCreate(size_t item_size, size_t init_capacity)
{
    struct Array_List* list =  (struct Array_List*) calloc(1,sizeof(struct Array_List));
    
    if(!list)
	ERROR_RETURN(NULL, "could not allocate memory for Array_list\n")

	    list->item_size = item_size;
    list->capacity = init_capacity;
    list->len =  0;
    list->items =  malloc(item_size * init_capacity);

    if(!list->items)
	ERROR_RETURN(NULL, "could not allocate memory for Array_list\n")

	    return list;
}

size_t
arrayListAppend(struct Array_List *list, void *item)
{
    //check if at capacity
    if(list->len == list->capacity)
    {
	//if capacity is 0, make it 1
	//if capacity isnt 0, multiply capacity by 2
	list->capacity = list->capacity > 0 ? list->capacity*2:1;

	//reallocate memory to accomadate the new item added
	void* items = realloc(list->items, list->item_size*list->capacity);
       
	if(!items)
	    ERROR_RETURN(-1, "could not reallocate memory for Array_list\n");

        // point the appeneded item to the last item slot in the list
        list->items = items;
    }
    // return the current lenght then increment it by 1
    size_t index = list->len++;

    //calculate the destination address as a byte pointer
    //add an offset of index * list->item_size bytes to reach the exact memory location of the new element's slot
    //then copies exactly list->item_size bytes from the memory pointed to by item into that slot
    memcpy((uint8*)list->items + index * list->item_size, item ,list->item_size);
    return index;
}


void *arrayListGet(struct Array_List *list, size_t index)
{
    if(index>=list->len)
	ERROR_RETURN(NULL, "Index out of bound \n");
    return (uint8*)list->items + index * list->item_size;
}

//out of order remove
//copy the last item into the spot of the item to be removed then
// reduce the index by one
uint8 arrayListRemove(struct Array_List *list, size_t index)
{
    if(list->len == 0)
	ERROR_RETURN(1, "List is already empty\n");
    if(index >=list->len )
	ERROR_RETURN(1, "Index out of bound\n");
    if(list->len == 1)
    {
	list->len = 0;
	return 0;
    }

    --list->len;

    uint8* item_ptr = (uint8*)list->items +index *list->item_size;
    uint8* end_ptr = (uint8*)list->items +list->len *list->item_size;
    memcpy(item_ptr,end_ptr,list->item_size);

    return 0;
}

void arrayListClear(struct Array_List *list)
{
    list->len =0;
}

void arrayListDestroy(struct Array_List *list)
{
    if(!list) return;
    free(list->items);  
    free(list);      
}
