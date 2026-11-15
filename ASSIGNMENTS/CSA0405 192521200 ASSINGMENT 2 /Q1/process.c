/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    pid_t children[2];

    printf("Parent Process Started\n");
    printf("Parent PID: %ld\n", (long)getpid());
    fflush(stdout);

    // Create two child processes
    for (int i = 0; i < 2; i++)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            return 1;
        }

        if (pid == 0)
        {
            printf("Child %d created. PID: %ld, Parent PID: %ld\n",
                   i + 1, (long)getpid(), (long)getppid());

            sleep(4);

            printf("Child %d terminated. PID: %ld\n",
                   i + 1, (long)getpid());

            fflush(stdout);
            _exit(0);
        }

        children[i] = pid;
    }

    printf("\n--- Process Information ---\n");

    for (int i = 0; i < 2; i++)
    {
        char path[100];
        char command[150];

        printf("\nChild PID: %ld\n", (long)children[i]);

        // Display process information from /proc
        snprintf(path, sizeof(path), "/proc/%ld/status",
                 (long)children[i]);

        FILE *fp = fopen(path, "r");

        if (fp != NULL)
        {
            char line[256];

            while (fgets(line, sizeof(line), fp))
            {
                if (strncmp(line, "Name:", 5) == 0 ||
                    strncmp(line, "State:", 6) == 0 ||
                    strncmp(line, "Pid:", 4) == 0 ||
                    strncmp(line, "PPid:", 5) == 0)
                {
                    printf("%s", line);
                }
            }

            fclose(fp);
        }
        else
        {
            printf("/proc information unavailable\n");
        }

        // Display process information using ps
        snprintf(command, sizeof(command),
                 "ps -o pid,ppid,state,comm -p %ld",
                 (long)children[i]);

        printf("\nUsing ps command:\n");
        system(command);
    }

    // Wait for both child processes
    for (int i = 0; i < 2; i++)
    {
        waitpid(children[i], NULL, 0);
    }

    printf("\nParent Process: All children completed.\n");

    return 0;
}
