#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void create_process_tree(int depth, int max_depth) {
    if (depth >= max_depth) {
        return;
    }

    pid_t left_child_pid = fork();
     if (left_child_pid == 0) {
        printf("I am process %d (left child of %d)\n", getpid(), getppid());
        create_process_tree(depth + 1, max_depth);
        return;
    }
    pid_t right_child_pid =  fork();
     if (right_child_pid == 0) {
        printf("I am process %d (right child of %d)\n", getpid(), getppid());
        create_process_tree(depth + 1, max_depth);
        return;
    }

}

int main(int argc, char *argv[]) {
	int a = getpid();    
if (argc < 2) {
        return
 }

    int max_depth = atoi(argv[1]);

    create_process_tree(0, max_depth-1);

    // print the process tree using pstree
	if(a== getpid())
{
sleep(3);
    char command[256];
    sprintf(command, "pstree -p %d", getpid());
    system(command);
}
else
{
sleep(100);
}
    return EXIT_SUCCESS;
}
