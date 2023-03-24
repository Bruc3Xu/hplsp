#include <iostream>
#include <thread>
#include <vector>

#include "Macros.h"

DECLARE_SINGLETON(my_singleton);

std::mutex g_mutex;

void foo() {
  auto p = my_singleton::GetInstance();
  std::lock_guard<std::mutex> lock(g_mutex);
  std::cout << "thread: " << std::this_thread::get_id() << " ,address is " << p
            << std::endl;
}

int main() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 3; ++i) {
    std::thread t(foo);
    threads.push_back(std::move(t));
  }
  for (int i = 0; i < threads.size(); ++i) {
    threads[i].join();
  }
  return 0;
}