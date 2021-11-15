#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "stm32f4xx.h"
#include "id.h"
#include "crc16.h"
#include "CalibrationData.h"
#include "FlashData.h"
#include "RAMdata.h"
#include "ModbusSlave.h"

#define CRC_SIZE 2 //размер crc-2 байта
#define WR_ANSWER_SIZE 8 //размер ответа на запрос записи 
#define MASK_ANSWER_SIZE 10 //размер ответа на запрос маски 
#define PREFIX_SHIFT 4
#define PREFIX_MASK  ((tU8)0xF0)

//функция вычисления длины ID 
tU8 GetDeviceIDLength(void){
tU8 i=0;
 while (DeviceID[i++]!=0);
 return i;
}

//запись ID устройства в буфер + crc. ответ на команду х11
tU8 GetDeviceID(ModbusSlaveType* Slave){
  tU8 i=0;  
  tU8 DataLength = 0; //длинна отправляемой посылки
  
  DataLength = GetDeviceIDLength();  
  Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
  do
  Slave->Buffer[MB_DATA_SECTION_CMD_11+i]=DeviceID[i];
  while ((i++)!=DataLength);
  DataLength += MB_DATA_SECTION_CMD_11;//прибавить длину заголовка   
  DataLength += CRC_SIZE;//прибавить длину crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);

  return DataLength;
}



//функция чтения данных из разных секторов памяти
//раньше называлась ModbusRamRead, что не есть правильно - тк читает она из любого сектора в зависимости от адреса
tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum)
{  
  //пишем в буфер нужные данные для отправки старшим байтом вперед
  ModbusSwapCopy( (DATA_BASE+(RegAddr<<1)), (tU16*)&Buffer[MB_DATA_SECTION_CMD_03], RegNum);  
  //определяем размер отправляемого пакета данных в байтах
  tU16 DataLength=RegNum<<1;
  Buffer[MB_DATA_BYTE_NUM_CMD_03]= DataLength;
  DataLength += MB_DATA_SECTION_CMD_03 + CRC_SIZE; //+ длинна заголовка + размер crc 
  //формируем конец пакета
  FrameEndCrc16((tU8*)Buffer, DataLength);
  return DataLength; //возвращает размер отправляемой посылки
}


//функция выбора функции чтения памяти RAM, FLASH или CD 
tU8 ModbusMemRead(ModbusSlaveType* Slave)
{
  //старшая тетрада старшего байта адреса первого регистра данных (префикс)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //начальный адрес регистра данных
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //количество запрашиваемых регистров  
  tU16 RegNum =  (((tU16)Slave->Buffer[MB_REG_NUM_HI_CMD_03]) << 8) + Slave->Buffer[MB_REG_NUM_LO_CMD_03];
  //последний считываемый регистр
  tU16 LastRegAddr = RegAddr + RegNum -1;
  //указатель на область памяти для чтения
  tU8* Source=0;
  
  //выбор области памяти для чтения 
  //если области памяти не найдено или запрос не соответствует размеру области,
  //то указатель адреса чтения оставляем равным 0.
  switch (MemSpacePrefix)
  {
    case RAM_DATA_PREFIX:
      if(LastRegAddr < RAM_DATA_SIZE) Source=(tU8*)&RAM_DATA;  
    break;
    
    case FLASH_DATA_PREFIX:
      if(LastRegAddr < FLASH_DATA_SIZE) Source=(tU8*)&FLASH_DATA;
    break;
    
    case CD_DATA_PREFIX:
      if(LastRegAddr < CD_DATA_SIZE) Source=(tU8*)&CD_DATA;
    break; 
    
    default:
      break;
  }
  
  //если указатель задан - читаем из памяти, если не задан - возвращаем ошибку
  if(Source==0) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR);
  else DataLength = ModbusDataRead(Source, Slave->Buffer, RegAddr, RegNum);
  
  
  return (DataLength);  
}

//функция записи данных в RAM память
tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum){
  tU8 DataLength = 0; //длинна отправляемой посылки

  tU16 *ModbusAddrSet = (tU16*)((tU32)&RAM_DATA + ((tU32)RegAddr << 1));
  //чтение буфера в память
  ModbusSwapCopy((tU8*)&Buffer[BufDataIdx], ModbusAddrSet, RegNum);
  /* если BufDataIdx = 0, значит мы работаем не с буфером slave, а с переменной. 
  считать CRC в этом случае не нужно  */  
  if(BufDataIdx != 0)
  {
    DataLength = WR_ANSWER_SIZE; //размер ответа на запись регистров (0x10, 0x06)
    FrameEndCrc16((tU8*)Buffer, DataLength);
  }
  return DataLength; //возвращает размер отправляемой посылки
}

