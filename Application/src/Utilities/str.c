#include "str.h"

//функция вычисления длины строки
int getStrLenght(char * str){
  int i=0;
    while (str[i++]!=0);
  return i;
}

void u8_mem_cpy( unsigned char *s, unsigned char *d, unsigned int c){
  while (c--) {
    *d++ = *s++;
  }
}