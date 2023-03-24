#ifndef _THRDPOOL_H_
#define _THRDPOOL_H_

#include <atomic>
#include <functional>
#include <memory>
#include <stddef.h>
#include "msgqueue.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>


using Func = std::function<void(void *)>;
using ThreadPtr = std::shared_ptr<std::thread>;

struct Task {
  Func routine;
  void *context;
};

class ThreadPool{
public:
  ThreadPool(unsigned int nthreads, size_t stacksize);
  void Start();
  int Schedule(Task task);
  void Destroy(Func pending_callback);

private:
  void Run();

private:
  msgqueue_t *msgqueue;
  size_t nthreads_=0;
  size_t stacksize;
  int tid;
  std::vector<ThreadPtr>threads_;
  
  std::mutex mutex;
  pthread_key_t key;
  std::atomic<bool> finished_ {false};
  std::condition_variable terminate;
  std::atomic<int> join_tid {0};
};


#endif
