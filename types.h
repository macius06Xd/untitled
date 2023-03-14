//
// Created by paekis on 3/14/23.
//

#include <stdio.h>
#ifndef UNTITLED_TYPES_H
#define UNTITLED_TYPES_H
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
#endif //UNTITLED_TYPES_H
