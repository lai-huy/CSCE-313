#include "RoundRobin.h"

/*
This is a constructor for RoundRobin Scheduler, you should use the extractProcessInfo function first
to load process information to process_info and then sort process by arrival time;

Also initialize time_quantum
*/
RoundRobin::RoundRobin(string file, int time_quantum): time_quantum{time_quantum}, processVec{queue<Process>{}}, processQ{queue<Process>{}} { this->extractProcessInfo(file); }

// Schedule tasks based on RoundRobin Rule
// the jobs are put in the order the arrived
// Make sure you print out the information like we put in the document
void RoundRobin::schedule_tasks() {
	int systime{};

	while (!processVec.empty() || !processQ.empty()) {
		// Forced waiting
		if (processQ.empty()) {
			Process proc = processVec.front();
			while (proc.get_arrival_time() > systime)
				print(systime++, -1, false);
			processQ.push(proc);
			processVec.pop();
		}

		// Execute earliest process
		Process proc = processQ.front();
		processQ.pop();
		proc.Run(0);
		for (int i = 0; i < this->time_quantum; ++i) {
			if (proc.is_Completed())
				break;
			print(systime++, proc.getPid(), proc.is_Completed());
			proc.Run(1);
		}

		// Push new processes to the process Q
		while (!processVec.empty() && processVec.front().get_arrival_time() <= systime) {
			processQ.push(processVec.front());
			processVec.pop();
		}

		if (!proc.is_Completed()) {
			processQ.push(proc);
		} else
			this->print(systime, proc.getPid(), proc.is_Completed());
	}
}

/***************************
ALL FUNCTIONS UNDER THIS LINE ARE COMPLETED FOR YOU
You can modify them if you'd like, though :)
***************************/

// Default constructor
RoundRobin::RoundRobin(): time_quantum{0}, processVec{queue<Process>{}}, processQ{queue<Process>{}} {}

// Time quantum setter
void RoundRobin::set_time_quantum(int quantum) { this->time_quantum = quantum; }

// Time quantum getter
int RoundRobin::get_time_quantum() { return this->time_quantum; }

// Print function for outputting system time as part of the schedule tasks function
void RoundRobin::print(int system_time, int pid, bool isComplete) {
	string s_pid = pid == -1 ? "NOP" : to_string(pid);
	cout << "System Time [" << system_time << "].........Process[PID=" << s_pid << "] ";
	if (isComplete)
		cout << "finished its job!\n";
	else
		cout << "is Running\n";
}

// Read a process file to extract process information
// All content goes to proces_info vector
void RoundRobin::extractProcessInfo(string file) {
	// open file
	ifstream processFile(file);
	if (!processFile.is_open()) {
		perror("could not open file");
		exit(1);
	}

	// read contents and populate process_info vector
	string curr_line, temp_num;
	int curr_pid, curr_arrival_time, curr_burst_time;
	while (getline(processFile, curr_line)) {
		// use string stream to seperate by comma
		stringstream ss(curr_line);
		getline(ss, temp_num, ',');
		curr_pid = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_arrival_time = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_burst_time = stoi(temp_num);

		processVec.push(Process(curr_pid, curr_arrival_time, curr_burst_time));
	}

	// close file
	processFile.close();
}