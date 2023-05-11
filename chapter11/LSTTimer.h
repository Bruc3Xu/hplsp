#ifndef LST_TIMER_H_
#define LST_TIMER_H_

#include <functional>
#include <netinet/in.h>
#include <time.h>

class util_timer;

struct client_data {
  struct sockaddr_in addr;
  int fd;
  char *buf[64];
  util_timer *timer;
};

class util_timer {
public:
  util_timer() : prev(nullptr), next(nullptr) {}

public:
  time_t expire;
  std::function<void(client_data *)> cb_;
  client_data *data;
  util_timer *prev;
  util_timer *next;
};

class sort_timer_lst{
public:
  sort_timer_lst():head(nullptr), tail(nullptr){

  }
  ~sort_timer_lst(){
    util_timer* tmp = head;
    while(tmp)
    {
      head = tmp->next;
      delete tmp;
      tmp = head;
    }
  }
  void add_timer(util_timer* timer)
  {
    if(!timer)
    {
      return;
    }
    if(!head)
    {
      head = tail = timer;
      return;
    }
    if(timer->expire < head->expire)
    {
      timer->next = head;
      head->prev = timer;
      head = timer;
      return;
    }
    add_timer(timer, head);
  }

  void ajust_timer(util_timer* timer)
  {
    if(!timer)
    {
      return;
    }
    util_timer* tmp = timer->next;
    if(!tmp || timer->expire < tmp->expire){
      return;
    }
    if(timer == head)
    {
      head = head->next;
      head->prev = nullptr;
      timer->next = nullptr;
      add_timer(timer);
    }
    else{
      timer->prev->next = timer->next;
      timer->next->prev = timer->prev;
      add_timer(timer, timer->next);
    }
  }

private:
  void add_timer(util_timer* timer, util_timer* last_head)
  {

  }
private:
  util_timer* head;
  util_timer* tail;
};

#endif
