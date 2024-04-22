#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define SHM_NAME "/my_shared_memory"
#define SHM_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number to search> <number of children>\n", argv[0]);
        return 1;
    }

    int x = atoi(argv[1]);
    int n = atoi(argv[2]);
    int shm_fd;
    int *array;

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHM_SIZE);
    array = (int *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < 256; i++) {
        array[i] = i;
    }

    pid_t pids[n];
    for (int i = 0; i < n; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            int start = i * (256 / n);
            int end = (i + 1) * (256 / n);
            for (int j = start; j < end; j++) {
                if (array[j] == x) {
                    printf("Number %d found at index %d by child %d\n", x, j, getpid());
                    munmap(array, SHM_SIZE);
                    exit(0);
                }
            }
            exit(1);
        }
    }

    int status;
    int success = 0;
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            success = 1;
        }
    }

    if (success) {
        printf("Search successful.\n");
    } else {
        printf("Number not found.\n");
    }

    munmap(array, SHM_SIZE);
    shm_unlink(SHM_NAME);
    return 0;
}

