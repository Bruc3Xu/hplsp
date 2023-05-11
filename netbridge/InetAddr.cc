#include "InetAddr.h"
#include <cstring>
#include <strings.h>
#include <utility>


namespace nb {
    InetAddr::InetAddr(const char* ip, uint16_t port){
        bzero(&addr_, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(ip);
        addr_.sin_port = htons(port);
    }

    InetAddr::InetAddr(const sockaddr_in& addr){
        ::memcpy(&addr_, &addr, sizeof(addr));
    }
}