#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE (1024 * 1024)

int main(void)
{
    char *shared_data = mmap(NULL, SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        strcpy(shared_data, "Weather data received successfully");
        printf("Child: Data written to shared memory\n");
        munmap(shared_data, SIZE);
        _exit(0);
    }
    else {
        waitpid(pid, NULL, 0);
        printf("Parent: Received data: %s\n", shared_data);
        munmap(shared_data, SIZE);
    }

    return 0;
}