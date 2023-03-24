#include "queue.hpp"
#include <iostream>
#include <string>


int main()
{
  Queue<std::string> q(100);
  q.Put("hello");
  std::string out;
  q.Get(out);
  std::cout << out << std::endl;
  return 0;
}