#include "bastypes.h"
#include "mmach.h"

f32 f32ABS(f32 value);//���������� ������ ����� float
u16 s16ABS(s16 v);//���������� ������ ����� s16
bool GetAbsMax(f32 v, f32 max);//�������� �� ������������ �������� �� ������
bool SignIsEqu (f32 v1, f32 v2);//��������, ���������� ����� ���� ����?
///////////////////////////////////////////////////////////////////////////////
float getMaxValue(float args[], unsigned short len) {
        unsigned int i;
        float max = 0;
        for(i = 0; i < len; i++){
            float f = args[i];
            max = (f >= max)
                  ? f
                  : max;
        }
        return max;
}

//���������� ������ ����� float
f32 f32ABS(f32 value){
  return (value < 0)
         ? value = -value
         : value;
}

//���������� ������ ����� s16
u16 s16ABS(s16 v){
  return (v < 0)
        ? -v
        : (u16)v;
}

//�������� �� ������������ �������� �� ������
//���������� �������� �� ������
//TRUE  ���� v >= max
//FALSE ���� v <  max
bool GetAbsMax(f32 v, f32 max){
  return (f32ABS(v)>= f32ABS(max));
}

//��������, ���������� ����� ���� ����?
//TRUE - ����� �����
//FALSE - ����� ����������
bool SignIsEqu (f32 v1, f32 v2){
  u8 c = 0;
  if (v1 >= 0) c |= (1<<0);
  if (v2 >= 0) c |= (1<<1);
  //0(00) - ��� ���������� ����� ���� "-"
  //3(11) - ��� ���������� ����� ���� "+"
  //1(01) - �� ���������
  //2(10) - .............
  if (c == 0) return TRUE;
  if (c == 3) return TRUE;
  return FALSE;
}