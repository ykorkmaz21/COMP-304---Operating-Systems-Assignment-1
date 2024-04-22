#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of forks>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    printf("Main Process ID: %d, level: 0\n", getpid());

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            printf("Process ID: %d, Parent ID: %d, level: %d\n", getpid(), getppid(), i + 1);
        } else if (pid > 0) {
            if (i == 0) {
                printf("Process ID: %d, Parent ID: %d, level: 1\n", pid, getpid());
            }
            wait(NULL);
        } else {
            perror("Fork failed!");
            exit(1);
        }
    }
    return 0;
}
 
