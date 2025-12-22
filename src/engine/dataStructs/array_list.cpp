#include "../array_list.h"
#include <stdlib.h>
#include "../../helpers.h"

Array_List *array_list_create(size_t item_size, size_t init_capacity)
    {
	Array_List* list =  (Array_List*) calloc(1,sizeof(Array_List));
	
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
    size_t array_list_append(Array_List *list, void *item)
    {
	if(list->len == list->capacity)
	{
	    list->capacity = list->capacity > 0 ? list->capacity*2:1;
	    void* items = realloc(list->items, list->item_size*list->capacity);
	    if(!items)
		ERROR_RETURN(-1, "could not reallocate memory for Array_list\n");
	    list->items = items;
	}
	size_t index = list->len++;
	memcpy((uint8*)list->items + index * list->item_size, item ,list->item_size);
	return index;
    }
    void *array_list_get(Array_List *list, size_t index)
    {
	if(index>=list->len)
	    ERROR_RETURN(NULL, "Index out of bound \n");
	return (uint8*)list->items + index * list->item_size;
    }

//out of order remove
//copy the last item into the spot of the item to be removed then
// reduce the index by one
    uint8 array_list_remove(Array_List *list, size_t index)
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
