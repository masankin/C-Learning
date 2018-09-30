/*
 * file.cpp
 *
 *  Created on: 2016年9月1日
 *      Author: masankin
 */

#include "file.h"
#include "file_2.h"
string getValue(){
  stringstream s;
  MyStruct myStruct;
  s << "hello world" << myStruct.name;
  return s.str();
}


string getName(){
  return "Bill";
}

