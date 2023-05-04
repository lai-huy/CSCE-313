#include <vector> // vector, push_back, at
#include <string> // string
#include <iostream> // cin, getline
#include <fstream> // ofstream
#include <unistd.h> // getopt, exit, EXIT_FAILURE
#include <assert.h> // assert
#include <thread> // thread, join
#include <sstream> // stringstream

#include "BoundedBuffer.h" // BoundedBuffer class

#define MAX_MSG_LEN 256

using namespace std;

/************** Helper Function Declarations **************/

void parse_column_names(vector<string>& _colnames, const string& _opt_input);
void write_to_file(const string& _filename, const string& _text, bool _first_input = false);

/************** Thread Function Definitions **************/

// "primary thread will be a UI data entry point"
void ui_thread_function(BoundedBuffer* bb) {
    string line{};
    while (true) {
        cout << "Enter data to add to the file (or type 'Exit' to quit): " << std::flush;
        getline(cin, line);
        if (line == "Exit") {
            cout << "UI thread: received 'Exit', stopping...\n";
            break;
        }
        bb->push((char*) line.c_str(), line.length());
    }
}

// "second thread will be the data processing thread"
// "will open, write to, and close a csv file"
void data_thread_function(BoundedBuffer* bb, string filename, const vector<string>& colnames) {
    // TODO: implement Data Thread Function
    // (NOTE: use "write_to_file" function to write to file)

    // write column names to file
    stringstream header{};
    for (vector<string>::const_iterator iter = colnames.cbegin(); iter != colnames.cend(); ++iter) {
        header << *iter;
        if (iter == colnames.cend() - 1)
            header << "\n";
        else
            header << ", ";
    }
    write_to_file(filename, header.str(), true);

    // process data from buffer
    size_t i = 0;
    while (true) {
        // pop data off buffer
        char buf[MAX_MSG_LEN]{};
        bb->pop(buf, MAX_MSG_LEN);

        // exit if signal is received
        string str(buf);
        if (str == "Exit") {
            cout << "Data thread: received 'Exit', stopping...\n";
            break;
        }

        i = (i + 1) % colnames.size();
        str += i ? ", " : "\n";

        // write data to file
        write_to_file(filename, str, false);
    }
}

/************** Main Function **************/

int main(int argc, char* argv[]) {

    // variables to be used throughout the program
    vector<string> colnames; // column names
    string fname; // filename
    BoundedBuffer* bb = new BoundedBuffer(3); // BoundedBuffer with cap of 3

    // read flags from input
    int opt;
    while ((opt = getopt(argc, argv, "c:f:")) != -1) {
        switch (opt) {
        case 'c': // parse col names into vector "colnames"
            parse_column_names(colnames, optarg);
            break;
        case 'f':
            fname = optarg;
            break;
        default: // invalid input, based on https://linux.die.net/man/3/getopt
            fprintf(stderr, "Usage: %s [-c colnames] [-f filename]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // TODO: instantiate ui and data threads
    thread ui_thread(ui_thread_function, bb);
    thread data_thread(data_thread_function, bb, fname, colnames);

    // TODO: join ui_thread
    ui_thread.join();

    // TODO: "Once the user has entered 'Exit', the main thread will
    // "send a signal through the message queue to stop the data thread"
    char exit[] = {'E', 'x', 'i', 't', '\0'};
    bb->push(exit, 5);

    // TODO: join data thread
    data_thread.join();

    // CLEANUP: delete members on heap
    delete bb;
}

/************** Helper Function Definitions **************/

// function to parse column names into vector
// input: _colnames (vector of column name strings), _opt_input(input from optarg for -c)
void parse_column_names(vector<string>& _colnames, const string& _opt_input) {
    stringstream sstream(_opt_input);
    string tmp;
    while (sstream >> tmp) {
        _colnames.push_back(tmp);
    }
}

// function to append "text" to end of file
// input: filename (name of file), text (text to add to file), first_input (whether or not this is the first input of the file)
void write_to_file(const string& _filename, const string& _text, bool _first_input) {
    // based on https://stackoverflow.com/questions/26084885/appending-to-a-file-with-ofstream
    // open file to either append or clear file
    ofstream ofile;
    if (_first_input)
        ofile.open(_filename);
    else
        ofile.open(_filename, ofstream::app);
    if (!ofile.is_open()) {
        perror("ofstream open");
        exit(-1);
    }

    // sleep for a random period up to 5 seconds
    usleep(rand() % 5000);

    // add data to csv
    ofile << _text;

    // close file
    ofile.close();
}