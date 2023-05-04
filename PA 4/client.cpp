#include <fstream>
#include <iostream>
#include <thread>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <utility> 
#include <iostream>

#include "BoundedBuffer.h"
#include "common.h"
#include "Histogram.h"
#include "HistogramCollection.h"
#include "TCPRequestChannel.h"

// ecgno to use for datamsgs
#define EGCNO 1

using namespace std;

void patient_thread_function(int p_number, BoundedBuffer* request_buffer, int n) {
	double ptime = 0.00;
	for (int i = 0; i < n; ++i) {
		datamsg obj(p_number, ptime, EGCNO);
		request_buffer->push((char*) &obj, sizeof(datamsg));
		ptime += 0.004;
	}
}

void file_thread_function(int m, string filename, BoundedBuffer* request_buffer, int64_t fSize) {
	filemsg fm(0, 0);
	int len = sizeof(filemsg) + filename.size() + 1;

	char* buf = new char[len] {};
	memcpy(buf, &fm, sizeof(filemsg));
	strcpy(buf + sizeof(filemsg), filename.c_str());
	string updatedFileName = "received/" + filename;

	FILE* customStream = fopen(updatedFileName.c_str(), "wb");
	fclose(customStream);

	int64_t offset = 0;
	filemsg* file = ((filemsg*) buf);

	while (offset < fSize) {
		int remainingLength = fSize - offset;
		file->length = remainingLength > m ? m : remainingLength;

		request_buffer->push(buf, len);
		file->offset += file->length;
		offset = file->offset;
	}

	delete[] buf;
}

void worker_thread_function(int m, TCPRequestChannel* workerChannel, string fileName, BoundedBuffer* response_buffer, BoundedBuffer* request_buffer) {
	char* buf = new char[m] {};

	while (true) {
		request_buffer->pop(buf, m);
		MESSAGE_TYPE msg = *((MESSAGE_TYPE*) buf);

		if (msg == DATA_MSG) {
			workerChannel->cwrite(buf, sizeof(datamsg));

			double reply{};
			workerChannel->cread(&reply, sizeof(double));

			datamsg patient{*((datamsg*) buf)};
			pair<int, double> p{patient.person, reply};

			response_buffer->push((char*) &p, sizeof(pair<int, double>));
		} else if (msg == FILE_MSG) {
			filemsg file = *((filemsg*) buf);
			string poppedFileName = "received/" + fileName;

			int fileLength = sizeof(file) + fileName.size() + 1;
			workerChannel->cwrite(buf, fileLength);

			char* fileBuf = new char[m] {};
			workerChannel->cread(fileBuf, file.length);
			FILE* openFile = fopen(poppedFileName.c_str(), "r + b");

			fseek(openFile, file.offset, SEEK_SET);
			fwrite(fileBuf, 1, file.length, openFile);
			fclose(openFile);

			delete[] fileBuf;
		} else if (msg == QUIT_MSG) {
			workerChannel->cwrite(buf, sizeof(QUIT_MSG));
			break;
		}
	}

	delete[] buf;
}

void histogram_thread_function(HistogramCollection* hc, BoundedBuffer* response_buffer) {
	pair<int, double> temp{};
	while (true) {
		response_buffer->pop((char*) &temp, sizeof(pair<int, double>));
		if (temp.first == INT32_MIN && temp.second != temp.second)
			break;
		hc->update(temp.first, temp.second);
	}
}


