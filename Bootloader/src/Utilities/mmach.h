#ifndef MMACH_H
#define MMACH_H
#include "bastypes.h"

  #ifdef __cplusplus
    extern "C" f32 f32ABS(f32 value);//���������� ������ ����� float
    extern "C" u16 s16ABS(s16 v);//���������� ������ ����� s16
    extern "C" bool GetAbsMax(f32 v, f32 max);//�������� �� ������������ �������� �� ������
    extern "C" bool SignIsEqu (f32 v1, f32 v2);//��������, ���������� ����� ���� ����?
    extern "C" float getMaxValue(float args[], unsigned short len);
  #else
    extern f32 f32ABS(f32 value);//���������� ������ �����
    extern u16 s16ABS(s16 v);//���������� ������ ����� s16
    extern bool GetAbsMax(f32 v, f32 max);//�������� �� ������������ �������� �� ������
    extern bool SignIsEqu (f32 v1, f32 v2);//��������, ���������� ����� ���� ����?
    extern float getMaxValue(float args[], unsigned short len);
  #endif
    
#endif 