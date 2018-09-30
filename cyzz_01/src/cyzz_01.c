/*
 ============================================================================
 Name        : cyzz_01.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int const p = 1;  //整形常量
  const int pp = 2;

  int const *pi = &p;
  const int *ppi = &pp;

  //  *pi = 2; //错误 pi指针所指向的对象是一个整形常量  还不能被修改
  //  *ppi = 2;
  ppi = &p;
  ppi = pi;  //同上  正确 ppi指针原来指向 pp 的地址 ，此处指向pi所指向的地址

  int * const pi2 = &p;

  return 0;
}
