#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void compute(int);

void error(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in server_address, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /*
    The socket() function creates a new socket.
    */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /*
    Read the server port number.
    */
    portno = atoi(argv[1]);

    /*
    The variable server_address is a structure of type struct sockaddr_in. This structure has four fields.
    The first field is short sin_family, which contains a code for the address family.
    AF_INET is an address family that designate the type of addresses that the socket can communicate with (in this case, Internet Protocol v4 addresses).
    The second field is the internet address - Here we use any Address to accept any incoming messages.
    The third field is the port number.
    */
    bzero((char*) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portno);


    /*
    The bind() function binds a socket to an address, in this case the address of the current host and port number on which the server will run.
    It takes three arguments, the socket file descriptor, the address to which is bound, and the size of the address to which it is bound.
    The second argument is a pointer to a structure of type sockaddr, but what is passed in is a structure of type sockaddr_in,
    and so this must be cast to the correct type.
    */
    if (bind(sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
        error("ERROR on binding");

    /*
    The listen function allows the process to listen on the socket for connections.
    The first argument is the socket file descriptor, and the second is the size of the backlog queue, i.e., the number of connections that can be waiting while the process is handling a particular connection.
    */
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        /*
        The accept() function causes the process to block until a client connects to the server.
        The server wakes up the process when a connection from a client has been successfully established.
        It returns a new file descriptor, and all communication on this connection should be done using the new file descriptor.
        The second argument is a reference pointer to the address of the client on the other end of the connection, and the third argument is the size of this structure.
        */
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        /*
        Fork a new child process
        */
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");

        /*
        The child process calls the compute function
        */
        if (pid == 0) {
            close(sockfd);
            compute(newsockfd);
            exit(0);
        } else
            close(newsockfd);
    } /* end of while */
    close(sockfd);

    return 0; /* we never get here */
}

/*
 There is a separate instance of this function for each connection.
 It handles all communication once a connnection has been established.
 */
void compute(int sock) {
    int n;
    char buffer[256];
    bzero(buffer, 256);

    /*
    Read incoming message from socket
    */
    n = read(sock, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("Message from client: %s\n", buffer);

    /*
    Write outgoing message to socket
    */
    n = write(sock, "Message from server: I received your message", 44);
    if (n < 0)
        error("ERROR writing to socket");
}