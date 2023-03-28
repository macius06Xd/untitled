#include "List.h"
#include <pthread.h>

typedef struct test test;
struct test{
    char a;
    int b;
};
void print_test(void* value)
{
    test return_value = *(test*)(value);
    printf("[%c,%d] ",return_value.a,return_value.b);

}
int compare_test(void * first, void* second)
{
    test first_i = *((test*)first);
    test second_i = *((test*)second);
    return first_i.b > second_i.b;
}
void print_int(void* value)
{
    int * return_value = (int*)(value);
    printf("%d ",*return_value);

}
int compare_int(void * first, void* second)
{
    int first_i = *((int*)first);
    int second_i = *((int*)second);
    return first_i > second_i;
}

void apply_example(void * value)
{
    int * k = (int*)value;
    *k=*k+1;

}

int predicate_char(void *c,int parameter_number,va_list list)
{
    char znak = *(char*)c;
    for(int i = 0 ; i < parameter_number ; i++) {
        if (znak == (char) va_arg(list, int)) {
            return 1;
        }
    }
        return 0;
}
struct add_args
{
    int start;
    int number;
    List *lista;
};
void *adding_job(void *arg)
{
    struct add_args args = *(struct add_args*)arg;
    int i = 0 ;
    while (i<args.number)
    {
        int value = args.start+i;
        list_add(args.lista,&value);
        i++;
    }
}
struct del_args
{
    int number;
    List *lista;
};
void *delete_job(void* arg)
{
    struct del_args args = *(struct del_args*)arg;
    int i = 0 ;
    while(i<args.number)
    {
        list_delete(args.lista,1);
        i++;
    }
}
//Sorted list that can hold any type of data
int
main(int argc, char **argv) {
	List *lista = create_list(print_int,compare_int,sizeof(int));
    int a = 2137;
    list_add(lista,&a);
     pthread_t t1,t2,t3,t4,t5;
    int number = atoi(argv[1]);
    int start = 10000;
    list_print(lista);
    struct add_args addArgs = {10000,number,lista};
    pthread_create(&t1,NULL,adding_job,&addArgs);
    struct add_args addArgs2 = {20000,number,lista};
    pthread_create(&t2,NULL,adding_job,&addArgs2);
    struct add_args addArgs3 = {30000,number,lista};
    pthread_create(&t3,NULL,adding_job,&addArgs3);
    struct add_args addArgs4 = {40000,number,lista};
    pthread_create(&t4,NULL,adding_job,&addArgs4);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    list_print(lista);
    struct del_args delArgs = {number,lista};
    pthread_create(&t1,NULL, delete_job,&delArgs);
    pthread_create(&t2,NULL, delete_job,&delArgs);
    pthread_create(&t3,NULL, delete_job,&delArgs);
    pthread_create(&t4,NULL, delete_job,&delArgs);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    list_print(lista);
}

