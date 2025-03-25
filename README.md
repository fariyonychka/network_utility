# Traceroute for Linux(Ubuntu)

This project is an implementation of `traceroute` written in C++ for use on Linux(Ubuntu).  
It allows you to trace the route of packets to a host using ICMP.  

Assembly and installation.  

### **On Ubuntu**  
```sh
git clone https://github.com/fariyonychka/network_utility.git
cd network_utility
make
sudo ./traceroute <hostname> <maxHops>

