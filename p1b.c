#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        printf("Child process: PID = %d\n", getpid());
        exit(0);
    } else {
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
        sleep(5);
        system("ps -l");
        wait(NULL);
    }
    return 0;
}

