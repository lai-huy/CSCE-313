# Quiz 4.2.a: Packet sniffing using Scapy

Scapy is a Python-based interactive packet manipulation program and library. It can forge or decode packets of a wide number of protocols, send them on the wire, capture them, store or read them using pcap files, match requests and replies, and much more. It is designed to allow fast packet prototyping by using default values that work.

When we sniff packets, we are only interested specific packets. We can do that by setting filters in sniffing. 
Write a Python program that send packets or listen to traffic according to some filters.


## Options to implement
### 1. Create and send packets
Create and send the following multilayer packet using the function `send_pkt()`.

The `send_pkt()` function takes as input:
- The number of packets to be sent
- The number of seconds between each packet sending

**Construct the packet as follow:**

#### Ethernet layer
- Source MAC address: 00:11:22:33:44:55
- Destination MAC address: 55:44:33:22:11:00

#### IP layer
- Source address: 192.168.10.4
- Destination address: 8.8.4.4
- Protocol: TCP
- TTL: 26

#### TCP layer
- Source port: 23
- Destination port: 80

#### Raw payload
- Payload: "RISC-V Education: https://riscvedu.org/"

### 2. Listen to all traffic to `8.8.4.4` for 1 minute
- Sniff all traffic goind to the address 8.8.4.4 for 1 minute.
- Use the function `print_pkt()` to print specific information about the packet sniffed.

### 3. Listen continuously to only ping commands to `8.8.4.4`
- Sniff continuously only the packets from a `ping` command to the address `8.8.4.4`.
- Use the function `print_pkt()` to print specific information about the packet sniffed.

### 4. Listen continuously to only outgoing telnet commands
- Sniff continuously only the packets from a telnet command executed from localhost.
- Use the function `print_pkt()` to print specific information about the packet sniffed.

### Write the function `print_pkt()`
The `print_pkt()` function should print the following packet fields:
- Source IP
- Destination IP
- Protocol number
- TTL
- Length in bytes
- Raw payload (if any)

## Important notes:
1. Usage: `sudo python3 sniffer.py`
1. Refer to the [sample_outputs ](sample_outputs )
1. Your program will be manually tested for correctness with additional test cases.
1. Your program should execute with no errors and warnings.