#include <iostream>


class A{
public:
  A() = default;
  A& operator=(const A& a){
    std::cout << "a copy construct" << std::endl;
    if(this == & a){
      return *this;
    }
    this->val = a.val;
    return *this;
  }
private:
  int val;
};

class B{
public:
  B(const A& a) {
    a_ = a;
    std::cout << "in construction" << std::endl;
  }
private:
  A a_;
};

int main()
{
  A a;
  B b(a);
  return 0;
}