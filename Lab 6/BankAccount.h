#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H
#include <mutex> // mutex
#include <string> // string
#include <iostream> // cout, endl
#include <unistd.h> // usleep

class BankAccount {

    // variables and helper functions
private:
    // current account balance
    int balance;
    // mutex to be used in transaction_threadsafe
    std::mutex m;

    // constructor and interface functions
public:
    // default constructor
    BankAccount();

    // interface function to perform transactions
    void transaction(int _amount);

    // interface function to perform *threadsafe* transactions
    // "you will need to implement the transaction_threadsafe() function...
    // "using mutexes to ensure proper concurrency"
    void transaction_threadsafe(int _amount);

    // print function for consistency
    void print_balance();

};

#endif