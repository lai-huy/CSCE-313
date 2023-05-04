#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
char* custom_env[] = {"USER=Pikachu", "PATH=/tmp", NULL};

int main(void) {
    printf("Running parent process PID: %d\n\n", getpid());
    fflush(stdout);

    pid_t pid1 = fork();
    pid_t pid2;

    if (pid1 < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (!pid1) {
        printf("Running child process PID: %d\n", getpid());
        fflush(stdout);

        const char* path = "echoall";
        if (execle(path, path, "Bandicoot", "Pacman", NULL, custom_env) < 0) {
            fprintf(stderr, "Error: execle failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    int status;
    waitpid(pid1, &status, 0);

    pid2 = fork();
    if (pid2 < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (!pid2) {
        printf("Running child process PID: %d\n", getpid());
        fflush(stdout);
        if (execlp("echoall", "echoall", "Spyro", NULL) < 0) {
            fprintf(stderr, "Error: execlp failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    waitpid(pid2, &status, 0);

    exit(0);
}