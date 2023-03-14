gcc -c List.c -o List.o
gcc -c main.c -o main.o

ar r lib_List.a List.o

gcc main.o lib_List.a -o program_static

gcc -c -fPIC List.c
gcc List.o -shared -o lib_List.so

gcc main.o -L . -l _List -o program_dynamic


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/paekis/dev/untitled
