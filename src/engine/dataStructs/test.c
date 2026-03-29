#include <stdio.h>
#include <stdlib.h>
#include "../linked_list.h"  // your header with Node/List definitions

// Assume Merge(List*, List*) is implemented as discussed

int main(void) {
    // Create first sorted list: 1 -> 3 -> 5
    List *list1 = createEmptyList();
    postInsert(createNode((int*)(intptr_t)1), list1);
    postInsert(createNode((int*)(intptr_t)3), list1);
    postInsert(createNode((int*)(intptr_t)5), list1);

    printf("List 1:\n");
    printList(list1);

    // Create second sorted list: 2 -> 4 -> 6
    List *list2 = createEmptyList();
    postInsert(createNode((void*)(intptr_t)2), list2);
    postInsert(createNode((void*)(intptr_t)4), list2);
    postInsert(createNode((void*)(intptr_t)6), list2);

    printf("List 2:\n");
    printList(list2);



    return 0;
}
