//
// Created by wojte on 11.03.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifndef UNTITLED_LIST_H
#define UNTITLED_LIST_H
typedef struct Node Node;
typedef struct List List;
struct Node {
    void *data;
    Node *next;
};

struct List {
    Node * head;
    Node * tail;
    void (*print)(void *);
    int (*compare)(void*,void*);
    size_t data_size;
};
// prints whole list uses given print function
void list_print (const List list );

// prints last element of list
void list_printTail(const List list);

// alias for memcpy
void copy_content(void ** target , void* source,size_t data_size);

// adds new value to an array creates a copy of the value
void list_add (List* list , void* value);

//returns size of the list
int list_size(List list);

//frees the memory //user is responsible for clearing any allocated memory inside a given value//
void free_node(Node * node);

//deletes node at a given position
void list_delete ( List * list , int k );

// changes the value holded by the node
void list_modify ( List *list, int k,void * value );

// create a copy of a part of the list
List list_split (List *list , int start , int length);
// saves a content of the node to passes value
void  list_value  (List list, int k,void * value);

//sorts list
void  list_sort(List *list);

// applies given function to every list value
void list_apply(List * list, void(* function)(void*));

// combines two lists
List list_combine(List first , List second);

// filter list based on predicate
void list_filter(List *list , int(*predicate)(void*,int,va_list),int arg_number,...);

//clears list
void list_clear(List * list);

__attribute__ ((visibility ("hidden"))) static void push_back(List *list , void* value);

#endif //UNTITLED_LIST_H
