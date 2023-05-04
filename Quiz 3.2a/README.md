# Quiz 3.2.a: Threads synchonization

Write a C++ program that synchronizes two threads to replicate a TCP 2-way handshake.

One thread prints SYN and the other thread prints ACK.

The program takes one command line argument, which is the number of `SYN`, `ACK` pairs to output:

`./handshake 10` would output 10 pairs of `SYN`, `ACK`.     

The program should launch two threads and output the `SYN`s and `ACK`s in a strictly alternating fashion.

## Sample output for a count of 10

```
[0] SYN ... ACK
[1] SYN ... ACK
[2] SYN ... ACK
[3] SYN ... ACK
[4] SYN ... ACK
[5] SYN ... ACK
[6] SYN ... ACK
[7] SYN ... ACK
[8] SYN ... ACK
[9] SYN ... ACK
```

## Important notes:
1. The program should create two threads -- one runs the `void query() {...}`, while the other runs `void response() {...}`.
1. Provide the threads with necessary args (e.g., the counter, synchronization primitives etc.)
1. Your choice of synchronization primitives are limited to **semaphores**, **condition variables**, and **mutexes**.
1. Use of sleep is not permitted. 
1. All `SYN` prints should come from the thread running `void query() {...}`; while all `ACK` prints should come from the other thread running `void response() {...}`.
