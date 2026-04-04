#include "../linked_list.h"

#include "../pool_allocator.h"

#include <stdlib.h>
#include <stdio.h>


static struct PoolAllocator node_pool;


void initLinkedListPool(void)
    {
	initPool(&node_pool, sizeof(Node), 256);
    }

Node *createNode(void *item) {

    Node* new_node = allocatePool(&node_pool);

    new_node->item = item;
    new_node->prev = NULL;
    new_node ->next = NULL;
    
    return new_node;
};

List *createEmptyList(void)
{
    List* new_list = malloc(sizeof(List));

    new_list->head = NULL;
    new_list->tail = NULL;
    new_list->count= 0;
    
    return new_list;
};

List *
createList(Node* node)
{
    List* new_list = malloc(sizeof(List));

    new_list->head = node;
    new_list->tail = node;
    new_list->count= 1;
    
    return new_list;
};

Node *
preInsert(Node *node, List* list)
{
    if (list->head != NULL) {
        list->head->prev = node; 
        node->prev = NULL; //marks the top of the new list
        node->next = list->head; //push the prev head node
        list->head = node;
    } else {
        // empty list case
        node->prev = NULL;
        node->next = NULL;
        list->head = node;
        list->tail = node;
    }
    list->count++;
    return list->head;
}

Node *
postInsert(Node *node, List* list)
{
    if (list->tail != NULL) {
        list->tail->next = node; 
        node->next = NULL; //marks the tail of the new list
        node->prev = list->tail; //push the prev head node
        list->tail = node;
    } else {
        // empty list case
        node->prev = NULL;
        node->next = NULL;
        list->head = node;
        list->tail = node;
    }
    list->count++;
    return list->tail;
};

Node *
orderInsert(Node *node, List* list, size_t idx)
{
    if (!list || !node) return NULL;

    Node *ptr = list->head;
    size_t i = 0;

    // traverse the list until idx or end 
    while (ptr && i < idx) {
        ptr = ptr->next;
        i++;
    }
    
    if (!ptr)//the end of the list
    {
        // inserting at end
        node->prev = list->tail;
        node->next = NULL;
        if (list->tail) list->tail->next = node;
        list->tail = node;
        if (!list->head) list->head = node; // empty list case
    } else {
        // inserting before ptr
        node->prev = ptr->prev;
        node->next = ptr;
        if (ptr->prev) ptr->prev->next = node;
        else list->head = node; // new head
        ptr->prev = node;
    }
    
    list->count++;
    return node;
}

Node*
findNode(List *list, size_t idx)
{

    if (!list) return NULL;

    Node* ptr = list->head;
    size_t i  = 0;
    while(ptr && i < idx)
    {
	ptr = ptr->next;
	i++;
    }
    return ptr; //returns NULL if node doesnt exist
}

Node *
deleteNodeByIdx(List *list, size_t idx)
{
    Node* ptr = findNode(list, idx);
    if (!ptr) return NULL;

    // at the top of the list
    if (ptr->prev == NULL) {
        list->head = ptr->next;
        if (list->head) list->head->prev = NULL;
    }
    // at the end of the list
    else if (ptr->next == NULL) {
        list->tail = ptr->prev;
        if (list->tail) list->tail->next = NULL;
    }
    // in the middle
    else {
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
    }

     deallocatePool(&node_pool, ptr);
    list->count--;
    return list->head; //return a pointer to the top of the list 
}

void
deleteNode(List *list, Node *node)
{
    if (!list || !node) return;

    // If node is the head
    if (node == list->head) {
        list->head = node->next;
        if (list->head) list->head->prev = NULL;
    }

    // If node is the tail
    if (node == list->tail) {
        list->tail = node->prev;
        if (list->tail) list->tail->next = NULL;
    }

    // If node is in the middle
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;

    list->count--;
     deallocatePool(&node_pool, node);
}

void
deleteList(List *list)
{
    Node* ptr = list->head;

    while (ptr) {
        Node* next = ptr->next; 
        deallocatePool(&node_pool, ptr);
        ptr = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count= 0;
}

//this prints the pointer as is  
void
printList(List *list)
{
    if (!list || !list->head) {
        printf("List is empty.\n");
        return;
    }

    Node *ptr = list->head;
    while (ptr) {
        printf("%p\n", ptr->item); 
        ptr = ptr->next;
    }
}


size_t getNodesCount(List * list)
{
    return list->count;
}
