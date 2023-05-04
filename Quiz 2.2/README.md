# Quiz 2: fork, exec, and environment variables

Write a C program that forks two processes that print command-line arguments and environment variables.

Before your start coding, you can briefly read about environment variables in Linux here: https://www.geeksforgeeks.org/environment-variables-in-linux-unix/

Each process should echo all its command-line arguments and its entire environment list. 

## Parent process
The parent process should:
- Print its process ID

## First child process
The first process should:
- Print its process ID
- Call the `echoall` executable using `execle`, which requires a **pathname** and a specific environment. 
  - `execle` should pass two arguments to `echoall` that are: "Bandicoot" and "Pacman".
  - `execle` should use the custom environment variables provided in `custom_env`.

## Second child process
The second process should:
- Print its process ID
- Call the `echoall` executable using `execlp`, which uses a **filename**.
  - `execlp` should pass one argument to `echoall` that is: "Spyro".
  - The caller’s environment is automatically passed to the new program. 

## Important notes: 
1. For `execlp` to work, the path to the executable program `echoall` should be in your system path. Read about how to set your `$PATH` variable in Linux at https://opensource.com/article/17/6/set-path-linux.
1. Another reference for adding `$PATH`: https://zwbetz.com/how-to-add-a-binary-to-your-path-on-macos-linux-windows/
1. You can create a directory `bin` and add `/home/your_username/bin/` to your path.
1. For both exec calls, set the first argument, `argv[0]` in the new program, to be the filename component of the pathname. 
1. We provided the program echoall.c that should be executed by both processes. Compile echoall.c and move the executable to a directory in your `$PATH`.
1. Refer to the sample output [expected_output.txt](expected_output.txt) as an example
