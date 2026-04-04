#include "../linked_list.h"
#include <stdio.h>

int main(void) {
    // Initialize the pool for nodes
    initLinkedListPool();

    // Create an empty list
    List* list = createEmptyList();

    // Create a few nodes with simple integer items
    int a = 10, b = 20, c = 30, d = 40;

    Node* nodeA = createNode(&a);
    Node* nodeB = createNode(&b);
    Node* nodeC = createNode(&c);
    Node* nodeD = createNode(&d);

    // Insert nodes
    preInsert(nodeA, list);   // insert at head
    postInsert(nodeB, list);  // insert at tail
    postInsert(nodeC, list);  // insert at tail
    orderInsert(nodeD, list, 1); // insert at index 1

    // Get count
    printf("List count=%zu\n", getNodesCount(list));

    printf("List after inserts:\n");
    printList(list);

    // Find node at index 2
    Node* found = findNode(list, 1);
    if (found) {
        printf("Found node at index 1 with item=%d\n", *(int*)found->item);
    }

    // Delete node by index
    deleteNodeByIdx(list, 1);
    printf("List after deleting index 1:\n");
    printList(list);

    // Delete node by pointer
    deleteNode(list, nodeC);
    printf("List after deleting nodeC:\n");
    printList(list);

    // Get count
    printf("List count=%zu\n", getNodesCount(list));

    // Clear list
    deleteList(list);
    printf("List after deleteList:\n");
    // Get count
    printf("List count=%zu\n", getNodesCount(list));

    printList(list);

    return 0;
}
