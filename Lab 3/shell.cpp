/****************
LE2: Basic Shell
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
#include <sys/wait.h> // wait
#include <iostream>
#include "Tokenizer.h"
using namespace std;

/**
 * @brief Converts a vector of strings into a dynamic array of c strings.
 *
 * @param vect vector to convert
 * @return char** converted strings
 */
char** vec_to_char_array(const vector<string>& vect) {
    char** result = new char* [vect.size() + 1] {};
    for (size_t i = 0; i < vect.size(); ++i)
        result[i] = const_cast<char*>(vect[i].c_str());
    result[vect.size()] = nullptr;
    return result;
}

int main() {
    int std_cin = dup(STDIN_FILENO); // store std::in value
    int std_cout = dup(STDOUT_FILENO); // store std::out value

    while (true) {  // loop to prompt user for commands
        dup2(std_cin, STDIN_FILENO);
        dup2(std_cout, STDOUT_FILENO);

        string line{};
        getline(cin, line);

        // cout << "Line received:\t" << line << "\n";
        if (line == "exit") {
            cout << "Exit terminal.\n";
            break;
        } else if (line.empty()) {
            cout << "Received empty line.\n";
            break;
        }

        Tokenizer tkn(line);
        if (tkn.hasError()) {
            cout << "Tokenizer encountered an error. Continuing to the next input\n";
            continue;
        }

        for (size_t i = 0; i < tkn.commands.size(); ++i) {
            const Command& cmd = *tkn.commands[i];

            int fd[2];
            if (pipe(fd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(2);
            }

            if (!pid) {
                if (i < tkn.commands.size() - 1)
                    dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);

                char** command = vec_to_char_array(cmd.args);
                if (execvp(command[0], command) < 0) {
                    perror("execvp");
                    exit(2);
                }
                delete[] command;
            } else {
                cout << "PID:\t" << pid << "\n";
                dup2(fd[0], STDIN_FILENO);
                close(fd[1]);
            }
        }
    }

    // Reset the input and output file descriptors of the parent.
    dup2(std_cin, STDIN_FILENO);
    dup2(std_cout, STDOUT_FILENO);

    return 0;
}
