#include "/home/yustyna/traceroute_project/include/traceroute.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <chrono>

#define SIZE_OF_PACKET 64

// Функція для обчислення контрольної суми ICMP
uint16_t calculateChecksum(void* buffer, int length) {
    uint16_t* buf = (uint16_t*)buffer;
    uint32_t sum = 0;
    
    while (length > 1) {
        sum += *buf++;
        length -= 2;
    }
    
    if (length == 1) {
        sum += *(uint8_t*)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

// Функція для трасування маршруту
void traceRoute(const std::string& hostname, int maxHops) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // тайм-аут для recvfrom
    struct timeval timeout{};
    timeout.tv_sec = 1;  
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in dest {};
    struct addrinfo hints {}, *res;

    hints.ai_family = AF_INET;  

    if (getaddrinfo(hostname.c_str(), nullptr, &hints, &res) != 0) {
        perror("Error resolving hostname");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
    memcpy(&dest.sin_addr, &addr->sin_addr, sizeof(addr->sin_addr));
    dest.sin_family = AF_INET;
    freeaddrinfo(res);

    std::cout << "Tracing route to " << hostname << " (" << inet_ntoa(dest.sin_addr) << ")\n";

    for (int ttl = 1; ttl <= maxHops; ttl++) {
        // TTL для пакету
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0) {
            perror("setsockopt failed");
            exit(EXIT_FAILURE);
        }

        // ICMP-пакет
        char packet[SIZE_OF_PACKET] = {0};
        struct icmphdr* icmpHeader = (struct icmphdr*)packet;
        icmpHeader->type = ICMP_ECHO;
        icmpHeader->code = 0;
        icmpHeader->un.echo.id = getpid();
        icmpHeader->un.echo.sequence = ttl;
        icmpHeader->checksum = 0;
        icmpHeader->checksum = calculateChecksum(packet, SIZE_OF_PACKET);

        
        auto start = std::chrono::high_resolution_clock::now();
        sendto(sock, packet, SIZE_OF_PACKET, 0, (struct sockaddr*)&dest, sizeof(dest));

        
        char buffer[SIZE_OF_PACKET] = {0};
        struct sockaddr_in reply{};
        socklen_t replyLength = sizeof(reply);
        int bytesReceived = recvfrom(sock, buffer, SIZE_OF_PACKET, 0, (struct sockaddr*)&reply, &replyLength);
        auto end = std::chrono::high_resolution_clock::now();

        double rtt = std::chrono::duration<double, std::milli>(end - start).count();

        if (bytesReceived > 0) {
            struct iphdr* ipHeader = (struct iphdr*)buffer;
            struct icmphdr* icmpReply = (struct icmphdr*)(buffer + (ipHeader->ihl * 4));

            if (icmpReply->type == ICMP_TIME_EXCEEDED || icmpReply->type == ICMP_ECHOREPLY) {
                struct hostent* hopHost = gethostbyaddr(&reply.sin_addr, sizeof(reply.sin_addr), AF_INET);
                std::cout << ttl << ". " << inet_ntoa(reply.sin_addr) << " ("
                          << (hopHost ? hopHost->h_name : "Unknown") << ") "
                          << "RTT: " << rtt << " ms\n";

                if (reply.sin_addr.s_addr == dest.sin_addr.s_addr) break;  // Досягнуто пункту призначення
            }
        } else {
            std::cout << ttl << ". * * * (Timeout)\n";
        }
    }
    close(sock);
}