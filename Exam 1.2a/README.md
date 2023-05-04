Problem 1: synchronization among processes
------------------------------

1. The codebase consists of two .cpp files: `p1.cpp` and `p1-helper.cpp`. We should only work on `p1.cpp`.

2. The program takes one command line argument, which is the number of child processes to fork. The input is provided as follows:

    `./p1 -n 5           // launch 5 child processes`

3. Once launched, each child process should use execvp to switch to `p1-helper` -- which simply prints two lines.

4. However, the prints should be in a specific order. First the parent process should print the child pids in sorted order. Then the child process should print such that the process with larger pid prints first. Sample ouput for the input above:

```
   28769 28770 28771 28772 28773 
   Process 28773: hello 
   Process 28773: exiting 
   Process 28772: hello 
   Process 28772: exiting 
   Process 28771: hello
   Process 28771: exiting
   Process 28770: hello 
   Process 28770: exiting 
   Process 28769: hello
   Process 28769: exiting 
   Parent: exitin
```

5. Hints and restrictions: 
    - Take a look at the usage of `raise` and `kill` system calls. 
    - You should not change the `p1-helper`; all prints for the child processes should come from `p1-helper`.
    - You can use `std::sort` to sort the child process ids.
    - Note that `sleep` does not guarantee the desired synchronization.
