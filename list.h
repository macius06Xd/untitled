#include <stdlib.h>
#include <semaphore.h>

typedef struct Node Node;
typedef struct List List;

struct Node
{
    void *data;
    Node *next;
};


struct List
{
    Node *head;
    void (*print)( void*);
    int (*comparator)( void*, void*);
    int var_size;
    sem_t sem;
};

List* create(void (*print)(void*), int (*comparator)(void*, void*), int size);
void insert(List *l, void *Data);
void remove_from_list(List *l, void *Data);
void print(List *l);
void clear(List *l);