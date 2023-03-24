#include "channel.h"
#include "sys/epoll.h"


void Channel::HandleEvent(){
  if(revents_ & EPOLLERR){
    if(error_callback_){
      error_callback_();
    }
  }
  if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
    if(read_callback_){
      read_callback_(0);
    }
  }
  if(revents_ & (EPOLLOUT)){
    if(write_callback_){
      write_callback_();
    }
  }
}