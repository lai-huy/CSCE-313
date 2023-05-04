# Burgers and Fries
A fast-food restaurant has adopted the following policies:

1. It offers either burger or fries per order.
1. It can process up to k concurrent orders. If already k orders are in progress, incoming customers must wait.
1. The preparation of each order takes exactly 2 seconds.
1. The cooks prefer working on the same type of order that they handled last. So a recently completed order should be replaced by a waiting order of the same type. If such an order does not exist, the cook picks the other type.
For example, if a burger order is complete, and more burger orders are waiting, we pick a burger order even if a fries order has been waiting longer. If there are no more burger orders, we can proceed with the fries order. This example is similar for fries.

Write a program to synchronize the incoming customers. 

## Input
The program first takes in two inputs: 
1. `k`: the number of concurrent orders the shop can process 
1. `t`: the number of incoming customers 

The next t lines of the input each contain a tuple of two integers:
- first number: type of the order [0: BURGER, 1: FRIES]
- second number: arrival time of each incoming customer

### Sample input
```
2 9     // 2 concurrent orders; 9 incoming customers 
0 0     // first customer ordering burger, arriving at timestamp 0
1 0
0 2
1 2
1 5
1 5
0 6
1 6
1 6
```

## Program Structure
The starter code includes reading in the input and launching a thread per customer. Your task is to complete the `place_order(type)` function. More instructions are provided in the comments of the code.

### Restrictions

1. For synchronization primitives, **you are only allowed to use at most one mutex and two semaphores**.
1. You cannot use a spin lock, busy waiting, or `sleep()`. 
1. You cannot use queues. Implicit queueing must be done using mutex/semaphores.
1. Avoid deadlock conditions.
1. No interleaved prints.
1. You must have the capability of processing at most k orders in parallel (i.e., if enough concurrent order exists, the program should not simply process one at a time).


## Output
The output should print the timeline of the order status. If the order is placed, it should print "Order: <type name>". If the order is waiting, it should first print "Waiting: <type name>" and finally "Order: <type name>" when some cook is ready to pick it up.

### Sample output for the sample input shown above
```
Order: BURGER
Order: FRIES
Order: BURGER
Order: FRIES
Order: FRIES
Order: FRIES
Waiting: BURGER
Waiting: FRIES
Waiting: FRIES
Order: FRIES
Order: FRIES
Order: BURGER
```

### Explanation
1. k = 2, so the restaurant can handle two concurrent orders. 
1. Two orders (1 and 2) arrive at time = 0. The orders are placed without delay.
1. Two orders (3 and 4) arrive at time = 2. At this time, both of the previous orders are complete, so orders 3 and 4 can be placed. 
1. Two more orders (5 and 6) arrive at time = 5. At this time, both orders 3 and 4 are complete. 
1. Three more orders arrive at time = 6. At this time, both cooks are occupied, so these orders wait. However, the burger would be the last one to be placed since both fries orders at (5 and 6) are replaced by fries orders (8 and 9) -- even though the burger order is first on the waiting list.

## Testing
1. There are some public test cases under test-files/. 
1. To test your program, run: `./burgers_and_fries < test-files/<filename>`
1. Your program will be manually tested for correctness with additional test cases.
1. Your program should compile with no errors and warnings.
