//
// Created by wojte on 15.03.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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
