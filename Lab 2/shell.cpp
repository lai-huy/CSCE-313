/****************
LE2: Introduction to Unnamed Pipes
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main() {
    char* cmd1[] = {(char*) "ls", (char*) "-al", (char*) "/", nullptr};
    char* cmd2[] = {(char*) "tr", (char*) "a-z", (char*) "A-Z", nullptr};

    int std_cin = dup(STDIN_FILENO); // store std::in value
    int std_cout = dup(STDOUT_FILENO); // store std::out value

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (!fork()) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        execvp(cmd1[0], cmd1);
    }

    if (!fork()) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        execvp(cmd2[0], cmd2);
    }

    // Reset the input and output file descriptors of the parent.
    dup2(std_cin, STDIN_FILENO);
    dup2(std_cout, STDOUT_FILENO);
}
