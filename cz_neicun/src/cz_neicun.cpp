//============================================================================
// Name        : cz_内存管理.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

#if 0
int main() {

  int *p = new int(200);
  string *ps = new string("china");

  struct Stu {
    int age;
    string name;
  };

  Stu *pStu = new Stu;

  cout << pStu->age << endl;  // prints !!!Hello World!!!
  return 0;
}
#endif

/**交换成功  交换的是实参的值
 * 交换后 注意
 * 地址并没有变化  而是地址指向的值被修改了
 * 
 * 同时注意 temp 作为临时变量
 *  int temp = *x;  temp指向的地址不是x的地址
 *  
 *  原理 ：传入的形参的地址起始就是实参的地址  在保证地址不变的情况下 对地址所指向的值进行修改
 * */
void swap(int *x,int *y){
  cout << "作为形参，x地址" << x << "\ty地址" << y << endl;
  cout << "作为形参，x指针指向的地址" << &x << "\ty地址" << &y << endl;
  int temp = *x; //为temp 赋值
  int *tempp = &temp;
  cout << "tempp指向的地址是：" << tempp << endl;

  cout << "将x值复制temp 后，temp指向的地址是：" << &temp << endl;
  *x = *y;
  cout<< "将x值修改 后，x指向的地址是" << x  << "\ttemp指向的地址是：" << &temp  << endl;
  *y = temp;
  cout  << "将y值修改 后，y指向的地址是" << y << "\ttemp指向的地址是：" << &temp << endl;
  cout << "交换后，x地址" << x << "\ty地址" << y << endl;
  cout << *x << *y << endl;
}
/**交换失败  交换的是形参的地址
 * 交换后 注意 
 *  
 * */
void swap2(int *x,int *y){
  cout << "作为形参，x地址" << x << "\ty地址" << y << endl;
  int *temp = x;
  
  cout << "将x值复制temp 后，temp指向的地址是：" << temp << endl;
  x = y;
  cout<< "将x值修改 后，x指向的地址是" << x  << "\ttemp指向的地址是：" << temp  << endl;
  y = temp;
  cout  << "将y值修改 后，y指向的地址是" << y << "\ttemp指向的地址是：" << temp << endl;
  cout << "交换后，x地址" << x << "\ty地址" << y << endl;
  cout << *x << *y << endl;
}

/**交换失败  交换的是形参的值
 * 交换后 注意 
 *  
 * */
void swap3(int x,int y){
  cout << "作为形参，x地址" << &x << "\ty地址" << &y << endl;
  int temp = x;
  cout << "将x值复制temp 后，temp指向的地址是：" << &temp << endl;
  x = y;
  cout<< "将x值修改 后，x指向的地址是" << &x  << "\ttemp指向的地址是：" << &temp  << endl;
  y = temp;
  cout  << "将y值修改 后，y指向的地址是" << &y << "\ttemp指向的地址是：" << &temp << endl;
  cout << "交换后，x地址" << &x << "\ty地址" << &y << endl;
  cout << x << y << endl;
}
/**交换成功
 * 交换后 注意 
 *  
 * */
void swap4(int &x,int &y){
  cout << "作为形参，x地址" << &x << "\ty地址" << &y << endl;
  int temp = x;
  cout << "将x值复制temp 后，temp指向的地址是：" << &temp << endl;
  x = y;
  cout<< "将x值修改 后，x指向的地址是" << &x  << "\ttemp指向的地址是：" << &temp  << endl;
  y = temp;
  cout  << "将y值修改 后，y指向的地址是" << &y << "\ttemp指向的地址是：" << &temp << endl;
  cout << "交换后，x地址" << &x << "\ty地址" << &y << endl;
  cout << x << y << endl;
}

int main() {

  char *p = new char[4];
  strcpy(p, "china");
  cout << p << endl;

  //  定义指针保存首地址   
  int *pi = new int[5] { 0 };
  memset(pi, 0, sizeof(int[5]));
  for (int i = 0; i < 5; i++) {
    cout << pi[i] << endl;
  }

  char **ppc = new char*[5] { NULL };

  delete[] ppc;
//##########################
    int num = 8;
  int errNumb = 0;
  int * const curErr = &errNumb;//指针是一个常量  从右向左 读作 常量指针指向int（非常量） 
//  curErr = &num;//curErr 将一直指向 &errNumb  但值可以修改
  
  
  int const *curErr2 = &errNumb;// 从右向左 读作 指针常量指向int（非常量）
  const int *curErr3 = &errNumb;

  //指向的地址可以修改 指向的值也就被修改的  但是不能通过 *curErr2 = 3 来修改 只能通过重新赋一个新的地址  
  //因为const 修饰 *curErr2  所以 *curErr2 不能修改
  curErr2 = &num;
  
  //所以指向的值可以修改,因为int前没有加限定符const
  *curErr = 1;//正确 不变的是指针本身的值 而不是指针指向的那个值
  cout << *curErr << endl;
  cout << *curErr2 << endl;
   

//  *curErr = &num;//错误  只能再直线其他的地址
  
  
  const double pie = 3.14159;
  const double *const piep = &pie;//常量指针指向 double型常量（双精度浮点型常量）
  //因此指针不可以指向其他的地方
  
  
  int a = 1,b = 2;
  cout << &a << "  " << &b << endl;
  int *ap = &a;
  cout << "\t" << ap << endl;
  swap4(a,b);
  cout << "交换：" << a << b << endl;
  cout << "\t" << ap << endl;
  
  int x = 1;
  int *xp = &x;
  cout <<  "\t\t" <<xp <<  endl;
  int y = *xp;;
  cout << "\t\t" << &y <<  endl;
  cout << "\t\t" << &y <<  endl;
  return 0;
}
