#include "str.h"

//функция вычисления длины строки
int getStrLenght(char * str){
  int i=0;
    while (str[i++]!=0);
  return i;
}