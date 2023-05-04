#include "RoundRobin.h"
#include <getopt.h>
#include <iostream>
// Example is from this website: https://codeyarns.com/2015/01/30/how-to-parse-program-options-in-c-using-getopt_long/
unsigned int quantum = 0;
std::string input_file = "";

void PrintHelp() {

	cout << "Usage: ./schedule -i <file> -q <quantum>\n\n";
	cout << "--------------------------------------------------------------------------\n";
	cout << "<file>:    "
		<< "file with PID, Arrival_Time and Execution Time, separated with a tab\n";
	cout << "--------------------------------------------------------------------------\n";
	cout << "<quantum>: "
		<< "positive integer value\n";
	cout << "--------------------------------------------------------------------------\n";
	cout << "make sure quantum is a positive number\n";
	exit(1);
}

void ProcessArgs(int argc, char** argv) {
	// print help if odd number of args are provided
	if (argc < 5) {
		PrintHelp();
	}

	while (true) {
		const auto opt = getopt(argc, argv, "i:q:h");

		if (-1 == opt)
			break;

		switch (opt) {
		case 'i':
			input_file = std::string(optarg);
			break;
		case 'q':
			quantum = std::stol(optarg);
			break;

		case 'h': // -h or --help
		default:
			PrintHelp();
			break;
		}
	}
}

int main(int argc, char** argv) {
	ProcessArgs(argc, argv);

	RoundRobin rr_scheduler = RoundRobin(input_file, quantum);
	rr_scheduler.schedule_tasks();

	return 0;
}
