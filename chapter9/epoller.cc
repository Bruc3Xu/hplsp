#include "channel.h"
#include "epoller.h"
#include <cstdlib>
#include <sys/epoll.h>


constexpr int kEventInitNum = 16;
constexpr int kTimeoutMs = 1000;

EPoller::EPoller(): epfd_(::epoll_create1(EPOLL_CLOEXEC)), events_(kEventInitNum){
  if(epfd_ < 0){

    //error
    exit(0);
  }
}

int EPoller::wait(int timeout_ms, std::vector<Channel> &channels){
  int events_num = ::epoll_wait(epfd_, events_.data(), events_.size(), kTimeoutMs);
  if(events_num > 0){
    for (int i; i<events_num; ++i) {
      Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
      channel->SetREvents(events_[i].events);
      channels.push_back(*channel);
    }
    if(events_num >= static_cast<int>(0.8 * events_.size())) {
      events_.resize(events_.size() * 2);
    }
  } else if(events_num == 0){

  } else{
    //error
  }
  return 0;
}