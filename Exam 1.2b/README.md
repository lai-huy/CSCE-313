Problem 2:
------------------------------

Goal: bidirectional communication with anonymous pipes

1. The codebase consists of a single .cpp file: `p2.cpp`. 

2. The program should have the following functionalities:
    - Maintain two processes (P1 and P2) and communicate between them with anonymous pipes.
    - Send a message from P1 (of upto 256 bytes) to P2 using pipes. 
    - Once P2 receives the message, it will use a hash function to compute hash of the message. The seed for the hash function should be the pid of P2.
    - P2 then sends back the computed hash to P1 using pipes.
    - P1 reads the hash from pipe, computes its own version of the hash (using pid of P2 as seed as well), and prints both.
    - Both processes exit.

3. The program takes one command line argument -- a string to pass from P1 to P2.

   `./p2 "hello world"      // "hello world" should be sent from P1 to P2`

4. Sample output should just print the two hash values in hex. All other prints should be removed for the autograder to function.

5. Hint: 
    - Check read/write functions used in FIFORequestChannel.
