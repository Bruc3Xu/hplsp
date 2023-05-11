#include "Socket.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

namespace nb {
Socket::Socket(int fd) : fd_(fd) {}
Socket::~Socket() { ::close(fd_); }

void Socket::Bind(const InetAddr &addr) {
  if (::bind(fd_, addr.GetAddr(),
             static_cast<socklen_t>(sizeof(struct sockaddr_in))) == -1) {
    exit(1);
  }
}

void Socket::Bind(const char *ip, uint16_t port) {
  InetAddr addr(ip, port);
  Bind(addr);
}

void Socket::Listen() {
  if (::listen(fd_, 64) == -1) {
    exit(1);
  }
}

void Socket::Accept(InetAddr *peer_addr) {
  socklen_t addr_len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
  if (::accept(fd_, peer_addr->GetAddr(), &addr_len) == -1) {
    exit(1);
  }
}

void Socket::SetNonBlock(bool on) {
  int flag = ::fcntl(fd_, F_GETFL, 0);
  if (on) {
    ::fcntl(fd_, F_SETFL, flag | O_NONBLOCK);
  } else {
    ::fcntl(fd_, F_SETFL, flag | ~O_NONBLOCK);
  }
}
void Socket::SetNoDelay(bool on) {
  int opt = 1 ? on : 0;
  ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, (void *)&opt,
               sizeof(opt));
  ;
}
void Socket::SetSndBuf(socklen_t winsize) {
  ::setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, (void *)&winsize, sizeof(winsize));
}
void Socket::SetRcvBuf(socklen_t winsize) {
  ::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (void *)&winsize, sizeof(winsize));
}

void Socket::SetReuseAddr(bool on) {
  int opt = 1 ? on : 0;
  ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
}
void Socket::SetReusePort(bool on) {
  int opt = 1 ? on : 0;
  ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, (void *)&opt, sizeof(opt));
}
} // namespace nb