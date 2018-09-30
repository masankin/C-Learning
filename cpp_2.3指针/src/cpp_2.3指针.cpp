//============================================================================
// Name        : 3指针.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
using namespace std;

#define N_VALUES = 5


size_t strlen(char *str){
  int length = 0;
  char *p = str;
  cout << *p << endl; // prints !!!Hello World!!!

//  while(*str++){
//    cout << *str << endl; // prints !!!Hello World!!!
////    cout << (str-p-1) << endl; // prints !!!Hello World!!!
//
//  }
  
  while(*str++ != '\0'){
      cout << *str << endl; // prints !!!Hello World!!!
  //    cout << (str-p-1) << endl; // prints !!!Hello World!!!

  }
      cout << ("cocos2d-x"+2)<< endl; // prints !!!Hello World!!!

}
#if 0
int main() {
  /*
   指针本身也是一个对象
   允许对指针进行复制和拷贝
   在指针的生命周期内可以先后指向不同的对象
   在块作用域内 如果没有被初始化 将会指向一个不确定的值
   * 
   * */
  
  
  int *ip1,*ip2;
  
  int iVal = 4;
  int *p = &iVal; //
  char str[6] = "hello";
  strlen(str);
  
 	return 0;
}
#endif


//指针运算
int main() {
  float values[N_VALUES];
  float *vp;
  for (vp = &values[0]; vp < &values[N_VALUES];) {
    cout <<  *vp << endl; // prints !!!Hello World!!!
  }  
  
  return 0;
  
}