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