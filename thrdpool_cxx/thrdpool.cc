#include "thrdpool.h"
#include <cstddef>
#include <memory>
#include <thread>

ThreadPool::ThreadPool(unsigned int nthreads, size_t stacksize) {
  msgqueue = msgqueue_create((size_t)-1, 0);
  if (msgqueue) {
    while (nthreads_ < nthreads) {
      threads_.push_back(std::make_shared<std::thread>(&Run, this));
      nthreads_++;
    }
  }
  
}


void ThreadPool::Run() {
  Task *entry;
  void (*task_routine)(void *);
  void *task_context;
  pthread_t tid;

  while (!finished_) {
    entry = msgqueue_get(msgqueue);
    if (!entry)
      break;

    task_routine = entry->task.routine;
    task_context = entry->task.context;
    free(entry);
    task_routine(task_context);

    if (nthreads_ == 0) {
      /* Thread pool was destroyed by the task. */
      free(pool);
      return NULL;
    }
  }

  /* One thread joins another. Don't need to keep all thread IDs. */
  pthread_mutex_lock(&pool->mutex);
  tid = pool->tid;
  pool->tid = pthread_self();
  if (--pool->nthreads == 0)
    pthread_cond_signal(pool->terminate);

  pthread_mutex_unlock(&pool->mutex);
  if (memcmp(&tid, &__zero_tid, sizeof(pthread_t)) != 0)
    pthread_join(tid, NULL);

  return NULL;
}