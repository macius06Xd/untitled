gcc -c -fPIC list.c
gcc list.o -shared -o libDynamicLinux.so
gcc -c -g main.c
gcc -o DynamicLinux main.o -L. -lDynamicLinux
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/nikky/dev/untitled
./DynamicLinux