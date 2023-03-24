#include "EventLoop.h"
#include "Debug.h"
#include <cassert>
#include <sys/poll.h>
namespace nb {

thread_local EventLoop *loop_in_thread_ = nullptr;

EventLoop::EventLoop() : thread_id_(std::this_thread::get_id()) {
  if (loop_in_thread_) {
    NB_ERROR << "already a loop in this thread";
    throw;
  } else {
    NB_INFO << "EventLoop created " << this << " in thread ";
    loop_in_thread_ = this;
  }
}

EventLoop::~EventLoop() {
  assert(!running_);
  loop_in_thread_ = nullptr;
}

EventLoop *EventLoop::LoopOfThread() { return loop_in_thread_; }

void EventLoop::Loop() {
  assert(!running_);
  assert(InLoopThread());
  running_ = true;
  int ret = ::poll(nullptr, 0, 5000);
  NB_INFO << "EventLoop " << this << " stop looping";
  running_ = false;
}

} // namespace nb