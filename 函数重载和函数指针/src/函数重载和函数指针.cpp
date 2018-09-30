//============================================================================
// Name        : 函数重载和函数指针.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

//声明一个函数类型 
typedef void mytypeDefFun(int a);

//声明一个函数指针类型 
typedef void (*mytypeDefFun2)(int a);

//声明一个函数指针变量 
void (*mytypeDefFun3)(int a);


int main() {
  
  mytypeDefFun2 *pFun = mytypeDefFun;
	cout << (*pFun)(12)  << endl; // prints !!!Hello World!!!
	return 0;
} 

void  mytypeDefFun4(int p){
  cout << p  << endl;
}
