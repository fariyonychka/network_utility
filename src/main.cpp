#include "../include/traceroute.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <hostname> <maxHops>\n";
        return 1;
    }
    std::string hostname = argv[1];
    int maxHops = std::stoi(argv[2]);
    traceRoute(hostname, maxHops);
    return 0;
}