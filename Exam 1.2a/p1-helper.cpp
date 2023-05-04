#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

int main() {
    /*
    DO NOT CHANGE THIS FILE
    */

    printf("Process %d: hello\n", getpid());
    printf("Process %d: exiting\n", getpid());
    return 0;
}