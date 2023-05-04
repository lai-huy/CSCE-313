#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
// include additional necessary headers

using std::mutex, std::condition_variable;
using std::unique_lock, std::thread;
using std::cout;

mutex mtx;
condition_variable cv;
bool syn_ready = true;
int count;

void query() {
	// Should print: the print number (starting from 0), "SYN", and the three dots "..."

	for (int i = 0; i < count; ++i) {
		unique_lock<mutex> lock(mtx);
		cv.wait(lock, [] { return syn_ready; });
		syn_ready = false;
		cout << "[" << i << "] SYN ... ";
		cv.notify_one();
	}
}

void response() {
	// Should print "ACK"

	for (int i = 0; i < count; ++i) {
		unique_lock<mutex> lock(mtx);
		cv.wait(lock, [] { return !syn_ready; });
		syn_ready = true;
		cout << "ACK\n";
		cv.notify_one();
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: ./handshake <count>\n");
		exit(1);
	}

	/**
	 * Steps to follow:
	 * 1. Get the "count" from cmd args
	 * 2. Create necessary synchronization primitive(s)
	 * 3. Create two threads, one for "SYN" and the other for "ACK"
	 * 4. Provide the threads with necessary args
	 * 5. Update the "query" and "response" functions to synchronize the output
	 */

	count = std::stoi(argv[1]);

	thread t1(query);
	thread t2(response);

	t1.join();
	t2.join();

	return 0;
}