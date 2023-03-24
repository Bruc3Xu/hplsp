#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "NonCopyable.h"

#include <atomic>
#include <thread>

namespace nb {
class EventLoop : interval::noncopyable {
public:
  EventLoop();
  ~EventLoop();
  EventLoop *LoopOfThread();

  bool InLoopThread() { return std::this_thread::get_id() == thread_id_; }

  void Loop();

private:
  std::atomic<bool> running_{false};
  std::thread::id thread_id_;
};
} // namespace nb

#endif