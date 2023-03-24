#include <iostream>
using namespace std;

class Parent {
    public:
    virtual void virtualFunc() = 0;
    void helper() {
        virtualFunc();
    }
    virtual ~Parent(){
        // helper();
        // virtualFunc();
    }
};

class Child : public Parent{
    public:
    void virtualFunc() {
        cout << "Child" << endl;
    }
    virtual ~Child(){}
};


int main() {

    // Child child;
    Parent* child = new Child();
    child->virtualFunc();
    Parent* p = child;
    delete child;
    p->virtualFunc();
    delete p;
    return 0;
}
