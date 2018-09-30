//============================================================================
// Name        : cz_内存管理.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;


#if 0
int main() {
  
  int *p = new int(200);
  string *ps = new string("china");
  
  
  struct Stu {
    int age;
    string name; 
  };
  
  Stu *pStu = new Stu; 
 
  
	cout << pStu->age << endl; // prints !!!Hello World!!!
	return 0;
}
#endif

int main() { 
  
  char *p = new char[4];
  strcpy(p,"china");
  cout << p << endl;  
  
  //  定义指针保存首地址  
  int *pi = new int[5];
  
//  for (int i = 0; i < 5; i++) {
//      cout<< pi[i]+' '+i <<endl;
//  }
  
  
  return 0;
}