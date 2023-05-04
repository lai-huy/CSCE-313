#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>          
#include <sys/stat.h>

using std::string, std::cout, std::endl;

#define MAX_MESSAGE 256

long long unsigned int hash(int seed, char* buf, int nbytes) {
    long long unsigned int H = seed;
    for (int i = 0; i < nbytes; ++i)
        H = H * 2 * buf[i] + 1;
    return H;
}

int main(int argc, char** argv) {
    string strToHash(argc ? argv[1] : "hello world");

    // TODO: create pipe 
    int fd1[2]{};
    if (pipe(fd1) < 0) {
        perror("pipe 1 failed");
        exit(EXIT_FAILURE);
    }

    int fd2[2]{};
    if (pipe(fd2) < 0) {
        perror("pipe 2 failed");
        exit(EXIT_FAILURE);
    }

    int bytes = strToHash.size();
    char buf[MAX_MESSAGE]{};

    for (string::const_iterator iter = strToHash.cbegin(); iter != strToHash.cend(); ++iter)
        buf[iter - strToHash.cbegin()] = *iter;


    int pid = fork();
    if (pid == 0) {
        // TODO: read from parent
        close(fd1[1]);
        read(fd1[0], buf, bytes);
        close(fd1[0]);
        // TODO: compute hash 
        long long unsigned int h = hash(getpid(), buf, bytes);

        // TODO: send hash to parent
        close(fd2[0]);
        write(fd2[1], &h, sizeof(h));
        close(fd2[1]);
    } else {
        // TODO: write to child 
        close(fd1[0]);
        write(fd1[1], buf, bytes);
        close(fd2[1]);

        // TODO: get hash from child 
        long long unsigned int hrecv;
        close(fd2[1]);
        read(fd2[0], &hrecv, sizeof(hrecv));
        close(fd2[0]);

        // TODO: calculate hash on parent side
        long long unsigned int h;
        h = hash(pid, buf, bytes);

        // print hashes; DO NOT change
        printf("%llX\n", h);
        printf("%llX\n", hrecv);
    }

    return 0;
}