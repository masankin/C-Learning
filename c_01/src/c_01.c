/*
 ============================================================================
 Name        : c_01.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const int con_var1 = 3;
  int *b = (int *)&con_var1;
  *b = 5;

  printf("%d", con_var1);
}
