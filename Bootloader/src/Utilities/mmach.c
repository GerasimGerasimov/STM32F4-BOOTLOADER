#include "bastypes.h"
#include "mmach.h"

f32 f32ABS(f32 value);//возвращает модуль числа float
u16 s16ABS(s16 v);//возвращает модуль числа s16
bool GetAbsMax(f32 v, f32 max);//проверка на максимальное значение по модулю
bool SignIsEqu (f32 v1, f32 v2);//проверка, переменные имеют один знак?
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

//возвращает модуль числа float
f32 f32ABS(f32 value){
  return (value < 0)
         ? value = -value
         : value;
}

//возвращает модуль числа s16
u16 s16ABS(s16 v){
  return (v < 0)
        ? -v
        : (u16)v;
}

//проверка на максимальное значение по модулю
//сравнивает величины по модулю
//TRUE  если v >= max
//FALSE если v <  max
bool GetAbsMax(f32 v, f32 max){
  return (f32ABS(v)>= f32ABS(max));
}

//проверка, переменные имеют один знак?
//TRUE - знаки равны
//FALSE - знаки различаютс
bool SignIsEqu (f32 v1, f32 v2){
  u8 c = 0;
  if (v1 >= 0) c |= (1<<0);
  if (v2 >= 0) c |= (1<<1);
  //0(00) - обе переменные имеют знак "-"
  //3(11) - обе переменные имеют знак "+"
  //1(01) - всё остальное
  //2(10) - .............
  if (c == 0) return TRUE;
  if (c == 3) return TRUE;
  return FALSE;
}