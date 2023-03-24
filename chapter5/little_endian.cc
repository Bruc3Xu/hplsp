#include<iostream>


union test{
	int a;
	char b;
};

int main(){
	test t;
	t.a = 65;
	std::cout << t.a << " " << t.b << std::endl;
	return 0;
}