//функция записи в CD
tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum)// 
{
  tU8 DataLength = 0; //длинна отправляемой посылки

  //если запись во флеш запрещена -не пишем, а просто формируем ответ
  if(FlashStatus.Bits.FLASH_WRITE_DIS){
    if(BufDataIdx != 0)
    {
      DataLength = WR_ANSWER_SIZE; //размер ответа на запись регистров (0x10, 0x06)
      FrameEndCrc16((tU8*)Buffer, DataLength);
    } 
    return DataLength; //возвращает размер отправляемой посылки
  }
  CopyFlashToTmpBuffer((tU32*)&CD_DATA);//DATA_BASE
  //адрес во временном буфере
  tU16 *ModbusAddrSet = (tU16*)((tU32)&FlashTmpBuffer + ((tU32)RegAddr << 1));
  //внести изменения во временный буфер со свопом байт
  ModbusSwapCopy((tU8*)&Buffer[BufDataIdx], (tU16*) ModbusAddrSet, RegNum);
  //данные находятся во временном буфере, теперь:
  //записать данные из временного буфера
  __disable_irq(); // handles nested interrupt
  FlashSectorWrite((tU32*)&CD_DATA, (tU32*) FlashTmpBuffer);
  __enable_irq(); // handles nested interrupt
    /* если BufDataIdx = 0, значит мы работаем не с буфером slave, а с переменной. 
  считать CRC в этом случае не нужно  */  
  if(BufDataIdx != 0)
  {
    DataLength = WR_ANSWER_SIZE; //размер ответа на запись регистров (0x10, 0x06)
    FrameEndCrc16((tU8*)Buffer, DataLength);
  }
  return DataLength; //возвращает размер отправляемой посылки

}

//функция записи во FLASH память
tU8 ModbusFlashWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum)
{ 
  tU8 DataLength = 0; //длинна отправляемой посылки
  volatile tU16Union crc, _crc;
  
  if(FlashStatus.Bits.FLASH_WRITE_DIS == 0)
  {
    if(FlashStatus.Bits.FLASH_DATA_ERR==0)
    {
      //копирование данных из основного сектора с уставками в буфер
      CopyFlashToTmpBuffer((tU32*)&FLASH_DATA);
      //адрес во временном буфере
      tU16 *ModbusAddrSet = (tU16*)((tU32)&FlashTmpBuffer + ((tU32)RegAddr << 1));
       //внести изменения во временный буфер со свопом байт
      ModbusSwapCopy((tU8*)&Buffer[BufDataIdx], (tU16*) ModbusAddrSet, RegNum);
      //данные находятся во временном буфере, теперь:
      //Подсчитать контрольную сумму временного буфера
      if(BufDataIdx != 0) FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);       
      //буфер готов к записи       
      FlashDataProtectedWrite((tU32*)&FLASH_DATA, (tU32*)&BKFLASH_DATA);
    }
  }   
  /* если BufDataIdx = 0, значит мы работаем не с буфером slave, а с переменной. 
  считать CRC в этом случае не нужно  */  
  if(BufDataIdx != 0)
  {
    DataLength = WR_ANSWER_SIZE; //размер ответа на запись регистров (0x10, 0x06)
    FrameEndCrc16((tU8*)Buffer, DataLength);
  }
  return DataLength; //возвращает размер отправляемой посылки
}

//выбор функции записи в определенный сектор памяти
tU8 ModbusMemWrite(ModbusSlaveType* Slave){  
  //старшая тетрада старшего байта адреса первого регистра данных (префикс)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT;  
  //начальный адрес регистра данных
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //количество данных 
  tU16 RegNum =  (((tU16)Slave->Buffer[MB_REG_NUM_HI_CMD_10])<<8) + Slave->Buffer[MB_REG_NUM_LO_CMD_10];  
  //последний считываемый регистр  
  tU16 LastRegAddr = RegAddr + RegNum -1;
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //переменная для определения ошибки обращения к несуществующему адресу
  tU8  Error=0;
  
  //выбор программы записи памяти
  switch (MemSpacePrefix)
  {
    case RAM_DATA_PREFIX:
      if(LastRegAddr<RAM_DATA_SIZE ) DataLength=ModbusRamWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,RegAddr,RegNum);        
      else Error=1;//вдрес вне допустимой зоны       
    break;
    
    case FLASH_DATA_PREFIX:
      if(LastRegAddr<FLASH_DATA_SIZE ) DataLength=ModbusFlashWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum);
      else Error=1;//вдрес вне допустимой зоны          
    break;
    
    case CD_DATA_PREFIX:
      if(LastRegAddr<CD_DATA_SIZE ) DataLength=ModbusCDWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum);
      else Error=1;//вдрес вне допустимой зоны         
    break; 
    
    default:   //не нашёл подходящей области 
      Error=1;         
    break;  
  }
  //если широковещательное
  if(MB_IS_BROADCAST_MSG) DataLength = 0;  
  //если нет - смотрим была ли ошибка обращения к несуществующему адресу
  else if(Error) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR); 
  
  return (DataLength);
}

