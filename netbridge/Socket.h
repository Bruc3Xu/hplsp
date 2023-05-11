#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/socket.h>
#include "InetAddr.h"
#include "NonCopyable.h"

namespace nb {
class Socket : interval::noncopyable{
public:
  explicit Socket(int fd);
  ~Socket();
  int fd(){return fd_;}
  void Bind(const InetAddr& addr);
  void Bind(const char* ip, uint16_t port);
  void Listen();
  void Accept(InetAddr* peer_addr);
  void SetNonBlock(bool on);
  void SetNoDelay(bool on);
  void SetSndBuf(socklen_t winsize);
  void SetRcvBuf(socklen_t winsize);
  void SetReuseAddr(bool on);
  void SetReusePort(bool on);

private:
  int fd_;
};

} // namespace nb

#endif