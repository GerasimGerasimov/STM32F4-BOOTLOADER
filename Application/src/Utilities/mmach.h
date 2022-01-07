#ifndef MMACH_H
#define MMACH_H
#include "bastypes.h"

  #ifdef __cplusplus
    extern "C" f32 f32ABS(f32 value);//возвращает модуль числа float
    extern "C" u16 s16ABS(s16 v);//возвращает модуль числа s16
    extern "C" bool GetAbsMax(f32 v, f32 max);//проверка на максимальное значение по модулю
    extern "C" bool SignIsEqu (f32 v1, f32 v2);//проверка, переменные имеют один знак?
    extern "C" float getMaxValue(float args[], unsigned short len);
  #else
    extern f32 f32ABS(f32 value);//возвращает модуль числа
    extern u16 s16ABS(s16 v);//возвращает модуль числа s16
    extern bool GetAbsMax(f32 v, f32 max);//проверка на максимальное значение по модулю
    extern bool SignIsEqu (f32 v1, f32 v2);//проверка, переменные имеют один знак?
    extern float getMaxValue(float args[], unsigned short len);
  #endif
    
#endif 