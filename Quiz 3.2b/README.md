# Quiz 3.2.b: Threads synchonization for a campus library

Write a C++ program that manages books loan for a library.

Consider that the students are threads and the books are shared resources.

The total number of books available is 100 indicated by the global shared variable `books_available`.

The program creates six threads each requiring `quantity` books to be allocated.

After checking out a quantity of books, each thread sleeps for 2 seconds before checking in the books.

Since the books must be shared among the threads, each thread should wait on a conditional variable if the required quantity of books to checkout is not available.

Only 100 books are in circulation, so all the threads cannot run in parallel.

The threads that are unable to checkout their required quantity of books should wait for the other threads to finish and check in the books.

## Sample output

```
Student 0 checked out 90 books.
Student 5 checked out 5 books.
Student 0 checked in 90 books.
Student 3 checked out 50 books.
Student 2 checked out 40 books.
Student 5 checked in 5 books.
Student 3 checked in 50 books.
Student 2 checked in 40 books.
Student 4 checked out 30 books.
Student 4 checked in 30 books.
Student 1 checked out 90 books.
Student 1 checked in 90 books.
```

## Important notes:
1. Usage: `Usage: ./library`
1. Use a mutex and a condition variable for synchronization
1. Your program will be manually tested for correctness with additional test cases including different file types.
1. Your program should compile with no errors and warnings.
