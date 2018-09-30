//============================================================================
// Name        : 内存检索.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

int main() {
  
  
  int a = 10;
  cout << "a:"  <<  &a << endl;
  
  void *p1 = (void *)&a;
  void *p2 = (void *)&a;

  
  
  for (char *p = (char *)p1; p != p2; p++) {
    int *px = (int *)p;
    if (*px == 1) {
      cout << px << endl;
    }
    cout << "px:"  <<  px << endl;
  }
  cout << "a:"  <<  &a << endl;
     
}
