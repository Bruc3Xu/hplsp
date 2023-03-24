#include "Debug.h"
#include "EventLoop.h"
#include <iostream>
#include <unistd.h>


nb::EventLoop* g_loop;

void ThreadFunc() {
  g_loop->Loop();
}

int main() {
  nb::LogManager::Instance().Start();
  nb::internal::InitDebugLog(LogLevel::logINFO);
  std::cout << "main pid: " << static_cast<int>(getpid())
            << " tid: " << std::this_thread::get_id() << std::endl;
  nb::EventLoop loop;
  g_loop = &loop;
  std::thread thread(ThreadFunc);
  thread.join();
  nb::LogManager::Instance().Stop();
  return 0;
}