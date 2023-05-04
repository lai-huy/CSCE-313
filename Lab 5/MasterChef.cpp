#include "StepList.h"
#include <algorithm>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <time.h>
#include <unistd.h>

using namespace std;

StepList* recipeSteps;
vector<int>* completedSteps;
int completeCount = 0;

void PrintHelp() {
	cout << "Usage: ./MasterChef -i <file>\n\n";
	cout << "--------------------------------------------------------------------------\n";
	cout << "<file>:    "
		<< "csv file with Step, Dependencies, Time (m), Description\n";
	cout << "--------------------------------------------------------------------------\n";
	exit(1);
}

string ProcessArgs(int argc, char** argv) {
	string result = "";
	// print help if odd number of args are provided
	if (argc < 3) {
		PrintHelp();
	}

	while (true) {
		const auto opt = getopt(argc, argv, "i:h");

		if (-1 == opt)
			break;

		switch (opt) {
		case 'i':
			result = std::string(optarg);
			break;
		case 'h': // -h or --help
		default:
			PrintHelp();
			break;
		}
	}

	return result;
}

// Creates and starts a timer given a pointer to the step to start and when it will expire in seconds.
void makeTimer(Step* timerID, int expire) {
	struct sigevent te;
	struct itimerspec its;

	/* Set and enable alarm */
	te.sigev_notify = SIGEV_SIGNAL;
	te.sigev_signo = SIGRTMIN;
	te.sigev_value.sival_ptr = timerID;
	timer_create(CLOCK_REALTIME, &te, &(timerID->t_id));

	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;
	its.it_value.tv_sec = expire;
	its.it_value.tv_nsec = 0;
	timer_settime(timerID->t_id, 0, &its, NULL);
}

static void timerHandler(int /*sig*/, siginfo_t* si, void* /*uc*/) {

	// Retrieve timer pointer from the si->si_value
	Step* comp_item = (Step*) si->si_value.sival_ptr;

	/* TODO This Section - 2 */
	comp_item->PrintComplete();
	completedSteps->push_back(comp_item->id);
	++completeCount;
	/* End Section - 2 */

	raise(SIGUSR1);
}

// Removes the copmleted steps from the dependency list of the step list.
// Utilize the completedSteps vector and the RemoveDependency method.
// To Complete - Section 3
void RemoveDepHandler(int /*sig*/) {
	/* TODO This Section - 3 */
	/* End Section - 3 */
	for (vector<int>::const_iterator iter = completedSteps->cbegin(); iter != completedSteps->cend(); ++iter)
		recipeSteps->RemoveDependency(*iter);
	completedSteps->clear();
}

// Associate the signals to the signal handlers as appropriate
// Continuously check what steps are ready to be run, and start timers for them with makeTimer()
// run until all steps are done.
// To Complete - Section 1
int main(int argc, char** argv) {
	string input_file = ProcessArgs(argc, argv);
	if (input_file.empty()) {
		exit(1);
	}

	// Initialize global variables
	completedSteps = new vector<int>();
	recipeSteps = new StepList(input_file);

	/* Set up signal handler. */
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = timerHandler;
	sigemptyset(&sa.sa_mask);

	/* TODO This Section - 1 */
	if (sigaction(SIGRTMIN, &sa, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}

	// Associate the appropriate handler with the SIGUSR1 signal, for removing dependencies
	if (signal(SIGUSR1, RemoveDepHandler) == SIG_ERR) {
		perror("signal");
		exit(1);
	}
	// Until all steps have been completed, check if steps are ready to be run and create a timer for them if so
	/* End Section - 1 */
	while (completeCount != recipeSteps->Count()) {
		vector<Step*> steps = recipeSteps->GetReadySteps();
		for (vector<Step*>::iterator iter = steps.begin(); iter != steps.end(); ++iter) {
			(*iter)->running = true;
			makeTimer(*iter, (*iter)->duration);
		}
	}
	cout << "Enjoy!" << endl;
	// delete completedSteps;
	// delete recipeSteps;
}