int main(int argc, char* argv[]) {
	int n = 1000;        // default number of requests per "patient"
	int p = 10;          // number of patients [1,15]
	int w = 100;         // default number of worker threads
	int h = 20;          // default number of histogram threads
	int b = 20;          // default capacity of the request buffer (should be changed)
	int m = MAX_MESSAGE; // default capacity of the message buffer
	string f = "";       // name of file to be transferred

	// Address
	string addr = "127.0.0.1";
	string port = "8080";

	// read arguments
	int opt;
	while ((opt = getopt(argc, argv, "a:r:n:p:w:h:b:m:f:")) != -1) {
		switch (opt) {
		case 'a':
			addr = optarg;
			break;
		case 'r':
			port = optarg;
			break;
		case 'n':
			n = atoi(optarg);
			break;
		case 'p':
			p = atoi(optarg);
			break;
		case 'w':
			w = atoi(optarg);
			break;
		case 'h':
			h = atoi(optarg);
			break;
		case 'b':
			b = atoi(optarg);
			break;
		case 'm':
			m = atoi(optarg);
			break;
		case 'f':
			f = optarg;
			break;
		}
	}

	// // fork and exec the server
	// int pid = fork();
	// if (pid == 0) {
	// 	execl("./server", "./server", "-m", (char*) to_string(m).c_str(), nullptr);
	// }

	// initialize overhead (including the control channel)
	TCPRequestChannel* chan = new TCPRequestChannel(addr, port);
	BoundedBuffer request_buffer(b);
	BoundedBuffer response_buffer(b);
	HistogramCollection hc;

	vector<thread> producerVec;         	// array of producer threads (if data, p elements; if file, 1 element)
	vector<TCPRequestChannel*> workerChan;	// array of FIFOs (w elements)
	vector<thread> workerVec;           	// array of worker threads (w elements)
	vector<thread> histVec;            		// array of histogram threads (if data, h elements; if file, zero elements)

	// making histograms and adding to collection
	for (int i = 0; i < p; ++i) {
		Histogram* h = new Histogram(10, -2.0, 2.0);
		hc.add(h);
	}

	// record start time
	struct timeval start, end;
	gettimeofday(&start, 0);

	/* create all threads here */
	if (f.empty()) { // data
		for (int i = 1; i <= p; ++i)
			producerVec.push_back(thread(patient_thread_function, i, &request_buffer, n));
		for (int i = 0; i < h; ++i)
			histVec.push_back(thread(histogram_thread_function, &hc, &response_buffer));
	} else {
		filemsg fm(0, 0);

		int len = sizeof(filemsg) + f.size() + 1;

		// Retrieves FileSize
		char* buf2 = new char[len] {};
		memcpy(buf2, &fm, sizeof(filemsg));
		strcpy(buf2 + sizeof(filemsg), f.c_str());
		chan->cwrite(buf2, len);

		int64_t fsize = 0;
		chan->cread(&fsize, sizeof(int64_t));
		producerVec.push_back(thread(file_thread_function, m, f, &request_buffer, fsize));

		delete[] buf2;
	}

	for (int i = 0; i < w; ++i) {
		TCPRequestChannel* chan = new TCPRequestChannel(addr, port);
		workerChan.push_back(chan);
		workerVec.push_back(thread(worker_thread_function, m, chan, f, &response_buffer, &request_buffer));
	}

	/* join all threads here */
	for (thread& t : producerVec)
		t.join();


	MESSAGE_TYPE quitMessage = QUIT_MSG;
	for (size_t i = 0; i < workerChan.size(); ++i)
		request_buffer.push((char*) &quitMessage, sizeof(quitMessage));

	for (thread& t : workerVec)
		t.join();

	pair<int, double> quit(INT32_MIN, 0.0 / 0.0);
	for (size_t i = 0; i < histVec.size(); ++i)
		response_buffer.push((char*) &quit, sizeof(std::pair<int, double>));

	for (thread& t : histVec)
		t.join();

	// record end time
	gettimeofday(&end, 0);

	// print the results
	if (f == "") {
		hc.print();
	}
	int secs = ((1e6 * end.tv_sec - 1e6 * start.tv_sec) + (end.tv_usec - start.tv_usec)) / ((int) 1e6);
	int usecs = (int) ((1e6 * end.tv_sec - 1e6 * start.tv_sec) + (end.tv_usec - start.tv_usec)) % ((int) 1e6);
	cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;

	// quit and close all channels in FIFO array
	for (TCPRequestChannel*& chan : workerChan) {
		chan->cwrite(&m, sizeof(MESSAGE_TYPE));
		delete chan;
	}

	// quit and close control channel
	MESSAGE_TYPE q = QUIT_MSG;
	chan->cwrite((char*) &q, sizeof(MESSAGE_TYPE));
	cout << "All Done!" << endl;
	delete chan;

	// wait for server to exit
	wait(nullptr);
}
