#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <tuple>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

#include "Tokenizer.h"

// all the basic colours for a shell prompt
#define RED     "\033[1;31m"
#define GREEN	"\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE	"\033[1;34m"
#define WHITE	"\033[1;37m"
#define NC      "\033[0m"

using namespace std;

/**
 * @brief Get the Time object
 *
 * @return const string current system time as a string
 */
const string getTime() {
    const time_t t = time(0);
    tm* const now = localtime(&t);
    const string curr_time(asctime(now));
    return curr_time.substr(0, curr_time.size() - 1);
}

/**
 * @brief determines the name of the directory to cd into.
 *
 * @param cmd the cd Command
 * @param prev_dir previous directory
 * @return const string name of directory to change to
 */
const string getNewDirectoryName(const Command& cmd, const string& prev_dir) {
    switch (cmd.args.size()) {
    case 1:
        return string(getenv("HOME"));
    case 2:
        return cmd.args.back() == "-" ? prev_dir : cmd.args.back();
    default:
        throw runtime_error("Too many arguments");
    }
}

vector<string> getFileCompletions(const string& prefix) {
    vector<string> result{};
    DIR* dir = opendir(".");
    struct dirent* dirp{};
    while ((dirp = readdir(dir))) {
        const string name = dirp->d_name;
        if (name.find(prefix) == 0)
            result.push_back(name);
    }
    closedir(dir);
    return result;
}

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

/**
 * @brief Redirects std::cin and std::cout if a command has either an input file or an output file
 *
 * @param cmd Command to execute
 */
