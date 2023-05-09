# Traceroute

[Traceroute](https://www.geeksforgeeks.org/traceroute-command-in-linux-with-examples/?ref=gcse) tracks the route packets take from an IP network to a given host. It utilizes the IP protocol's time to live (`TTL`) field and attempts to elicit an `ICMP TIME_EXCEEDED` response from each gateway along the path to the host. Below is an example output of a `traceroute` execution. Please see the [manpage](https://linux.die.net/man/8/traceroute) for more details.

### Example
```
traceroute -q 1 facebook.com
traceroute to facebook.com (157.240.254.35), 30 hops max, 60 byte packets
 1  *
 2  *
 3  po131.asw01.ord3.tfbnw.net (157.240.33.164)  11.784 ms
 4  157.240.39.157 (157.240.39.157)  12.870 ms
 5  157.240.36.109 (157.240.36.109)  12.029 ms
 6  edge-star-mini-shv-02-ord5.facebook.com (157.240.254.35)  12.819 ms
```

Each line prints the hop distance followed by the name and ip of the router at that hop. An `*` is printed for timeout or no response.
Implement a simplified version of `traceroute` using ICMP packets. When routers receive such packets, they decrement the `TTL` and forward to the next router if it is larger than zero. If zero/expired at some router, it will send back an ICMP message with `ICMP_TIME_EXCEEDED` type, followed by the original message that we sent to the router in the following format:

```
|-- router ipheader --|--router icmpheader--|--original ipheader--|--original icmpheader--|
```

1. `router ipheader`: contains routers information/ip
1. `router icmpheader`: contains response message with type (e.g., `ICMP_TIME_EXCEEDED`)
1. `original ipheader` + `original icmpheader`: the original icmp message sent by us to the router.

If the message is from our target host and not from some intermediate routers, the response would only contain `router ipheader` + `router icmpheader` with type `ICMP_ECHO_REPLY`.

Using this knowledge you can implement `traceroute` as follows:

```
for ttl=1 to MAX_HOPS:
    retry = 0
    while true:
        prepare ICMP packet (TODO 1)
        set ICMP message sequence = ttl (TODO 2)
        send ICMP message (TODO 3)
        wait to check if data ready
        if timeout:
            (TODO 4a)
            resend message 
            retry++
        else if data ready:
            recv response (TODO 4b)
            if response is ICMP_TIME_EXCEEDED and response->seq == ttl:
                (TODO 5a)
                print router ip
                ttl++
                break
            else if response is ICMP_ECHO_REPLY:
                (TODO 5b)
                print router ip 
                exit()
    if retry == MAX_RETRY:
        (TODO 6)
        ttl++
        break       
```

The outline of the algorithm is given in `traceroute.cpp`. You are only required to fill-in some missing segments. More specific details are provided in the comments of the starter code.


## Input
The program takes a single input: 
1. `hostname`: the destination hostname to trace the route for

### Sample input
1. `./traceroute facebook.com`
1. `./traceroute yahoo.com`
1. `./traceroute google.com`

## Output
Each line should print for each hop distance until either (i) reached destination, or (ii) reached `MAX_HOPS`. If there is a valid response (`ICMP_TIME_EXCEEDED` or `ICMP_ECHO_REPLY`), each line should print the sender router ip. For timeouts, it should just print an `*` per attempt.

### Sample output
1. ```
    traceroute to facebook.com (157.240.254.35), 30 hops max, 28 bytes packets
    1 * * * 
    2 157.240.33.174
    3 129.134.33.223
    4 157.240.39.193
    5 157.240.254.35
    ```

1. ```
    traceroute to yahoo.com (98.137.11.163), 30 hops max, 28 bytes packets
    1 * * * 
    2 216.115.110.162
    3 209.191.64.79
    4 209.191.64.238
    5 66.196.67.117
    6 98.136.158.175
    7 98.136.159.242
    8 98.136.158.193
    9 98.137.11.163
    ```

Note that the first line of output is already given in the starter code. For a successful trace, the final line of print must match the resolved ip in the parentheses of the first output line. Also, there may be cases where we have an unsuccessful trace, in which case we show `MAX_HOPS` lines of output:

1. ```
    traceroute to google.com (173.194.194.138), 30 hops max, 28 bytes packets
     1 * * * 
     2 172.253.64.58
     3 142.251.236.152
     4 216.239.41.191
     5 216.239.58.139
     6 * * * 
     7 * * * 
     8 * * * 
     9 * * * 
    10 * * *
    11 * * *
    12 * * *
    13 * * *
    14 * * *
    15 * * *
    16 * * *
    17 * * *
    18 * * *
    19 * * *
    20 * * *
    21 * * *
    22 * * *
    23 * * *
    24 * * *
    25 * * *
    26 * * *
    27 * * *
    28 * * *
    29 * * *
    30 * * *
    ```


## Additional Notes

1. **All communation must be through ICMP messages**.
1. Using raw sockets require root priviliges. Make sure you test the program as root; otherwise we would get `permission denied` in `sendto(...)`. 
1. Based on the resolved IP by the DNS, we can expect to get different output for different runs of the same test case.
1. Your program will be manually tested for correctness.
1. Your program should compile with no errors and warnings.
