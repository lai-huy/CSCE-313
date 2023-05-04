# Performance optimization with threading

This program approximates the value of pi using the Riemann sum method.

You can read about it here: https://dotink.co/posts/pi-by-riemann-sum.

We provided the entire algorithm, and your goal is to parallelize the program to improve its performance.

There are two sections of the code that you need to parallelize. The first one is the for loop that calculates the value of each square (see `single_sum_thread`). The second one is the for loop that calculates the sum of the squares (see `pi_sum_thread`). The second summation is the pi approximation.

## main.cpp
### Section 1
Similar to LE6, you must create a vector of threads, call the thread constructor, and push back nthreads # of threads. Pass in the appropriate arguments to the thread function. Then, join the threads.

Hint: The single_num_thread has an id argument. This is so that each thread knows what section of the sum array it works on.

### Section 2
Your goal is to parallelize the code given (comment it out when you finish). Reference the `pi_sum_thread` and compare it to the serial code. We deliberately added a `sleep(1)` in both the serial code and the function to simulate a long-running function. Your goal is to make the output time of this section to be about 1 second, like the sample output.

Hint: You do not need to understand the algorithm. Think about parallelizing a loop. The `pi_sum_thread` is not thread-safe, so make sure to add the appropriate synchronization code to make it thread-safe. This also means that the shared variable(s) must be passed by reference and not by value.

## Sample output
For two threads, this is a sample output (Ignore the first two lines):
```
Number of threads: 2
Cache padding for coherency: 64 bytes
---
Pi approximation: 3.141593
Time to complete part 1: 3.355059 seconds
Time to complete part 2: 1.000477 seconds
```
- Your pi should be very close to 3.1415.
- The time to complete part 1 should be around this time (or 1/2 of single thread performance)
- The time to complete part 2, if parallelized correctly, should be close to 1 second.

## Notes
1. Remember, you want the critical section to be as small as possible.
1. No argument is passed to the program. Just make and run it.
1. You can complete part 1 and run it first without doing part 2.
1. Your program will be manually tested for correctness with additional test cases.
1. Your program should compile with no errors and warnings.
