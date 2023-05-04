# Client-server communication

Write a client program that establishes a connection with a server and sends messages to the server. The client and the server communicate using stream sockets. Both applications should run on the same host computer. 

## Steps
The server program is provided, do not modify it.

Update the client program.

The makefile provided will generate two executables: `server` and `client`.

### Start the server first:
`.\server <port number>`

### Start the client next:
The client needs two arguments, the server hostname (`localhost`) since the server is running on the same computer and the port number on which the server is listening. 

`.\client localhost <server port number>`

1. The client will prompt the user to enter a message.
1. The server should display the message.
1. The client should receive a response from the server and terminate.

## Sample output
### Server
```
$ ./server 8080

Message from client: A long time ago, in a galaxy far, far away

Message from client: It is a period of civil war
```

### Client
```
$ ./client localhost 8080
Please enter the message: A long time ago, in a galaxy far, far away
Message from server: I received your message

$ ./client localhost 8080
Please enter the message: It is a period of civil war
Message from server: I received your message
```

## Testing
1. Your program will be manually tested for correctness with additional test cases.
1. Your program should compile with no errors and warnings.

