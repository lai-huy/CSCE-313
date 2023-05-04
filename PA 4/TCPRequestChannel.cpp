#include "TCPRequestChannel.h"
#include "common.h"
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

TCPRequestChannel::TCPRequestChannel(const std::string _ip_address, const std::string _port_no) {
    if (_ip_address.empty()) {
        struct addrinfo hints, * serv;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP

        int rv;
        if ((rv = getaddrinfo(NULL, _port_no.c_str(), &hints, &serv))) {
            cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
            exit(-1);
        }

        if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) < 0) {
            perror("Failed to socket server");
            exit(-1);
        }

        if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) < 0) {
            close(sockfd);
            perror("Failed to bind server");
            exit(-1);
        }

        if (listen(sockfd, 20) == -1) {
            perror("Failed to listen");
            exit(1);
        }

        freeaddrinfo(serv); // all done with this structure
    } else {
        struct addrinfo hints, * res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(_ip_address.c_str(), _port_no.c_str(), &hints, &res)) {
            cerr << "getaddrinfo failed\n";
            exit(-1);
        }

        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0) {
            cerr << "socket failed\n";
            exit(-1);
        }

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
            cerr << "connect failed\n";
            exit(-1);
        }

        freeaddrinfo(res);
    }
}

TCPRequestChannel::TCPRequestChannel(int _sockfd): sockfd{_sockfd} {}

TCPRequestChannel::~TCPRequestChannel() { close(this->sockfd); }

int TCPRequestChannel::accept_conn() {
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    return accept(this->sockfd, (struct sockaddr*) &their_addr, &addr_size);
}

int TCPRequestChannel::cread(void* msgbuf, int msgsize) {
    return recv(this->sockfd, msgbuf, msgsize, 0);
}

int TCPRequestChannel::cwrite(void* msgbuf, int msgsize) {
    return send(this->sockfd, msgbuf, msgsize, 0);
}
