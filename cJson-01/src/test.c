#include <stdio.h>
#include <string.h>

#define E1 1

#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)

int main() {
  printf("cJSON_False: %d \n", cJSON_False);
  printf("cJSON_True: %d \n", cJSON_True);
  printf("cJSON_NULL: %d \n", cJSON_NULL);
  printf("cJSON_Number: %d \n", cJSON_Number);
  printf("cJSON_String: %d \n", cJSON_String);
  printf("cJSON_Array: %d \n", cJSON_Array);
  printf("cJSON_Object: %d \n", cJSON_Object);

  char *buf1 = "aaabbb", *buf2 = "bbbccc", *buf3 = "ccc";
  int ptr;
  
  /** 
   * 
  【返回值】
  若buf2与buf1的前n个字符相同，则返回0；
  若buf2大于buf1，则返回大于0的值；
  若buf2 若小于buf1，则返回小于0的值。
   * */

 
  ptr = strncmp(buf2, buf1, 3);
  if (ptr > 0)
    printf("buffer2 is greater than buffer1 \t %d \n",ptr);
  else if(ptr < 0)
  printf("buffer2 is less than buffer1\t %d \n",ptr);
 
  ptr = strncmp(buf2, buf3, 3); 
  if (ptr > 0)
    printf("buffer2 is greater than buffer3 \t %d \n",ptr);
  else if(ptr < 0)
  printf("buffer2 is less than buffer3 \t %d \n",ptr);

  return 0;

}
