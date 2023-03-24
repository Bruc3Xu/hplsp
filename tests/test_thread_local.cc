#include <iostream>
#include <threadpool>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

pid_t gettid()
{
  return static_cast<pid_t>(syscall(SYS_gettid));
}

#ifdef THREAD_LOCAL
#pragma message "`thread_local`"
thread_local int data = 2;
#else
#pragma message "`__thread`"
__thread int data = 2;
#endif

int main()
{
  std::mutex mtx;
  auto func = [&](const int x)
  {
    std::lock_guard<std::mutex> lck(mtx);
    data *= x;
    fprintf(stderr, "tid: %d\t data: %d\n", gettid(), data);
  };
  std::threadpool pool;
  pool.add_task(func, 2);
  pool.add_task(func, 4);
  pool.add_task(func, 5);
  pool.add_task(func, 6);
  fprintf(stderr, "main: %d\t data: %d\n", gettid(), data);
}