#define SINGLE_WRITE_REGNUM 1
tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave){
  
  //старшая тетрада старшего байта адреса первого регистра данных (префикс)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  //начальный адрес регистра данных  
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //переменная для определения ошибки обращения к несуществующему адресу  
  tU8  Error=0;
  
  
  //выбор программы записи памяти
  switch (MemSpacePrefix)
  {
    case RAM_DATA_PREFIX:
      if(RegAddr<RAM_DATA_SIZE ) DataLength = ModbusRamWrite(Slave->Buffer,MB_DATA_SECTION_CMD_06,RegAddr,SINGLE_WRITE_REGNUM); 
      else Error=1;//вдрес вне допустимой зоны
    break;
    
    case FLASH_DATA_PREFIX:
      if(RegAddr<FLASH_DATA_SIZE ) DataLength = ModbusFlashWrite(Slave->Buffer,MB_DATA_SECTION_CMD_06,(tU8)RegAddr,SINGLE_WRITE_REGNUM);
      else Error=1;//вдрес вне допустимой зоны
    break;
    
    case CD_DATA_PREFIX:
      if(RegAddr<CD_DATA_SIZE ) DataLength = ModbusCDWrite(Slave->Buffer,MB_DATA_SECTION_CMD_06,(tU8)RegAddr,SINGLE_WRITE_REGNUM);
      else Error=1;//вдрес вне допустимой зоны
    break; 
    
    default: //не нашёл подходящей области 
      Error=1; 
    break;  
  }
  //если широковещательное
  if(MB_IS_BROADCAST_MSG) DataLength = 0;  
  //если нет - смотрим была ли ошибка обращения к несуществующему адресу
  else if(Error) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR); 
  
  return (DataLength);
}


//запись одного регистра по маске
#define MASK_WRITE_REGNUM 1
tU8 ModbusMemWriteMask(ModbusSlaveType* Slave)
{
  //старшая тетрада старшего байта адреса первого регистра данных (префикс)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  //начальный адрес регистра данных
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //переменная для определения ошибки обращения к несуществующему адресу
  tU8  Error=0;
  
  /*получили маску*/
  tU16Union Mask_AND;
  tU16Union Mask_OR;  
  Mask_AND.B[0] = Slave->Buffer[MB_REG_AND_MASK_HI];
  Mask_AND.B[1] = Slave->Buffer[MB_REG_AND_MASK_LO];  
  Mask_OR.B[0] = Slave->Buffer[MB_REG_OR_MASK_HI];
  Mask_OR.B[1] = Slave->Buffer[MB_REG_OR_MASK_LO];  
  
  tU16 Data;  
  switch (MemSpacePrefix)       
  {
    case RAM_DATA_PREFIX: 
      if(RegAddr<RAM_DATA_SIZE){
        ModbusSwapCopy (((tU8*)&RAM_DATA)+(RegAddr<<1),&Data,MASK_WRITE_REGNUM);
        Data = (Data & Mask_AND.I) | Mask_OR.I;
        ModbusRamWrite((tU8*)&Data,0,RegAddr,MASK_WRITE_REGNUM);
        DataLength = MASK_ANSWER_SIZE;      
      }
      else Error=1;//вдрес вне допустимой зоны
    break;
    
    case FLASH_DATA_PREFIX:
      if(RegAddr<FLASH_DATA_SIZE){
        ModbusSwapCopy (((tU8*)&FLASH_DATA)+(RegAddr<<1),&Data,MASK_WRITE_REGNUM);          
        Data = (Data & Mask_AND.I) | Mask_OR.I;
        ModbusFlashWrite((tU8*)&Data,0,(tU8)RegAddr,MASK_WRITE_REGNUM);
        DataLength = MASK_ANSWER_SIZE;
      }
      else Error=1;//вдрес вне допустимой зоны
    break;
    
    case CD_DATA_PREFIX:
      if(RegAddr<CD_DATA_SIZE){
        ModbusSwapCopy (((tU8*)&CD_DATA)+(RegAddr<<1),&Data,MASK_WRITE_REGNUM);          
        Data = (Data & Mask_AND.I) | Mask_OR.I;
        ModbusCDWrite((tU8*)&Data,0,(tU8)RegAddr,MASK_WRITE_REGNUM);
        DataLength = MASK_ANSWER_SIZE;
      }
      else Error=1;//вдрес вне допустимой зоны
    break; 
    
    default:  //не нашёл подходящей области    
      Error=1;
    break;
  }
  //если широковещательное
  if(MB_IS_BROADCAST_MSG) DataLength = 0;  
  //если нет - смотрим была ли ошибка обращения к несуществующему адресу
  else if(Error) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR); 
  
  return (DataLength);  
}
