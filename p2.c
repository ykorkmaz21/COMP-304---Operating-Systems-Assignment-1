#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <float.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <n> <command> [<args>...]\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int fd[n][2];

    for (int i = 0; i < n; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            close(fd[i][0]);
            int devNull = open("/dev/null", O_WRONLY);
            dup2(devNull, STDOUT_FILENO);
            dup2(devNull, STDERR_FILENO);
            close(devNull);

            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            for (int j = 0; j < 10; j++) {
                system(argv[2]);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);

            double execTime = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
            write(fd[i][1], &execTime, sizeof(execTime));
            close(fd[i][1]);
            exit(0);
        }
        close(fd[i][1]);
    }

    double times[n], max = 0.0, min = DBL_MAX, sum = 0.0;
    for (int i = 0; i < n; i++) {
        double execTime;
        read(fd[i][0], &execTime, sizeof(execTime));
        wait(NULL);  // Wait for child to prevent zombies
        times[i] = execTime;
        printf("Child %d Executed in %.2f millis\n", i + 1, execTime);
        if (execTime > max) max = execTime;
        if (execTime < min) min = execTime;
        sum += execTime;
        close(fd[i][0]);
    }

    printf("Max: %.2f millis\n", max);
    printf("Min: %.2f millis\n", min);
    printf("Average: %.2f millis\n", sum / n);

    return 0;
}

