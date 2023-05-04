#ifndef PROCESS_HEADER
#define PROCESS_HEADER
#include <string>

using namespace std;

// Process Class
class Process {
private:
	int pid;
	int arrival_time;
	int cpu_burst_time;
	int remaining_time;
	bool isCompleted;

public:
	Process();
	Process(int pid, int arrival_time, int cpu_burst_time);
	Process(const Process& p);
	void Run(int run_time);
	int getPid() const;
	bool is_Completed() const;
	int get_remaining_time() const;
	int get_cpu_burst_time() const;
	int get_arrival_time() const;
	void change_arrival_time(int new_arrival_time);
	void update_remaining_time(int updated_remaining_time);
	Process& operator=(const Process& p);
};

#endif