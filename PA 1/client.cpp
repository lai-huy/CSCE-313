/*
	Original author of the starter code
	Tanzir Ahmed
	Department of Computer Science & Engineering
	Texas A&M University
	Date: 2/8/20

	Please include your Name, UIN, and the date below
	Name: Huy Lai
	UIN:  132000359
	Date: 06 February 2023
*/
#include "common.h"
#include "FIFORequestChannel.h"
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

/**
 * @brief Request a single ecg value
 *
 * @param chan FIFORequestChannel to use to request the data from the server
 * @param person the id of the person to request
 * @param time the time value to request
 * @param ecg the ecg number to request
 */
void requestSinglePoint(FIFORequestChannel& chan, int person, double time, int ecg) {
	cout << "Requested single data point\n";
	char buf[MAX_MESSAGE];
	datamsg x(person, time, ecg);

	memcpy(buf, &x, sizeof(datamsg));
	chan.cwrite(buf, sizeof(datamsg)); // question
	double reply;
	chan.cread(&reply, sizeof(double)); //answer
	cout << "For person " << person << ", at time " << time << ", the value of ecg " << ecg << " is " << reply << "\n";
}

/**
 * @brief Request 1000 points from a person
 *
 * @param chan FIFORequestChannel to use to reqest the data from the server
 * @param person the id of the person to request
 */
void requestMultiple(FIFORequestChannel& chan, int person) {
	cout << "Requested multiple data points\n";
	const string outFile = "./received/x1.csv";
	ofstream fout(outFile, ios::out);

	char buf1[MAX_MESSAGE]{};
	char buf2[MAX_MESSAGE]{};
	double time{};
	double reply1{};
	double reply2{};

	auto start = high_resolution_clock::now();

	for (size_t i = 0; i < 1000; ++i) {
		time = 0.004 * i;

		// Request ECG 1 data point
		datamsg ecg1(person, time, 1);
		memcpy(buf1, &ecg1, sizeof(datamsg));
		chan.cwrite(buf1, sizeof(datamsg));
		chan.cread(&reply1, sizeof(double));

		// Request ECG 2 data point
		datamsg ecg2(person, time, 2);
		memcpy(buf2, &ecg2, sizeof(datamsg));
		chan.cwrite(buf2, sizeof(datamsg));
		chan.cread(&reply2, sizeof(double));

		// Write response to ofstream
		fout << time << "," << reply1 << "," << reply2 << "\n";
	}

	auto end = high_resolution_clock::now();
	auto time_taken = duration_cast<nanoseconds>(end - start);

	cout << "Time taken by program is : " << time_taken.count() << " ns\n";
	cout << "Output File:\t" << outFile << "\n";
	fout.close();
}

/**
 * @brief Request a file from the server
 *
 * @param chan FIFORequestChannel to use to request the server
 * @param filename name of the file to request
 * @param buf_cap the maximum size of the buffer
 */
void requestFile(FIFORequestChannel chan, const string& filename, int buf_cap) {
	filemsg msg(0, 0);
	size_t len = sizeof(filemsg) + filename.size() + 1;
	char* buf = new char[len] {};
	memcpy(buf, &msg, sizeof(filemsg));
	strcpy(buf + sizeof(filemsg), filename.c_str());
	chan.cwrite(buf, len);

	__int64_t filesize{};
	chan.cread(&filesize, sizeof(__int64_t));
	cout << "File length is: " << filesize << " bytes\n";
	cout << "Buf Cap:\t" << buf_cap << "\n";

	__int64_t numBlocks = ceil(double(filesize) / buf_cap);
	__int64_t last_count = filesize % buf_cap;
	if (!last_count)
		last_count = buf_cap;

	filemsg* file_req = (filemsg*) buf;
	file_req->offset = 0;
	file_req->length = numBlocks == 1 ? filesize : buf_cap;

	chan.cwrite(buf, len);
	char* response = new char[file_req->length] {};
	chan.cread(response, file_req->length);

	const string outFile = "./received/" + filename;
	ofstream fout(outFile, ios::out);
	fout.write(response, file_req->length);


	auto start = high_resolution_clock::now();
	for (__int64_t i = 1; i < numBlocks; ++i) {
		if (i == numBlocks - 1) {
			file_req->length = last_count;
			delete[] response;
			response = new char[file_req->length] {};
		}

		file_req->offset += buf_cap;
		chan.cwrite(buf, len);
		chan.cread(response, file_req->length);
		fout.write(response, file_req->length);
	}

	auto end = high_resolution_clock::now();
	auto time_taken = duration_cast<nanoseconds>(end - start);

	cout << "File Transfer took:\t" << time_taken.count() << " ns\n";

	delete[] buf;
	delete[] response;
}

int main(int argc, char* argv[]) {
	int opt{};
	int person = -1;
	double time = -1;
	int ecg = -1;
	int buf_cap = MAX_MESSAGE;

	bool time_flag{};
	bool file_flag{};
	bool chan_flag{};
	bool buff_flag{};

	string filename{};
	while ((opt = getopt(argc, argv, "p:t:e:f:cm:")) != -1) {
		switch (opt) {
		case 'p':
			person = atoi(optarg);
			break;
		case 't':
			time = atof(optarg);
			time_flag = true;
			break;
		case 'e':
			ecg = atoi(optarg);
			break;
		case 'f':
			filename = optarg;
			file_flag = true;
			break;
		case 'm':
			buf_cap = atoi(optarg);
			buff_flag = true;
			break;
		case 'c':
			chan_flag = true;
			break;
		}
	}

	if (!fork()) {
		cout << "Fork!\n";
		char cmd[] = {"./server"};
		char msg[] = {"-m"};
		string s = to_string(buf_cap);
		char* size = const_cast<char*>(s.c_str());
		if (buff_flag) {
			char* args[] = {cmd, msg, size, NULL};
			execvp(args[0], args);
		} else {
			char* args[] = {cmd, NULL};
			execvp(args[0], args);
		}
	}

	FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);

	if (chan_flag) {
		MESSAGE_TYPE new_chan = NEWCHANNEL_MSG;
		chan.cwrite(&new_chan, sizeof(MESSAGE_TYPE));
		char response[30]{};
		chan.cread(response, sizeof(response));
		string channel_name = response;
		cout << "New Channel:\t" << channel_name << "\n";

		FIFORequestChannel new_channel = FIFORequestChannel(channel_name, FIFORequestChannel::CLIENT_SIDE);

		if (time_flag)
			requestSinglePoint(new_channel, person, time, ecg);
		else if (!time_flag && !file_flag)
			requestMultiple(new_channel, person);
		else if (!time_flag && file_flag)
			requestFile(new_channel, filename, buf_cap);

		MESSAGE_TYPE quit = QUIT_MSG;
		new_channel.cwrite(&quit, sizeof(MESSAGE_TYPE));
	} else {
		if (time_flag)
			requestSinglePoint(chan, person, time, ecg);
		else if (!time_flag && !file_flag)
			requestMultiple(chan, person);
		else if (!time_flag && file_flag)
			requestFile(chan, filename, buf_cap);
	}

	// closing the channel    
	MESSAGE_TYPE quit = QUIT_MSG;
	chan.cwrite(&quit, sizeof(MESSAGE_TYPE));

	return 0;
}