//============================================================================
// Name        : cyzz_指针常量.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
extern "C" int test(int a){
}
int main() {
  
  int const p = 1;//整形常量
  const int pp = 2;
  
  //
  int const *pi = &p;
  const int *ppi = &pp;
   
//  *pi = 2; //错误 pi指针所指向的对象是一个整形常量  还不能被修改
//  *ppi = 2;
  ppi = &p;
  ppi = pi;//同上  正确 ppi指针原来指向 pp 的地址 ，此处指向pi所指向的地址
   
  
  typedef char *pstring;
  const pstring ctr = 0;
  const pstring *ps;
  
  const char *ctrs = 0;
//	cout << ">>>" << endl;  
//	cout << *ctr << endl;  
//	cout << ctrs << endl;  
//	
//	cout << ">>>" << endl;  
	
	const int *cp = &p;//底层const
//	*cp = 4; //值不可修改
	cp = &pp;//指向地址可修改
	
	constexpr int cpe = 1;
	constexpr int *cpep = nullptr;
	*cpep = 5;
	cout << cpep << endl;  
	return 0;
}
