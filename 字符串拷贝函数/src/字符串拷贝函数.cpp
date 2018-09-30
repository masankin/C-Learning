//============================================================================
// Name        : 字符串拷贝函数.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

//返回字符串数组的首地址
const char mystrcpy(char b[], char a[]) {
  int i = 0;
  for (i = 0; *(a + i) != '\0'; i++) {
    *(b + i) = *(a + i);
  }
  return *b;
}

//传递数组就是传递指针
char mystrcpy2(char *p_b, char *p_a) {
  cout << ">>>   " << p_a << endl; //整个数组的地址 
  
   int i = 0;
   for (i = 0; *(p_a + i) != '\0'; i++) {
     *(p_b + i) = *(p_a + i);
   }
  
  
}


int main() {

  char a[] = "123456";
  
  cout << a << endl;  
  cout << &a[0] << endl; 
  cout << &a << endl; //整个数组的地址 
  
  
   int z[] = {1,2,3,4,5};
   cout << "" << endl; 
   cout << z << endl; 
   cout << &z[0] << endl; 
   cout << &z << endl; //整个数组的地址 
  
  char b[64];
  mystrcpy2(b,a);//  取第一个元素的首地址  b,a 数组本身就是首地址

  //b = "2":
  cout << b << endl;  
  return 0;
}
