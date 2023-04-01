#include <stdio.h>//LINKED SORTED LIST
#include "list.h"
#include "comp_int.c"
#include "comp_char.c"
#include "comp_str.c"
#include "comp_str2.c"
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

sem_t sem;

struct arguments
{
    int thread_id;
    int beginning;
    int iterations;
    List* l;
};

void *insert_job(void *args){
    int i = 0;
    struct arguments arg = *(struct arguments*)args;
    char to_insert[10];
    char tid[3];
    char num[7];
    while (i < arg.iterations){
        int number_to_insert = arg.beginning + i;
        strcat(to_insert, "t");
        sprintf(tid, "%d",arg.thread_id);
        strcat(to_insert,tid);
        strcat(to_insert,"_");
        sprintf(num, "%d", number_to_insert);
        strcat(to_insert, num);
        strcat(to_insert,"\0");
        insert(arg.l, to_insert);
        //printf("%s\n", to_insert);
        memset(to_insert, 0, 7);
        memset(tid, 0, 3);
        i++;
    }
}

void *delete_job(void *args){
    int i = 0;
    struct arguments arg = *(struct arguments*)args;
    char to_insert[10];
    char tid[3];
    char num[7];
    while (i < arg.iterations){
        int number_to_insert = arg.beginning + i;
        strcat(to_insert, "t");
        sprintf(tid, "%d",arg.thread_id);
        strcat(to_insert,tid);
        strcat(to_insert,"_");
        sprintf(num, "%d", number_to_insert);
        strcat(to_insert, num);
        strcat(to_insert,"\0");
        remove_from_list(arg.l, to_insert);
        //printf("%s\n", to_insert);
        memset(to_insert, 0, 7);
        memset(tid, 0, 3);
        i++;
    }
}

int main(){
    pthread_t t1, t2, t3, t4, t5, t6;
    List* l = create(print_str, comp_str2, sizeof(char[10]));
    printf("Inserting initial test value\n");
    char test[10];
    memset(test, 0, 10);
    strcat(test,"TEST");
    strcat(test,"\0");
    insert(l, test);
    print(l);
    //return 1;
// {thread_id, start_number, iterations, list, semaphore }
    struct arguments args1 = {1, 0 , 10000, l};
    struct arguments args2 = {2, 10000, 10000, l};
    struct arguments args3 = {3, 20000, 10000, l};
    //struct arguments args4 = {4, 30000, 10000, l};
    //struct arguments args5 = {5, 40000, 10000, l};
    //struct arguments args6 = {6, 50000, 10000, l};

    printf("Beggining of insert operations\n");

    pthread_create(&t1, NULL, insert_job, &args1);
    pthread_create(&t2, NULL, insert_job, &args2);
    pthread_create(&t3, NULL, insert_job, &args3);
    //pthread_create(&t4, NULL, insert_job, &args4);
    //pthread_create(&t5, NULL, insert_job, &args5);
    //pthread_create(&t6, NULL, insert_job, &args6);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    //pthread_join(t4, NULL);
    //pthread_join(t5, NULL);
    //pthread_join(t6, NULL);

    printf("Beginning of delete operations\n");

    pthread_create(&t1, NULL, delete_job, &args1);
    pthread_create(&t2, NULL, delete_job, &args2);
    pthread_create(&t3, NULL, delete_job, &args3);
    //pthread_create(&t4, NULL, delete_job, &args4);
    //pthread_create(&t5, NULL, delete_job, &args5);
    //pthread_create(&t6, NULL, delete_job, &args6);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    //pthread_join(t4, NULL);
    //pthread_join(t5, NULL);
    //pthread_join(t6, NULL);
    print(l);
    clear(l);
    return 0;
}