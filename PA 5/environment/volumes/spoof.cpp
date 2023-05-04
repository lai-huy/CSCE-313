#include "common.h"

int main() {
    char buffer[PACKET_LEN];
    memset(buffer, 0, PACKET_LEN);

    ipheader* ip = (ipheader*) buffer;
    udpheader* udp = (udpheader*) (buffer + sizeof(ipheader));

    // add data
    char* data = (char*) udp + sizeof(udpheader);
    int data_len = strlen(CLIENT_IP);
    strncpy(data, CLIENT_IP, data_len);

    // create udp header
    udp->udp_sport = htons(CLIENT_PORT);
    udp->udp_dport = htons(SERVER_PORT);
    udp->udp_ulen = htons(sizeof(udpheader) + data_len);
    udp->udp_sum = 0; // checksum will be calculated by kernel

    // create ip header
    ip->iph_ihl = 5; // header length in 4-byte words
    ip->iph_ver = 4; // IPv4
    ip->iph_tos = 0; // Type of Service
    ip->iph_len = htons(sizeof(ipheader) + sizeof(udpheader) + data_len); // total length of IP datagram
    ip->iph_ident = htons(0); // identification
    ip->iph_flag = 0;
    ip->iph_offset = 0;
    ip->iph_ttl = 64; // Time to Live
    ip->iph_protocol = IPPROTO_UDP; // next protocol is UDP
    ip->iph_chksum = 0; // checksum will be calculated by kernel
    ip->iph_sourceip.s_addr = inet_addr(SPOOF_IP); // source IP
    ip->iph_destip.s_addr = inet_addr(SERVER_IP); // destination IP

    // send packet
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    send_raw_ip_packet(ip);

    return 0;
}