void redirectCommandIO(const Command& cmd) {
    int fd{};
    if (cmd.hasInput()) {
        fd = open(cmd.in_file.c_str(), O_RDONLY, S_IWUSR | S_IRUSR);
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (cmd.hasOutput()) {
        fd = open(cmd.out_file.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

/**
 * @brief Print the command to std::cerr
 *
 * @param cmd Command to print
 */
void printCommand(const Command& cmd) {
    for (const string& str : cmd.args)
        cerr << "|" << str << "| ";
    if (cmd.hasInput())
        cerr << "in< " << cmd.in_file << " ";
    if (cmd.hasOutput())
        cerr << "out> " << cmd.out_file << " ";
    cerr << "\n";
}

/**
 * @brief Kill all background processes.
 *
 * @param bgs vector of background pid
 */
void wait4Background(const vector<int>& bgs) {
    int status{};
    for (vector<pid_t>::const_iterator iter = bgs.begin(); iter != bgs.end(); ++iter) {
        waitpid(*iter, &status, WNOHANG);
        if (status) {
            cerr << "Process with PID " << *iter << " ended\n";
            kill(*iter, SIGKILL);
        }
    }
}

/**
 * @brief Execute an inputed command
 *
 * @param cmd command to execute
 * @param last_command a boolean, true when the last command is being executed
 * @param bgs a vector of background pids
 */
void executeCommand(const Command& cmd, const bool& last_command, vector<pid_t>& bgs) {
    // Pipe
    int fds[2]{};
    if (pipe(fds) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    // fork to create child
    pid_t pid = fork();
    if (pid < 0) {  // error check
        perror("fork failed");
        exit(2);
    }

    if (!pid) {
        // IO redirection
        redirectCommandIO(cmd);

        // Redirect cout on everything except last command
        if (!last_command) {
            dup2(fds[1], STDOUT_FILENO);
            close(fds[1]);
        }

        // Execute command
        char** args = vec_to_char_array(cmd.args);
        if (execvp(args[0], args) < 0) {  // error check
            perror("command not found");
            exit(2);
        }
        delete[] args;
    } else { // Handle the parent
        if (cmd.isBackground()) {
            cerr << "Background process with PID " << pid << ".\n";
            bgs.push_back(pid);
        } else if (last_command)
            waitpid(pid, 0, 0);
        else {
            int status{};
            waitpid(pid, &status, WNOHANG);
            if (status) {
                cerr << "Exit with status:\t" << status << "\n";
                exit(status);
            }
        }

        dup2(fds[0], STDIN_FILENO);
        close(fds[1]);
    }
}

/**
 * @brief Runs all the commands within a vector
 *
 * @param cmds a vector of commands to run
 * @param curr_dir the current directory to execute in
 * @param prev_dir the previous directory most recently visited
 * @param bgs a vector of background processes
 */
void runCommands(const vector<Command*>& cmds, string& curr_dir, string& prev_dir, vector<pid_t>& bgs) {
    char buff[PATH_MAX]{};
    for (vector<Command*>::const_iterator iter = cmds.begin(); iter != cmds.end(); ++iter) {
        const Command& cmd = **iter;
        // printCommand(cmd);

        // handle cd
        if (cmd.args.front() == "cd") {
            try {
                const string dir = getNewDirectoryName(cmd, prev_dir);
                if (dir.empty()) {
                    cerr << "cd: OLDPWD not set\n";
                    return;
                }

                cerr << "Change Directory.\n";
                chdir(dir.c_str());
                prev_dir = curr_dir;
                curr_dir = getcwd(buff, sizeof(buff));
            } catch (const runtime_error& err) {
                cerr << "cd: too many arguments\n";
            }

            return;
        }

        executeCommand(cmd, iter == cmds.cend() - 1, bgs);
    }
}

/**
 * @brief Process $USER, $PWD, etc
 *
 * @param input string holding the inputed line from the user
 * @return tuple<string, size_t, size_t> result of the $ replacement, start of the $ var name, end of the $ var name.
 */
tuple<string, size_t, size_t> processReplacement(const string& input) {
    size_t start = input.find('$');
    size_t end = start + 1;
    if (start == string::npos)
        throw invalid_argument("No $ to replace");
    if (!isupper(input[end]))
        throw invalid_argument("Invalid $ var name");
    string varName{};
    while (end < input.size() && input[end] != ' ')
        varName += input[end++];
    return tuple<string, size_t, size_t>(string(getenv(varName.c_str())), start, end);
}

/**
 * @brief Process $ expantion
 *
 * @param input string holding the inputed line from the user
 * @return tuple<string, size_t, size_t> result of the $ expantion, start of the $ expantion, end of the $ expantion
 */
tuple<string, size_t, size_t> processExpantion(const string& input) {
    size_t begin = input.find("$(");
    if (begin == string::npos)
        throw invalid_argument("Command does not contain $(");

    size_t num_paranthesis = 1;
    size_t end = begin + 2;
    while (end < input.size()) {
        if (input[end++] == '$' && input[end] == '(')
            ++num_paranthesis;
        if (input[end] == ')')
            --num_paranthesis;

        if (!num_paranthesis)
            break;
    }

    string command = input.substr(begin + 2, end - begin - 2);
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
        throw runtime_error("Failed to execute command");

    char buffer[1024]{};
    string result{};
    while (fgets(buffer, sizeof(buffer), pipe))
        result += buffer;

    result = result.substr(0, result.size() - 1);

    pclose(pipe);
    return tuple<string, size_t, size_t>{result, begin, end};
}

int main() {
    int std_cin = dup(STDIN_FILENO); // store std::in value
    int std_cout = dup(STDOUT_FILENO); // store std::out value

    // get current system time
    const string curr_time = getTime();

    // get user
    const string user(getenv("USER"));

    // Get current path
    char buff[PATH_MAX]{};
    string curr_dir(getcwd(buff, sizeof(buff)));
    string prev_dir{};

    // vector of background processes
    vector<pid_t> bgs{};

    while (true) {
        // wait for background processes to finish
        wait4Background(bgs);

        // Copy std::cin and std::cout
        dup2(std_cin, STDIN_FILENO);
        dup2(std_cout, STDOUT_FILENO);

        // prompt
        cout << curr_time << " " << GREEN << user << " " << BLUE << curr_dir << NC << "$ ";

        // get user inputted command
        string input{};
        getline(cin, input);

        // print exit message and break out of infinite loop
        if (input == "exit") {
            cout << RED << "Now exiting shell...\nGoodbye" << NC << "\n";
            break;
        } else if (input.empty())
            continue;
        else if (input.size() == 3) {
            switch (input[2]) {
            case 'A':
                cerr << "Up\n";
                break;
            case 'B':
                cerr << "Down\n";
                break;
            case 'C':
                cerr << "Right\n";
                break;
            case 'D':
                cerr << "Left\n";
                break;
            default:
                break;
            }
        }

        try {
            tuple<string, size_t, size_t> t = processReplacement(input);
            const string start = input.substr(0, get<1>(t));
            const string end = get<2>(t) + 1 >= input.size() ? string() : input.substr(get<2>(t) + 1);
            const string result = get<0>(t);
            input = start + result + end;
        } catch (const invalid_argument& err) {

        }

        try {
            tuple<string, size_t, size_t> t = processExpantion(input);
            const string start = input.substr(0, get<1>(t));
            const string end = get<2>(t) + 1 >= input.size() ? string() : input.substr(get<2>(t) + 1);
            const string result = get<0>(t);
            input = start + result + end;
        } catch (const invalid_argument& err) {

        }

        Tokenizer tknr(input);
        if (tknr.hasError())  // continue to next prompt if input had an error
            continue;

        runCommands(tknr.commands, curr_dir, prev_dir, bgs);
    }

    // Reset the input and output file descriptors of the parent.
    dup2(std_cin, STDIN_FILENO);
    dup2(std_cout, STDOUT_FILENO);
}