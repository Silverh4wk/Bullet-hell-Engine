#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stddef.h>

typedef struct node {
    void * item; // data of any type,
    //all elements in the node must share the same type of data
    struct node *prev; //pointer to the previous element
    struct node *next; // pointer to next element
}Node;

typedef struct List {
    Node *head; //pointer to the beggining of the list
    Node *tail; //pointer to the end of the list
    size_t count; //number of nodes in the list
} List;

void 
initLinkedListPool(void); //initiate the object pool for linked lists

Node *
createNode(void* item); /*creates a new, independant node  */

List *
createEmptyList(void); /*creates a new empty linked list */

List *
createList(Node* node); /*creates a new linked list using an item */

Node *
preInsert(Node* node, List* list); /* inserts a new item at the top of the list */

Node *
postInsert(Node* node, List* list); /* inserts a new item at the end of the list */

Node *
orderInsert(Node * node,List * list,size_t idx); /* inserts a new element in order, according to a key field */

Node *
findNode(List* list, size_t idx); /* find a node in the list */

Node *
deleteNodeByIdx(List* list, size_t idx); /* deletes a node corresponding to the inserted key */

void
deleteNode(List * list,Node* node); /* deletes a node */

void
deleteList(List* list); /* deletes a list */

void printList(List* list); /* prints all the nodes in the list */

//void MergeSort(List* list); /* sorting algorithm (TODO)*/

//List *Merge(List * fist, List * second); /* merges two sorted linked lists (TODO)*/

size_t
getNodesCount(List * list); /* returns the number of nodes in the list */

#endif
