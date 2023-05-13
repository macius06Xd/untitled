#include  "types.h"
#include <pthread.h>

List* create_list(void (*print)(void *),int (*compare)(void*,void*),size_t data)
{
    List* lista = malloc(sizeof(List));
    lista->head = NULL;
    lista->tail = NULL;
    lista->compare = compare;
    lista->print = print;
    lista->data_size = data;
    pthread_mutex_init(&lista->mutex,NULL);
    return lista;
}

void list_print( List* list) {
    pthread_mutex_lock(&list->mutex);
    Node* Iterator = list->head;
    int i = 0;
    while(Iterator != NULL)
    {
        list->print(Iterator->data);
        Iterator = Iterator->next;
        i++;
    }
    printf("\n");
    pthread_mutex_unlock(&list->mutex);
}

void list_printTail( List* list) {
    pthread_mutex_lock(&list->mutex);
    list->print(list->tail->data);
    printf("\n");
    pthread_mutex_unlock(&list->mutex);
}

void copy_content(void **target, void *source, size_t data_size) {
    memcpy(*target,source,data_size);
}

void list_add(List *list, void *value) {
	pthread_mutex_lock(&list->mutex);
    Node * new_node = (Node*)malloc(sizeof(Node));
    new_node->data = malloc(list->data_size);
    new_node->next = NULL;
    copy_content(&(new_node->data),value,list->data_size);
    Node * iterator = list->head;
    Node* pNode = NULL;
   
    while(iterator != NULL  )
    {
        if(!list->compare(iterator->data,value))
        {
            pNode = iterator;
            iterator=iterator->next;}
        else
        {
            break;
        }
    }
    if(pNode == NULL)
    {
        list->head = new_node;
    }
    else
    {
        pNode->next = new_node;

    }
    new_node->next = iterator;
    if(iterator == NULL)
    {
        list->tail=new_node;
    }
    fprintf(stderr,"new port added %d" , *(int*)new_node ->data);
    pthread_mutex_unlock(&list->mutex);
}

int list_size( List* list) {
    pthread_mutex_lock(&list->mutex);
    Node * iterator = list->head;
    int i = 0;
    while(iterator!=NULL)
    {
        i++;
        iterator = iterator->next;
    }
    return i;
}

void free_node(Node *node) {
    free((node)->data);
    free(node);
}

void list_delete(List *list, int k) {
    Node * iterator = list->head;
    Node * previous = NULL;
    pthread_mutex_lock(&list->mutex);
    for(unsigned int i = 0 ; i < k ; i++)
    {
        if(iterator!=NULL)
        {
            previous = iterator;
            iterator = iterator->next;
        }
        else
        {
            pthread_mutex_unlock(&list->mutex);
            return;
        }
    }
    if(iterator == list->head)
    {
        Node * temp = list->head->next;
        if(list->head == list->tail)
        {
            list->tail = NULL;

        }
        free_node((list->head));
        list->head = temp;
        pthread_mutex_unlock(&list->mutex);
        return;
    }
    if(iterator == list->tail)
    {
        free(list->tail);
        list->tail = previous;
        list->tail->next = NULL;
        pthread_mutex_unlock(&list->mutex);
        return;
    }
    Node * temp = iterator->next;
    free(iterator);
    previous->next = temp;
    pthread_mutex_unlock(&list->mutex);
}
static void list_sort(List *list) {
    
    //Node current will point to head
    struct Node *current = list->head, *index = NULL;
    void *temp;

    if(list->head == NULL) {
        return;
    }
    else {
        while(current != NULL) {
            //Node index will point to node next to current
            index = current->next;

            while(index != NULL) {
                //If current node's data is greater than index's node data, swap the data between them
                if(list->compare(current->data , index->data)) {
                    temp = current->data;
                    current->data = index->data;
                    index->data = temp;
                }
                index = index->next;
            }
            current = current->next;
        }
    }
}

void remove_from_list(List *l, void *Data){
    pthread_mutex_lock(&l->mutex);
    Node * iter = l->head;
    Node * prev = NULL;
    while(iter != NULL)
    {
        if(l->compare(iter->data,Data)==0)
            break;
        prev = iter;
        iter = iter->next;
    }
    if(iter == NULL) {
        pthread_mutex_unlock(&l->mutex);
        return;
    }
    if( prev == NULL)
    {
        l->head = iter->next;
    }
    else
    {
        prev->next = iter->next;
    }
    pthread_mutex_unlock(&l->mutex);
    free(iter);

}

void list_modify(List *list, int k, void *value) {
    pthread_mutex_lock(&list->mutex);
    void * target;
    Node * iterator = list->head;
    for(int i = 0 ; i<k ; i++)
    {
        if(iterator == NULL)
            return;
        iterator=iterator->next;
    }
    target = iterator->data;
    copy_content(&target,value,list->data_size);
    list_sort(list);
    pthread_mutex_unlock(&list->mutex);
}

