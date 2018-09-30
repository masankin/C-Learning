//
//  main.cpp
//  float
//
//  Created by 李宁 on 14-8-21.
//  Copyright (c) 2014年 李 宁. All rights reserved.
//

#include <cfloat>
#include <iostream>
using namespace std;

int main(int argc, const char* argv[]) {
  /*
   基础数据类型：浮点类型

   1. 浮点类型有哪些
   2. 浮点类型占用的字节数
   3. 浮点类型值的表达方式
   4. 浮点类型值的取值范围和精度
   5. 浮点类型值之间的比较
   */

  /*
   单精度浮点类型：float
   双精度浮点类型：double
   长双精度浮点类型：long double

   浮点类型占用的字节数
   float：4
   double：8
   long double：16
   */

  cout << "float size:" << sizeof(float) << endl;
  cout << "double size:" << sizeof(double) << endl;
  cout << "long double size:" << sizeof(long double) << endl;

  float value1 = 234.56;
  double value2 = 123.11;
  // 1234500000000000
  //  科学计数法
  // 1.  尾数
  // 2.  E/e
  // 3.  指数
  double value3 = 1234500000000000;
  double value4 = 1.2345E15;

  //  float: +38  -37
  //  double: +308   -307
  //  long double: +4932   -4931

  int f_max = FLT_MAX_10_EXP;
  int d_max = DBL_MAX_10_EXP;
  int ld_max = LDBL_MAX_10_EXP;
  cout << f_max << endl;

  /*
   精度
   float：6到7
   32位：1  8  23

   double：15到16
   long double：18到19
   */

  int f_dig = FLT_DIG;    // 6
  int d_dig = DBL_DIG;    // 15
  int ld_dig = LDBL_DIG;  // 18

  float v1 = 1.1234323432;
  double v2 = 1.123432343234512678;
  printf("%.18f\n", v2);

  float fv1 = 123.4;
  float fv2 = 123.4;
  if (fv1 == fv2) {
    cout << "equal" << endl;
  }

  float f_v1 = 20;
  float f_v2 = 20.3;
  float f_v3 = 20.5;

  double d_v1 = 20;
  double d_v2 = 20.3;
  double d_v3 = 20.5;

  cout << ((f_v1 == d_v1) ? "true" : "false") << endl;
  cout << ((f_v2 == d_v2) ? "true" : "false") << endl;
  cout << ((f_v3 == d_v3) ? "true" : "false") << endl;

  //  1.  输出true，还是false
  //  2.  why
  //  3.  如果让输出false的语句仍然通过比较输出true

  return 0;
}
