/*
 * Person.h
 *
 *  Created on: 2016年9月7日
 *      Author: masankin
 */

#ifndef PERSON_H_
#define PERSON_H_


#include <iostream>

using namespace std;

class Person {

private:
  int mCode;
  string mName;
  string vlaue;
public:
  Person(int code){
    mCode = code;
  }
  void setName(string name);
  void setValue(string value);
  void setCode(int code);
  
  int getCode();
  int getName();
  
  //内联方法
  void inlineMethod(){
    
  }
};


#endif /* PERSON_H_ */
