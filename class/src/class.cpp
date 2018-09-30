//============================================================================
// Name        : class.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "Person.h"
using namespace std;

int main() {
  Person p = Person(3);
	cout << p.getCode() << endl; // prints !!!Hello World!!!
	return 0;
}
