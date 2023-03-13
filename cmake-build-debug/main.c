#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "List.c"


void a(char* d )
{
    printf("a");
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
    return first_i < second_i;
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

        List lista = {NULL, NULL, print_char, compare_char, sizeof(int)};
        char a = 'a';
        char b = 'b';
        char c = 'c';
        char d = 'd';
        char e = 'e';

    list_add(&lista,&a);
    list_print(lista);
    list_add(&lista,&b);
    list_print(lista);
    list_add(&lista,&c);
    list_print(lista);
    list_add(&lista,&d);
    list_print(lista);
    list_add(&lista,&e);
    list_print(lista);
    list_delete(&lista,4);
    list_print(lista);
    list_delete(&lista,0);
    list_print(lista);
    list_delete(&lista,1);
    list_print(lista);
    list_add(&lista,&a);
    list_print(lista);
}
