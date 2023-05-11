#ifndef INET_ADDR_H_
#define INET_ADDR_H_
#include <arpa/inet.h>
#include <memory>


namespace nb {
    class InetAddr{
public:
    explicit InetAddr(const char* ip, uint16_t port);
    explicit InetAddr(const struct sockaddr_in& addr);
    struct sockaddr* GetAddr() const{return (struct sockaddr*)&addr_;}

private:
    struct sockaddr_in addr_;
    };
}



#endif