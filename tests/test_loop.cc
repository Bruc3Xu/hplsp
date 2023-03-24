#include "Debug.h"
#include "EventLoop.h"
#include <iostream>
#include <unistd.h>

void ThreadFunc() {
  std::cout << "main pid: " << static_cast<int>(getpid())
            << " tid: " << std::this_thread::get_id() << std::endl;
  nb::EventLoop loop;
  loop.Loop();
}

int main() {
  nb::LogManager::Instance().Start();
  nb::internal::InitDebugLog(LogLevel::logINFO);
  std::cout << "main pid: " << static_cast<int>(getpid())
            << " tid: " << std::this_thread::get_id() << std::endl;
  nb::EventLoop loop;
  std::thread thread(ThreadFunc);
  loop.Loop();
  thread.join();
  nb::LogManager::Instance().Stop();

  return 0;
}