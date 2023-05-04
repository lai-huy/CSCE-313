#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <set>

using namespace std;

int main(int argc, char** argv) {
	int n = 1, opt;
	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
		case 'n':
			n = atoi(optarg);
			break;
		}
	}

	/*
	1. TODO: fork n child processes and run p1-helper on each using execvp
		> note: we need to synchronize the child processes to print in the desired order
		> note: sleep(..) does not guarantee desired synchronization
		> note: check "raise" system call
		> note: check "WUNTRACED" flag for "waitpid"
	*/
	char* const args[]{(char*) "./p1-helper", NULL};
	int status{};
	set<pid_t> children{};

	for (int i = 0; i < n; ++i) {
		pid_t pid = fork();
		if (!pid) {
			// Raise the roof
			raise(SIGSTOP);

			// execute the roof
			execvp(args[0], args);
		} else {
			children.insert(pid);
		}
	}

	/*
	2. TODO: print children pids
	*/
	bool first{};
	for (const pid_t& pid : children) {
		if (first)
			cout << " ";
		cout << pid;
		first = true;
	}
	cout << "\n";

	fflush(stdout);             // DO NOT REMOVE: ensures the first line prints all pids

	/*
	3. TODO: signal children with the reverse order of pids
		> note: take a look at "kill" system call
	*/
	for (auto iter = children.crbegin(); iter != children.crend(); ++iter) {
		kill(*iter, SIGCONT);
		waitpid(*iter, &status, WUNTRACED);
	}

	printf("Parent: exiting\n");

	return 0;
}