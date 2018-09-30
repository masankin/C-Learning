//============================================================================
// Name        : 二级指针.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

void pp(char **p){
  *p = 200;
}

int main() {
  char *p1 = NULL;
   
  pp(&p1);
	cout << p1 << endl; // prints !!!Hello World!!!
	return 0;
}
