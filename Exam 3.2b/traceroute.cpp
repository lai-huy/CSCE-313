#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdlib.h>
#include <netdb.h>

/* IP Header */
struct ipheader {
    unsigned char iph_ihl : 4, iph_ver : 4;           // IP Header length & Version.
    unsigned char iph_tos;                            // Type of service
    unsigned short int iph_len;                       // IP Packet length (Both data and header)
    unsigned short int iph_ident;                     // Identification
    unsigned short int iph_flag : 3, iph_offset : 13; // Flags and Fragmentation offset
    unsigned char iph_ttl;                            // Time to Live
    unsigned char iph_protocol;                       // Type of the upper-level protocol
    unsigned short int iph_chksum;                    // IP datagram checksum
    struct in_addr iph_sourceip;                      // IP Source address (In network byte order)
    struct in_addr iph_destip;                        // IP Destination address (In network byte order)
};

/* ICMP Header */
struct icmpheader {
    unsigned char icmp_type;        // ICMP message type
    unsigned char icmp_code;        // Error code
    unsigned short int icmp_chksum; // Checksum for ICMP Header and data
    unsigned short int icmp_id;     // Used in echo request/reply to identify request
    unsigned short int icmp_seq;    // Identifies the sequence of echo messages,
                                    // if more than one is sent.
};

#define ICMP_ECHO_REPLY     0
#define ICMP_ECHO_REQUEST   8
#define ICMP_TIME_EXCEEDED  11
#define MAX_HOPS            30
#define MAX_RETRY           3
#define PACKET_LEN          1500

void traceroute(char* dest) {
    // raw sockets require root priviliges: no change necessary
    if (getuid() != 0) {
        perror("requires root privilige");
        exit(-1);
    }

    // open socket: no change necessary
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(-1);
    }

    // dns resolve and get ip for destination: no change necessary
    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    hostent* getip = gethostbyname(dest);
    if (getip == NULL) {
        perror("failed gethostbyname");
        exit(-1);
    }
    memcpy((char*)(&addr.sin_addr), getip->h_addr, getip->h_length);

    printf("traceroute to %s (%s), %d hops max, %ld bytes packets\n", dest, inet_ntoa(addr.sin_addr), MAX_HOPS, sizeof(ipheader) + sizeof(icmpheader));
    
    char send_buf[PACKET_LEN], recv_buf[PACKET_LEN];
    /** TODO: 1
     * Prepare packet
     * a. outgoing packets only contain the icmpheader with type = ICMP_ECHO_REQUEST, code = 0
     * b. ID in the icmpheader should be set to current process id to identify received ICMP packets
     * c. checksum can be set to 0 for this test
     * d. write/copy the header to the send_buf  
     * 
     * HINT:
     * - icmpheader* icmp = (icmpheader*)send_buf;
     * - set header fields with required values: icmp->field = value;
     * */
    
    for (int ttl = 1; ttl <= MAX_HOPS; ) {
        printf("%2d ", ttl);
        /** TODO: 2
         * set the seq in icmpheader to ttl
         * 
         * HINT:
         * similar to TODO 1 HINT, just set the seq
         */
       

        // set ttl to outgoing packets: no need to change
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, (const char*)&ttl, sizeof(ttl)) < 0) {
            perror("setsockopt failed");
            exit(-1);
        }

        int retry = 0;
        while (1) {
            /** TODO: 3
             * send packet using sendto(...)
             * 
             * HINT:
             * - check man page of sendto(...)
             * - ensure we send one icmpheader in the packet
             * 
             */
           
           
            // wait to check if there is data available to receive; need to retry if timeout: no need to change
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            FD_ZERO(&rfd);
            FD_SET(sockfd, &rfd);
            int ret = select(sockfd + 1, &rfd, NULL, NULL, &tv);

            /** TOOD: 4
             * Check if data available to read or timeout
             * a. if ret == 0: timeout --> retry upto MAX_RETRY
             * b. if ret > 0: data available, use recvfrom(...) to read data to recv_buf and process --> see TODO 5 below
             */
            if (ret == 0) {
                // TODO 4.a
            }
            else if (ret > 0) {
                // TODO 4.b
                /** HINT: 
                 * a. check man page of recvfrom, function returns bytes received
                 * b. ensure data is received in recv_buf
                 */
                
                /** TODO: 5
                 * handle received packets based on received bytes
                 * a. if (i) two pairs of ipheader and icmpheader received, (ii) type is TIME_EXCEEDED, (iii) sequence is the same as ttl, and (iv) id is same as pid
                 *      --> print router ip and increment ttl to move on to processing next hop
                 *      NOTE: first pair contains the ipheader and icmpheader created by the router; second pair would contain the original ipheader and icmpheader sent by us
                 *  HINT:
                 *    - check if bytes returned by recvfrom is at least 2 * (sizeof(ipheader) + sizeof(icmpheader))
                 *    - if yes, use the icmpheader from the first pair to detect if the type is ICMP_TYPE_EXCEEDED
                 *    - use the icmpheader from the second pair to match (i) seq == ttl, and (ii) id = pid
                 * 
                 * b. else if (i) type is ECHO_REPLY, and (ii) id is same as pid --> reached destination. Print ip and exit.
                 * HINT:
                 *    - should return only one icmpheader
                 *    - match the type to ICMP_ECHO_REPLY and id to pid
                 * c. otherwise ignore packet
                 * 
                 */
               
                // ----------------
            }
            else {
                perror("select failed");
                exit(-1);
            }
            fflush(stdout);

            /** TODO: 6
             * Check if timed out for MAX_RETRY times; increment ttl to move on to processing next hop
             */
        }
    }
    close(sockfd);
}

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Usage: traceroute <destination hostname>\n");
        exit(-1);
    }
    
    char* dest = argv[1];
    traceroute(dest);

    return 0;
}