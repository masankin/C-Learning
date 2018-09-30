/*
 * Person.cpp
 *
 *  Created on: 2016年9月7日
 *      Author: masankin
 */

#include "person.h"

void Person::setCode(int code) {
  mCode = code;
}
int Person::getCode() {
   return this->mCode;
}

void Person::setName(string name) {
  mName = name;
}

void Person::setValue(string value){
   this->vlaue = value;//成员变量和形参相同名称时  可使用指针为该成员变量赋值
}
