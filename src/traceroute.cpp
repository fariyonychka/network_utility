#include "/home/yustyna/traceroute_project/include/traceroute.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <cstring>
#include <unistd.h>

void traceRoute(const std::string& hostname, int maxHops) {
    int sock=socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sock<0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket created\n";
    close(sock);
}

int calculateChecksum(void* buffer, int length) {
    unsigned short* buf = (unsigned short*)buffer;
    unsigned int sum = 0;
    unsigned short result;
    for(int i=0; i<length/2; i++) {
        sum += buf[i];
    }
    if(length % 2) {
        sum += ((unsigned char*)buffer)[length-1];
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    result = ~sum;
    return result;
}

