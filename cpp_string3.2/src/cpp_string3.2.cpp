//============================================================================
// Name        : 2.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <string>
#include <iostream>

using std::string;
using std::cin;
using std::cout;
using std::endl;


int main_01(int argc, char **argv) {

//  int s1;
//  cin >> s1;
//  cout << s1;
 
  string s1;//默认初始化  s1是一个空字符串
  string s2 = s1;//s2是s1的副本
  string s3 = "abc";//s3是该字符串字面值的副本
  string s4(10,'4'); //直接初始化  不使用等于号
  //如果使用=号 表示的是拷贝初始化  编译器把等号右侧的值拷贝到新创建的对象中去
  
  cout << s4;
  
  
  return 0;
}

int main_02(int argc, char **argv) {
 
  string word;
  while(cin >> word){
    cout << word << endl;
  }
  return 0;
}

int main_03(int argc, char **argv) {
 
  string line;
  while(getline(cin,line)){
    if(line.empty()){

       cout << line <<  " empty" << endl;
       continue;
    }
    cout << line << endl;
  }
  return 0;
}

//两个string对象相加
int main(int argc, char **argv) {
 
  string s1 = "hello : ,",s2 = " world";
 
  string s3 = s1  +  s2;
  cout << s3 << endl;
  decltype(s3.size()) punct_cnt = 0;
  for (auto &c : s3) {
    if(ispunct(c)){
      ++punct_cnt;
      cout << c << " 标点" << endl;
    }
    c = toupper(c);
    cout << c << endl;
  }
  cout << s3 << endl;

  return 0;
}
