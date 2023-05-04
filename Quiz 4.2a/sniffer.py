from scapy.all import *
from scapy.layers.inet import Ether, IP, TCP


def main():
    """Driver function"""
    while True:
        print_menu()
        option = input('Choose a menu option: ')
        if option == '1':
            print("Creating and sending packets ...")
            number = int(input("How many packets? "))
            interval = int(input("How many seconds in between sending? "))
            send_pkt(number, interval)
        elif option == '2':
            print("Listening to all traffic to 8.8.4.4 for 1 minute ...")
            sniff(filter="host 8.8.4.4", prn=print_pkt, timeout=60)
        elif option == '3':
            print("Listening continuously to only ping commands to 8.8.4.4 ...")
            sniff(filter="icmp and host 8.8.4.4", prn=print_pkt)
        elif option == '4':
            print("Listening continuously to only outgoing telnet commands ...")
            sniff(filter="tcp", prn=print_pkt)
        elif option == '5':
            print("End")
            break
        else:
            print(f"\nInvalid entry\n")


def send_pkt(number, interval):
    """
    Send a custom packet with the following fields

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
    """

    src_mac = "00:11:22:33:44:55"
    dst_mac = "55:44:33:22:11:00"
    src_ip = "192.168.10.4"
    dst_ip = "8.8.4.4"
    ttl = 26
    src_port = 23
    dst_port = 80
    payload = "RISC-V Education: https://riscvedu.org/"

    # create the packet layers
    ether = Ether(src=src_mac, dst=dst_mac)
    ip = IP(src=src_ip, dst=dst_ip, ttl=ttl, proto=6)
    tcp = TCP(sport=src_port, dport=dst_port)

    # stack the layers and create the packet
    packet = ether/ip/tcp/payload

    # send the packet repeatedly
    sendp(packet, inter=interval, count=number)


def print_pkt(packet):
    """ 
    Print Packet fields

    - Source IP
    - Destination IP
    - Protocol number
    - TTL
    - Length in bytes
    - Raw payload (if any)
    """

    # TODO

    if IP in packet:
        ip_layer = packet[IP]
        print(f"Source IP: {ip_layer.src}")
        print(f"Destination IP: {ip_layer.dst}")
        print(f"Protocol number: {ip_layer.proto}")
        print(f"TTL: {ip_layer.ttl}")
        print(f"Length in bytes: {len(packet)}")
    if Raw in packet:
        raw_layer = packet[Raw]
        print(f"Raw payload: {raw_layer.load}")

    pass


def print_menu():
    """Prints the menu of options"""
    print("*******************Main Menu*******************")
    print('1. Create and send packets')
    print('2. Listen to all traffic to 8.8.4.4 for 1 minute')
    print('3. Listen continuously to only ping commands to 8.8.4.4')
    print('4. Listen continuously to only outgoing telnet commands')
    print('5. Quit')
    print('***********************************************\n')


main()
