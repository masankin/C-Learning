//============================================================================
// Name        : 析构函数.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

class Test {
 public:
  //构造函数
  Test() {
    p = (char *) malloc(100);
    strcpy(p, "abc");

    cout << "无参数构造函数" << endl;
  }
  Test(int _a) {
    a = _a;
    cout << "有参数构造函数" << endl;
  }

  Test(const Test& t) {
    cout << "copy构造函数" << endl;
    cout <<  "t->a :" <<  t.a << endl;
    cout <<  "a :" <<  a << endl;
  }

  //析构函数
  ~Test() {
//    if (p != NULL) {
//      free(p);
//    }
    cout << "析构函数被调用了" << endl;
  }

  void printp() {
    cout << p << endl;
  }
 
  int a;
  char *p;

} ;

void testLifeCircle() {
  Test t1;  // 先创建的对象后释放  创建时先调用t1 的构造函数  ／  释放时先调用t2的析构函数

//  t1.printp();
  Test t2(1);
  Test t2_1 = (1);  //c++  对＝进行了功能性增强
  Test t3;
  cout <<  &t3 << endl;
  cout <<  t3.a << endl;
}

void initTest(Test t){
  
}




Test g(){
  Test v = Test(4);
  return v;
}

//用匿名对象来初始化同类型的对象  匿名对象会转成实名对象
void objplay3(){
  Test r = g();
  cout << "-------objplay3--------" << endl;
}

//将匿名对象赋值给同类型的对象  匿名对象会被立即析构
void objplay4(){
  Test r(4);
  r = g();
  cout << "-------objplay3--------" << endl;
}

int main() {
  cout << "---------------" << endl;
 // testLifeCircle();
  cout << "---------------" << endl;
  //手动调用构造函数 
//  Test t2_1 = Test(1);//调用匿名函数完成t2_1  的初始化 ,只会调用Test(1)这个有参构造函数,不会调用 Test t2_1;
//  cout << "--------initTest-------" << endl;
//  initTest(t2_1);
  cout << "-------copyConTest--------" << endl;
  objplay4();
  cout << "-------copyConTest/e--------" << endl;
  return 0;
}
//
//---------------
//---------------
//-------copyConTest--------
//有参数构造函数
//-------objplay3--------
//析构函数被调用了
//-------copyConTest/e
