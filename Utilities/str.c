#include "str.h"

//������� ���������� ����� ������
int getStrLenght(char * str){
  int i=0;
    while (str[i++]!=0);
  return i;
}