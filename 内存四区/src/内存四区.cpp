//============================================================================
// Name        : 内存四区.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;



/**
 
 |------------| 
 |            | 
 |            | 
 |            | 
 |            | 
 |            | 
 |            | 
 |            | 
 |            |              
 |            | 
 |            | 
 |            |
 |            | 
 |            | 
 |------------| 
  
   
 * */



char * getStr1(){
  char *p1 = "1234";
  return p1;
}

char *getStr2(){
  char *p2 = "1234";
  return p2;
}

int main() {
	cout << getStr1() << endl; // prints !!!Hello World!!!
	cout << getStr2() << endl; // prints !!!Hello World!!!
	return 0;
}
