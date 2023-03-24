#ifndef POLLER_H_
#define POLLER_H_
#include <vector>

class Channel;

struct epoll_event;

class EPoller final {
public:
  EPoller();
  ~EPoller() {}
  int wait(int timeout_ms, std::vector<Channel> &channels);
private:
  int epfd_;
  std::vector<epoll_event> events_;
};

#endif