List* list_split(List *list, int start, int length) {
    pthread_mutex_lock(&list->mutex);
    List* new_list = create_list(list->print,list->compare,list->data_size);
    Node * iterator = list->head;
    for(int i = 0 ; i<start;i++)
    {
        if(iterator== NULL)
        {
            pthread_mutex_unlock(&list->mutex);
            return new_list;

        }
        iterator = iterator->next;
    }
    Node * new_list_iterator = NULL;
    for(int i = 0 ; i < length ;i++)
    {
        if(iterator == NULL) {
            pthread_mutex_unlock(&list->mutex);
            return new_list;

        }
        Node * new_node = malloc(sizeof (Node));
        new_node->data = malloc(sizeof(new_list->data_size));
        copy_content(&new_node->data,iterator->data,new_list->data_size);
        if(i==0)
        {
            new_list_iterator = new_list->head = new_node;
        }
        else
        {
            new_list_iterator->next = new_list->tail = new_node;
            new_list_iterator = new_list_iterator->next;
            new_list_iterator->next = NULL;
        }
        iterator=iterator->next;

    }
    pthread_mutex_unlock(&list->mutex);
    return new_list;

}

void  list_value  (List *list, int k , void* value)
{
    pthread_mutex_lock(&list->mutex);
    Node * iterator = list->head;
    for(int i = 0 ; i < k ; i++)
    {
        if(iterator == NULL)
        {
            pthread_mutex_unlock(&list->mutex);
            return;

            }
        iterator = iterator->next;
    }
    if(value == NULL)
    {
        value = malloc(sizeof (list->data_size));
    }
    memcpy(value,iterator->data,list->data_size);
    pthread_mutex_unlock(&list->mutex);
}



void list_apply(List * list , int(*predicate)(void*,int,va_list),int arg_number,...){
pthread_mutex_lock(&list->mutex);
    Node * iterator = list->head;
    Node * previous = NULL;
    va_list arguments;

        while(iterator!=NULL){
            va_start(arguments,arg_number);
        predicate(iterator->data,arg_number,arguments);
        iterator=iterator->next;
            va_end(arguments);
    }

    pthread_mutex_unlock(&list->mutex);
}

void push_back(List *list , void* value)
{
    Node * new_node = (Node*)malloc(sizeof(Node));
    new_node->data = malloc(list->data_size);
    new_node->next = NULL;
    copy_content(&(new_node->data),value,list->data_size);
    if(list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        return;
    }
    list->tail->next = new_node;
    list->tail = new_node;


}
List* list_combine(List* first, List* second) {
    pthread_mutex_lock(&first->mutex);
    pthread_mutex_lock(&second->mutex);
    List* result = create_list(first->print,first->compare,first->data_size);
    Node * it_first = first->head;
    Node * it_second = second->head;
    while(it_first !=NULL || it_second !=NULL)
    {

        if(it_first!=NULL) {
            if (it_second == NULL || !first->compare(it_first->data, it_second->data)) {
                push_back(result, it_first->data);
                it_first = it_first->next;
            }
            else
            {
                push_back(result,it_second->data);
                it_second = it_second->next;
            }
        }
        else
            {
                push_back(result,it_second->data);
                it_second = it_second->next;
            }

    }
    pthread_mutex_unlock(&first->mutex);
    pthread_mutex_unlock(&second->mutex);
    return result;

}

void list_filter(List * list , int(*predicate)(void*,int,va_list),int arg_number,...)
{
    pthread_mutex_lock(&list->mutex);
    Node * iterator = list->head;
    Node * previous = NULL;
    va_list arguments;
    va_start(arguments,arg_number);
    while(iterator != NULL)
    {
        if(predicate(iterator->data,arg_number,arguments))
        {
            if(previous == NULL)
            {
                list->head = iterator->next;
            }
            if( iterator->next ==NULL)
            {
                list->tail = previous;
            }
            Node * temp = iterator->next;
            free_node(iterator);
            if(previous != NULL)
            {
                previous->next = temp;
            }
            iterator = temp;

        }
        else {
            previous=iterator;
            iterator = iterator->next;
        }
    }
    va_end(arguments);
    pthread_mutex_unlock(&list->mutex);
}

void list_clear (List * list)
{
    pthread_mutex_lock(&list->mutex);
    Node * iterator = list->head;
    while(iterator != NULL)
    {
        Node * temp = iterator ->next;
        free_node(iterator);
        iterator = temp;
    }
    list->head = NULL;
    list->tail = NULL;
    pthread_mutex_unlock(&list->mutex);
}

void Delete_list(List* lista)
{
    list_clear(lista);
    pthread_mutex_destroy(&lista->mutex);
    free(lista);
}

