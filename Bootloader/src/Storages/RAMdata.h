#ifndef RAMDATA_H
#define RAMDATA_H

#include "bastypes.h"

typedef 
  union   {
    u32	   i;
    __packed struct {//флаги аварий
      //r0000.L
      unsigned f0 : 1;//
      unsigned f1 : 1;//
      unsigned f2 : 1;//
      unsigned f3 : 1;//
      unsigned f4 : 1;//
      unsigned f5 : 1;//
      unsigned f6 : 1;//
      unsigned f7 : 1;//
      //r0000.H
      unsigned f8 : 1;//
      unsigned f9 : 1;//
      unsigned f10 : 1;//
      unsigned f11 : 1;//
      unsigned f12 : 1;//
      unsigned f13 : 1;//
      unsigned f14 : 1;//
      unsigned f15 : 1;//
      //r0001.L
      unsigned f16 : 1;//
      unsigned f17 : 1;//
      unsigned f18 : 1;//
      unsigned f19 : 1;//
      unsigned f20 : 1;//
      unsigned f21 : 1;//
      unsigned f22 : 1;//
      unsigned f23 : 1;//
      //r0001.H
      unsigned f24 : 1;//
      unsigned f25 : 1;//
      unsigned f26 : 1;//
      unsigned f27 : 1;//
      unsigned f28 : 1;//
      unsigned T1000Hz : 1;//»спользуетс€ дл€ генерации 0.01 сек периодов отсчЄта временных интервалов основного алгоритма
      unsigned f30 : 1;//
      unsigned f31 : 1;//   
    } bits ;
} baFLAGS0;//флаги управлени€ и индикации;


//параметры расположеные в RAM
typedef struct TRAM_DATA {
    baFLAGS0 FLAGS0;
  u16 counter1;           //
  u16 counter2;        //
  u16 counter3;        //
  u16 counter4;
  u16 data[8];
  u32 data32[8];
} TRAM_DATA;

//параметры расположеные RAM
extern struct TRAM_DATA RAM_DATA;

//определение диапазона доступных регистров
#define RAM_DATA_PREFIX 0x0
#define RAM_DATA_SIZE 0xf0

#endif