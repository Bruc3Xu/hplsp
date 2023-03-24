#include "thrdpool.h"
#include <stdio.h>
#include <unistd.h>

void my_routine(void *context) {
  // 我们要执行的函数
  sleep(1);
  printf("task-%llu start.\n", reinterpret_cast<unsigned long long>(context));
}

void my_pending(const struct Task *task) {
  // 线程池销毁后，没执行的任务会到这里
  printf("pending task-%llu.\n",
         reinterpret_cast<unsigned long long>(task->context));
}

int main() {
  thrdpool_t *thrd_pool = thrdpool_create(3, 1024); // 创建
  struct Task task;
  unsigned long long i;

  for (i = 0; i < 10; i++) {
    task.routine = &my_routine;
    task.context = reinterpret_cast<void *>(i);
    thrdpool_schedule(&task, thrd_pool); // 调用
  }
  getchar();                                // 卡住主线程，按回车继续
  thrdpool_destroy(&my_pending, thrd_pool); // 结束
  return 0;
}