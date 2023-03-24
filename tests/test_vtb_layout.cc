#include <iostream>
using namespace std;

class Parent{
public:
    int a;
    int b;
};

class Child:public Parent{
public:
    virtual void test(){}
    int c;
};

int main() {
    Child c = Child();
    Parent p = Child();
    cout << sizeof(c) << endl;//24
    cout << sizeof(p) << endl;//8  

    Child* cc = new Child();
    Parent* pp = cc;
    cout << cc << endl;//0x7fbe98402a50
    cout << pp << endl;//0x7fbe98402a58  测试环境是64位，所以指针为8个字节。转换之后pp和cc相差一个虚表指针的偏移。
    cout << &(cc->a) << endl;//0x7fbe98402a58
    cout << &(cc->b) << endl;//0x7fbe98402a5c
    cout << &(cc->c) << endl;//0x7fbe98402a60
    return 0;
}