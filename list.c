#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>

List* create(void (*print)(void*), int (*comparator)(void*, void*), int size){
    List* l = (List*)malloc(sizeof(List));
    l->head = NULL;
    l->print = print;
    l->comparator = comparator;
    l->var_size = size;
    sem_t sem;
    l->sem = sem;
    sem_init(&sem, 0, 1);
    return l;
}

void clear(List *l){      
    while(l->head != NULL){
        Node* temp = l->head;
        l->head=l->head->next;
        free(temp->data);
        free(temp);
        }
}

void insert(List *l, void *Data){    
    
    Node* new_node = (Node*)malloc(sizeof(Node));
    void* D = (void*)malloc(l->var_size);
    memcpy(D,Data,l->var_size);
    new_node->data = D;
    new_node->next = NULL;
    sem_wait(&l->sem);

    if(l->head==NULL){// empty list
        l->head=new_node;
            sem_post(&l->sem);

        return;
    }
    Node* iter = l->head;
    Node* previous = NULL;
    if(l->head->next==NULL){// one elem list
        int comp = l->comparator(Data,iter->data);   
        if(comp == -1){ // iter data is bigger
            new_node->next=iter;
            l->head=new_node;
        }
        else{// new data is bigger
            new_node->next = iter->next;
            iter->next = new_node;
        }
        return;
    }
    while( iter->next!=NULL ){
        int comp = l->comparator(Data,iter->data);
        if(comp == -1){ //right is bigger, iter data is bigger, insert
            if(iter == l->head){
                l->head = new_node;
                new_node->next = iter;
                    sem_post(&l->sem);

                return;
            }
            previous->next = new_node;
            new_node->next = iter;
                sem_post(&l->sem);

            return;        
        }
        else { //new data is bigger or equal, travel
            previous = iter;
            iter = iter->next;            
        }
    }
    int comp = l->comparator(Data,iter->data); // compare with last element in a list
    if(comp == -1){ // add before last
        previous->next = new_node;
        new_node->next = iter;
    }
    else{
        iter->next = new_node;
    }
    sem_post(&l->sem);
}

void remove_from_list(List *l, void *Data){
    
    int comp2 = l->comparator(Data,l->head->data);
    sem_wait(&l->sem);
    if(l->head==NULL) {  //empty list
        return;
    }
    if(l->head->next == NULL && comp2 == 0){//one elem list
        free(l->head);
        l->head = NULL;
    }
      
    else{//2+ elem list
        Node* iter = l->head;
        Node* previous = NULL;
        while (iter->next != NULL) // go through whole list 
        {
            int comp = l->comparator(Data,iter->data);// iter.data = data ?
            if(comp == 0){ 
                
                if(iter == l->head){
                    l->head = iter->next;
                    free(iter);
                    iter = l->head;
                    continue;
                }
                else{
                    previous->next = iter->next;
                    free(iter);
                    iter = previous->next;
                    continue;
                }         
            }
            previous = iter; 
            iter = iter->next;
        } 
        int comp = l->comparator(Data,iter->data);
        if(comp == 0 ){ // checkup for last elem
            previous->next = NULL;
            free(iter);
        }
    }
    sem_post(&l->sem);
}

void print(List *l){    
    if(l->head==NULL) {        
        return;
    }
    else{        
        Node* iter = l->head;
        l->print(iter->data);
        while (iter->next != NULL)
        {         
            iter = iter->next;
            l->print(iter->data);
        }
        
    }
}