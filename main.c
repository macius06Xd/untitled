#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "List.h"


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
void print_char(void* value)
{
    char * return_value = (char*)(value);
    printf("%c ",*return_value);

}
int compare_char(void * first, void* second)
{
    char first_i = *((char*)first);
    char second_i = *((char*)second);
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

//Sorted list that can hold any type of data
int
main(int argc, char **argv) {
	/*
        List lista = {NULL, NULL, print_int, compare_int, sizeof(int)};
        int a = 2;
        int b = 6;
        int c = 4;
        int d = 1;
        int e = 0;*/

    List lista = {NULL, NULL, print_char, compare_char, sizeof(char)};
    char a = 'a';
    char b = 'b';
    char c = 'c';
    char d = 'd';
    char e = 'e';

    list_add(&lista, &a);
    list_add(&lista, &b);
    list_add(&lista, &c);
    list_add(&lista,&d);
    list_add(&lista,&e);
    list_print(lista);

    list_delete(&lista,4);
    list_delete(&lista,0);
    list_delete(&lista,1);
    list_print(lista);

    list_add(&lista,&a);
    list_print(lista);
    list_printTail(lista);

    char *value2 = malloc(sizeof(char));
    list_value(lista, 0, value2);
    list_print(lista);
    list_apply(&lista, apply_example);
    list_print(lista);
    List nowa_lista = list_combine(lista,lista);
    list_print(nowa_lista);
    list_filter(&nowa_lista,predicate_char,3,'e','b','k');
    list_print(nowa_lista);
    list_clear(&nowa_lista);
    list_print(nowa_lista);

    test test_a = {'a',1};
    test test_b = {'b',2};
    test test_c = {'p',-15};
    List lista_test = {NULL, NULL, print_test, compare_test, sizeof(test)};
    list_add(&lista_test,&test_a);
    list_add(&lista_test,&test_b);
    list_add(&lista_test,&test_c);
    list_print(lista_test);
    list_apply(&lista_test, apply_example);
    list_print(lista_test);


    List combine_lista = list_combine(lista,lista_test);
    list_print(combine_lista);
    list_sort(&combine_lista);
    list_print(combine_lista);

    List split_lista = list_split(&combine_lista,2,3);
    list_print(split_lista);
    free(value2);
}

