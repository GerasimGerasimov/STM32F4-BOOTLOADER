#include "flashdata.h"
#include "bastypes.h"//содержит описания структур данных
#include "ramdata.h"
#include "Calibrationdata.h"
#include "crc16.h"
#include "STM32F4xx_Intmash_Flash.h"

#pragma location = ".fdsection"
const TFLASH_DATA FLASH_DATA = //@ ".fdsection" 
{     
    0x0401, //Modbus_RS485; 
};


#pragma location = ".bkfdsection" //section
const TFLASH_DATA BKFLASH_DATA  = //@ ".bkfdsection"
{
    0x0401, //Modbus_RS485;
    0x0401, //Modbus_option; 
    0x0401, //Modbus_USB;    
};

//копия уставок в оперативной памяти
struct TFLASH_DATA FLASH_DATA_RC;
////////////////////////////////////////////////////////////////////////////////
static bool bFlashDataLoaded = FALSE;//
////////////////////////////////////////////////////////////////////////////////
void FlashDataMkRamCpy (void);//создание копии уставок в ОЗУ
bool FlashDataChk      (void);//сравнение уставок с их копией. TRUE если есть отличия
void FlashDataSave     (void);//сохранение копии уставок во flash
bool FlashDataLoaded   (void);//проверка на то что уставки выгружены в ОЗУ
////////////////////////////////////////////////////////////////////////////////
//создание копии уставок в ОЗУ
void u8_mem_cpy( u8 *s, u8 *d, u8 c){
  while (c) {
    c--;
    *d++ = *s++;
  }
}
//запись во флэш память уставки Iz
//Source  передаю начальный адрес данных которые хочу записать
//ValueFlashAddr адрес параметра во флэшке
//Count сколько байт записать
void DataToFlash(u32 Source, u32 ValueFlashAddr, u16 Count){
//скопировать текущее содержимое сектора в буфер
//внести изменнеия в памяти буфера
//посчитать контр. сумму полученного буфера
//записать контр сумму в конец буфера
//--прерывания запретить
//стереть сектор флэша
//записать сектор
//повторить для бэкапа
//--прерывания разрешить
//повторит для FRAM
  //копирую во временный буфер весь сектор
  //DataToFlash((u32)&RAM_DATA.Iz, (u32) FLASH_DATA.fIstart, 2);
  CopyFlashToTmpBuffer((tU32*)&FLASH_DATA);
  //подготовка к изменению данных в буфере
  u32 Offset = ValueFlashAddr-(u32)&FLASH_DATA;
  u8 *Dest   = (u8 *)((u32)&FlashTmpBuffer + Offset);//сохраняемый параметр адрес во временном буфере
  u8 *Src    = (u8 *)Source;
  //копирую в буфер указанные данные
  u8 i = Count;
  while (i) {
    *Dest++ = *Src++;
    i--;
  }  
  //данные находятся во временном буфере, теперь:
  //Подсчитать контрольную сумму временного буфера
   FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //стереть резервный сектор флэша
  //записать туда данные из временного буфера
   __disable_irq(); // handles nested interrupt
   //ОТКЛЮЧАЮ ВСЕ ИМПУЛЬСЫ УПРАВЛЕНИЯ
   //чтобы небыло рывков тока при записи флэш
   disableAllImpulses();
   ////////////////////////////////////////////////////////////////////////////////
   FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*) &FlashTmpBuffer);
  //6.1)Проверить CRC буфера, если не испортился писать дальше
     if (crc16((u8*)&FlashTmpBuffer, FlashTmpBufferSize)==0){
       //7)стереть основной сектор флэша
       //8)записать туда данные из временного буфера
       FlashSectorWrite((tU32*)&FLASH_DATA, (tU32*) &FlashTmpBuffer);
     }
     else {
        //Блок оказался кривым, резервный сектор флеша испорчен.
        //Попытаться Восстановить из основного, если не получится
        //не пытаться переписать основной сектор
        FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*)&FLASH_DATA);
        //тут по идее какую нибудь ошибку бы выдать
     };
  __enable_irq(); // handles nested interrupt  
  RAM_DATA.FLAGS0.bits.FLASH_WRITE = 1;//флэш был модифицирован
}
//запись во флэш память копии уставок из ОЗУ
void SaveFlash(void){
//скопировать текущее содержимое сектора в буфер
//внести изменнеия в памяти буфера
//посчитать контр. сумму полученного буфера
//записать контр сумму в конец буфера
//--прерывания запретить
//стереть сектор флэша
//записать сектор
//повторить для бэкапа
//--прерывания разрешить
//повторит для FRAM
  //копирую во временный буфер весь сектор
  CopyFlashToTmpBuffer((tU32*)&FLASH_DATA_RC);
  //данные находятся во временном буфере, теперь:
  //Подсчитать контрольную сумму временного буфера
    FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //стереть резервный сектор флэша
  //записать туда данные из временного буфера
   __disable_irq(); // handles nested interrupt
   ////////////////////////////////////////////////////////////////////////////////
   FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*) &FlashTmpBuffer);
  //6.1)Проверить CRC буфера, если не испортился писать дальше
     if (crc16((u8*)&FlashTmpBuffer, FlashTmpBufferSize)==0){
       //7)стереть основной сектор флэша
       //8)записать туда данные из временного буфера
       FlashSectorWrite((tU32*)&FLASH_DATA, (tU32*) &FlashTmpBuffer);
     }
     else {
        //Блок оказался кривым, резервный сектор флеша испорчен.
        //Попытаться Восстановить из основного, если не получится
        //не пытаться переписать основной сектор
        FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*)&FLASH_DATA);
        //тут по идее какую нибудь ошибку бы выдать
     };
  __enable_irq(); // handles nested interrupt  
  RAM_DATA.FLAGS0.bits.FLASH_WRITE = 1;//флэш был модифицирован
}

//запись во флэш память копии калибровок из ОЗУ
void SaveCD(void){
  //записать данные из временного буфера
  __disable_irq(); // handles nested interrupt
  FlashSectorWrite((tU32*)&CD_DATA, (tU32*) &CD_DATA_RC);  
  __enable_irq(); // handles nested interrupt
}

//сравнение содержимого двух буферов
//проверяю 16 бит вордами, о делении кол-ва байт на 2 позаботися заранее
bool ChkBuffersByWord(u32 Buff1, u32 Buff2, u16 WCount){
  u16 i =  WCount;//проверяю WORDами
  u16* c = (u16*)Buff1;//буфер "копия"
  u16* s = (u16*)Buff2;//буфер "исходник"
  while (i) {
    if (*c++ != *s++) {
      return TRUE;//блоки различаются
    }
    i--;
  }
  return FALSE;//блоки идентичны
}

void FlashDataMkRamCpy (void){
  u8_mem_cpy( (u8*)&FLASH_DATA, (u8*)&FLASH_DATA_RC, sizeof (TFLASH_DATA));
  bFlashDataLoaded = TRUE;//сигналю что выгрузил уставки в ОЗУ
}

//сравнение уставок с их копией. TRUE если есть отличия
bool FlashDataChk      (void){
  return ChkBuffersByWord((u32)&FLASH_DATA,
                           (u32)&FLASH_DATA_RC,
                              sizeof (TFLASH_DATA) / 2);
  //TRUE  - блоки различаются
  //FALSE - блоки идентичны
}

//сохранение копии уставок во flash
void FlashDataSave     (void){
  if (FlashDataChk()) {//если данные отличаются 
    SaveFlash();   //то сохранить их
  }
}

//проверка на то что уставки выгружены в ОЗУ
bool FlashDataLoaded   (void){
  return bFlashDataLoaded;
}