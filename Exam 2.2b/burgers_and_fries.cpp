#include <iostream>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>
using namespace std;
#define MAX_THREADS 100

#define BURGER 0
#define FRIES 1
const char* type_names[] = {"BURGER", "FRIES"};
#define pii pair<int, int>

int k;
int num_orders = 0;
int order_count[2] = {0, 0};
int wait[2] = {0, 0};

sem_t sems[2];


// mutex
mutex mtx;

// Do not change
void process_order() {
    sleep(2);
}

void place_order(int type) {
    /**
     *  Add logic for synchronization before order processing
     *  Check if already k orders in process;
     *  if true -> wait (print waiting)
     *  otherwise place this order (print order)
     *  Use type_names[type] to print the order type
     */
    mtx.lock();
    if (num_orders >= k) {
        cout << "Waiting: " << type_names[type] << endl;
        ++wait[type];
        mtx.unlock();
        sem_wait(&sems[type]);
    } else {
        mtx.unlock();
    }
    mtx.lock();
    cout << "Order: " << type_names[type] << endl;
    ++num_orders;
    mtx.unlock();

    process_order(); // Do not remove, simulates preparation

    /**
     *  Add logic for synchronization after order processed
     *  Allow next order of the same type to proceed if there is any waiting;
     *  if not, allow the other type to proceed.
     */
    --num_orders;
    mtx.lock();
    if (type == FRIES && wait[FRIES] >= 1) {
        sem_post(&sems[FRIES]);
        --wait[FRIES];
    } else if (type == BURGER && wait[BURGER] >= 1) {
        sem_post(&sems[BURGER]);
        --wait[BURGER];
    } else if (wait[FRIES] >= 1) {
        sem_post(&sems[FRIES]);
        --wait[FRIES];
    } else if (wait[BURGER] >= 1) {
        sem_post(&sems[BURGER]);
        --wait[BURGER];
    }
    mtx.unlock();
}

int main() {
    // Initialize necessary variables, semaphores etc.
    sem_init(&sems[BURGER], 0, 0);
    sem_init(&sems[FRIES], 0, 0);

    // Read data: done for you, do not change
    pii incoming[MAX_THREADS];
    int _type, _arrival;
    int t;
    cin >> k;
    cin >> t;
    for (int i = 0; i < t; ++i) {
        cin >> _type >> _arrival;
        incoming[i].first = _type;
        incoming[i].second = _arrival;
    }

    // Create threads: done for you, do not change
    thread* threads[MAX_THREADS];
    for (int i = 0; i < t; ++i) {
        _type = incoming[i].first;
        threads[i] = new thread(place_order, _type);
        if (i < t - 1) {
            int _sleep = incoming[i + 1].second - incoming[i].second;
            sleep(_sleep);
        }
    }

    // Join threads: done for you, do not change
    for (int i = 0; i < t; ++i) {
        threads[i]->join();
        delete threads[i];
    }

    // Destroy semaphores
    sem_destroy(&sems[BURGER]);
    sem_destroy(&sems[FRIES]);

    return 0;
}