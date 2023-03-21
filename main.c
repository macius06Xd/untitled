#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void create_process_tree(int depth, int max_depth) {
    if (depth >= max_depth) {
        return;
    }

    pid_t left_child_pid = fork();
    if (left_child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (left_child_pid == 0) {
        // child process (left)
        printf("I am process %d (left child of %d)\n", getpid(), getppid());
        create_process_tree(depth + 1, max_depth);
        exit(EXIT_SUCCESS);
    }

    pid_t right_child_pid = fork();
    if (right_child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (right_child_pid == 0) {
        // child process (right)
        printf("I am process %d (right child of %d)\n", getpid(), getppid());
        create_process_tree(depth + 1, max_depth);
        exit(EXIT_SUCCESS);
    }

    // parent process
    wait(NULL);
    wait(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <depth>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int max_depth = atoi(argv[1]);

    create_process_tree(0, max_depth);

    // print the process tree using pstree
    char command[256];
    sprintf(command, "pstree -p %d", getpid());
    system(command);

    return EXIT_SUCCESS;
}