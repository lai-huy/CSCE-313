#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;

// Total number of books available (the shared resource)
int books_available = 100;

// Mutex for locking
mutex mtx;

// Conditional variable for the allocating threads to wait upon
condition_variable checkout_ready;

/* TODO:
 1. acquire the lock
 2. wait on the checkout_ready conditional variable if <books_available> is less than <quantity>
 3. after acquiring the lock, decrement <books_available> by <quantity>
 4. release the lock
*/
void books_checkout(int quantity) {
    unique_lock<mutex> lock(mtx);
    while (books_available < quantity)
        checkout_ready.wait(lock);
    // checkout_ready.wait(lock, [quantity] {return books_available >= quantity;});
    books_available -= quantity;
    lock.unlock();
}

/* TODO:
 1. acquire the lock
 2. increment <books_available> with the <quantity>
 3. release the lock
 4. wakeup threads waiting on the books_checkout() function
 */
void books_checkin(int quantity) {
    unique_lock<mutex> lock(mtx);
    books_available += quantity;
    lock.unlock();
    checkout_ready.notify_all();
}

/**
 * thread function that takes in a thread id and the number of books to checkout and return.
 * the manager function is in charge of checkouts and checkins of books for the threads
 * @param threadId thread id.
 * @param quantity number of books to be allocated.
 */
void manager(int threadId, int quantity) {
    books_checkout(quantity);
    cout << "Student " << threadId << " checked out " << quantity << " books.\n";
    sleep(2); // keep the books for some time
    books_checkin(quantity);
    cout << "Student " << threadId << " checked in " << quantity << " books.\n";
}

int main() {
    // The number of books to checkout by each thread
    int cart[6] = {90, 90, 40, 50, 30, 5};

    // Creating an array of threads
    thread* student_threads = new thread[6]{};

    /*TODO: Create the six student threads that will use the function (manager) for a respective number of books (cart[i])*/

    for (size_t i = 0; i < 6; ++i)
        student_threads[i] = thread(manager, i, cart[i]);

    /*TODO: Wait for all the six threads*/
    for (size_t i = 0; i < 6; ++i)
        student_threads[i].join();

    delete[] student_threads;
    return 0;
}