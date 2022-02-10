#ifndef RAMDATA_H
#define RAMDATA_H

#include "bastypes.h"

typedef 
  union   {
    u32	   i;
    __packed struct {//флаги аварий
      //r0000.L
      unsigned DExS_PWR_LNK     : 1;//Наличие связи с платой DExS.PWR2 (для проверки исправности и достоверности показаний)
      unsigned DExS_PWR_Ready   : 1;//Готовность платы DExS.PWR (в т.ч. напряжение и частоты)
      unsigned SyncRect         : 1;//1-синхронизация в норме 0-синхроимпульсы отсутствуют
      unsigned ExtRegTimeOut    : 1;//1-приостановка внешнего регулятор (косинуса)
      unsigned R_INSL_LOW       : 1;//предупредительный порог изоляции
      unsigned PWR1             : 1;//включает/отключает импульсы упр-я выпрямителем
      unsigned Blank            : 1;//гашение поля инвертированием
      unsigned CLM              : 1;//Current Limit Mode - режим ограничения тока
      //r0000.H
      unsigned imp_block        : 1;//блокировка импульсов для закрытия тиристорного ключа    
      unsigned IExcRef_min      : 1;//Задание тока возбуждения ограничено уставкой минимального задания              
      unsigned IExcRef_max      : 1;//Задание тока возбуждения ограничено уставкой максимального задания
      unsigned FreqRealay       : 1;//Реле частоты наведённого напряжения
      unsigned FS_P             : 1;//сигнал скольжения +
      unsigned FS_M             : 1;//сигнал скольжения -    
      unsigned UstLow           : 1;//Просадка напряжения сети
      unsigned UstFail          : 1;//Отсутствие напряжения сети
      //r0001.L
      unsigned ForceByUst       : 1;//Форсировка при просадке напряжения статора
      unsigned cfi_enable       : 1;//Флаг обнаружения сигналов тока статора (меандр)
      unsigned cfu_enable       : 1;//Флаг обнаружения сигналов напряжения статора (меандр + уровень больше уставки "отсутствие")
      unsigned i2tR             : 1;// 1-i2t модель в перегрев ротора
      unsigned TestMode         : 1;// Режим опробования
      unsigned Tougle           : 1;//
      unsigned Up               : 1;//
      unsigned ThGFBReady       : 1;//ключ гашения поля (тиристорный) исправен
      //r0001.H
      unsigned b23              : 1;//    
      unsigned b24              : 1;//    
      unsigned BackPower        : 1;//отбратная мощность
      unsigned QminLimIrUp      : 1;//достигнута минимальная реактивная мощность, требуется повысить ток возбуждения
      unsigned QmaxLimIrDn      : 1;//достигнута максимальная реактивная мощность, требуется снизить ток возбуждения
      unsigned T1000Hz          : 1;//Используется для генерации 0.01 сек периодов отсчёта временных интервалов основного алгоритма
      unsigned FS_OK            : 1;//состояние файловой системы
      unsigned FLASH_WRITE      : 1;// 1-флэш модифицирован (сбразывается после выполнения команды чтения флэша)      
    } bits ;
} baFLAGS0;//флаги управления и индикации;


//параметры расположеные в RAM
typedef struct TRAM_DATA {
    baFLAGS0 FLAGS0;
    u16 r1;   //04 
    u16 r2;   //05
    u16 r3;   //04 
    u16 r4;   //05 
    u16 gap;
    u32 a8 [8];
} TRAM_DATA;

//параметры расположеные RAM
extern struct TRAM_DATA RAM_DATA;

//определение диапазона доступных регистров
#define RAM_DATA_PREFIX 0x0
#define RAM_DATA_SIZE 0xf0

#endif