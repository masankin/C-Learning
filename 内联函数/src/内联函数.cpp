//============================================================================
// Name        : 内联函数.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

//1.内联函数声明必须实现
//2.c++ 编译器会将函数体直接插入到函数调用的地方，因为没有压栈 出栈所以运行快,

inline void printA(){
  int a = 10;
  cout << a << endl; 
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
