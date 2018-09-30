//============================================================================
// Name        : constant.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

int main() {
  
  const int code = 1234;
//  code = 4;
  
//  const int abc; 常量必须初始化
  const int *p = new int;//常量指针通常是在定义时初始化
  cout << *p << endl;
//  *p = 333;
  
  char str[10] = "hellp";
   char *p_str = str;
  *p_str = '2';//修改了第一个字母
  cout << str << endl;
  
  
  //指针常量
  char * const p_str1 = str;
  char const * p_str2 = str;
  *p_str1 = 'w';
  cout << str << endl;
  
//  p_str1 = new char[3];//p_str1 指向的地址可以修改，只是其本身是一个常量指针，不能再指向新的地址
    
	return 0;
}
