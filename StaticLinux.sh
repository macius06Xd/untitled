gcc -c -g list.c
ar rcs libStaticLinux.a list.o
gcc -c -g main.c
gcc -o StaticLinux main.o -L. -lStaticLinux
./StaticLinux