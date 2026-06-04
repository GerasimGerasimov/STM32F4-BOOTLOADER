#include "ModbusSlave.h"
#include "crc16.h"
#include "id.h"

/*
все команды модбас теперь хранятся тут

в ModbusSlaveConf.h добавить, при отсутсвии строки:
#include "ModbusSlave.h"

в ModbusSlaveConf.c добавить, при отсутсвии строки:
#include "ModbusSlaveConf.h"

прописать саму карту:
ModbusMapType ModbusMap[7]={
//      1b                                 +       1b                1b              1b             
//      type                               +       prefix           size         bit_modify        pointer
  {(MB_DATA_TYPE_COILS)                 + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},
  {(MB_DATA_TYPE_COILS)                 + FLASH_DATA_PREFIX,  FLASH_DATA_SIZE,    0       ,(tU8*)&FLASH_DATA},
  {(MB_DATA_TYPE_DISCRETE_INPUTS)       + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},
  {(MB_DATA_TYPE_HOLDING_REGISTERS) + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},
  {(MB_DATA_TYPE_HOLDING_REGISTERS) + FLASH_DATA_PREFIX,  FLASH_DATA_SIZE,    0       ,(tU8*)&FLASH_DATA},
  {(MB_DATA_TYPE_INPUT_REGISTERS)   + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},  
  {0,0,0,0}
};
FLASD_DATA_PREFIX, CD_DATA_PREFIX должны состоять из половины байта (FLASD_DATA_PREFIX 0x2), (CD_DATA_PREFIX 0xC)


и проверить карту команд
ниже расписана полная карта поддерживаемых команд
//число используемых команд +1
ModbusCommandHandlerType ModbusCommands[11]={
  {ModbusReadCoils, 0x01},
  {ModbusReadDiscreteInputs, 0x02},
  {ModbusReadHoldingRegisters, 0x03},
  {ModbusReadInputRegisters, 0x04},
  {ModbusWriteSingleCoil, 0x05},  
  {ModbusWriteSingleRegister, 0x06}, 
  {ModbusWriteMultipleCoils, 0x0F},  
  {ModbusWriteMultipleRegisters, 0x10},  
  {ModbusReportSlaveID, 0x11},    
  {ModbusMaskWriteRegister, 0x16},  
  {0, 0x00},
}; 
*/


/*******************************************************************************
Name: ModbusCommandDecode
декодирование пришедшей команды и ее выполнение, если эта команда известна
Arg:
ModbusSlaveType* Slave - указатель на структуру ведомого
MesageSize - размер принятой посылки
ModbusCommandHandlerType* HandlerTable - указатель на структуру обработчиков команд
Ret:
размер отправляемой посылки в байтах
*******************************************************************************/

tU16 ModbusCommandDecode(ModbusSlaveType* Slave, tU16 MesageSize, ModbusCommandHandlerType* HandlerTable)
{
  tU16 Answer=0;
  //посчитали crc
  //отвечаем на команду только в случае, если контрольная сумма посылки сошлась
  if (crc16((tU8*)Slave->Buffer, MesageSize) == 0) 
  {
    //проверка адреса ведомого в сообщении 
    if (Slave->SlaveAddress == Slave->Buffer[MB_DEVICE_ADDRESS] || MB_IS_BROADCAST_MSG ) 
    { 
      //если адрес верный, либо сообщение широковещательное - поиск обработчика
      while (HandlerTable->CommandCode)
      {
        //если обработчик нашелся - обрабатывается запрос
        if (HandlerTable->CommandCode == Slave->Buffer[MB_COMMAND_CODE]) 
        {
          Answer = HandlerTable->Handler(Slave);
          break;
        }
        //если нет - поиск продолжается 
        else HandlerTable++;
      }
      //если поиск дошел до конца таблицы - обработчик не найден, ошибка
      if((HandlerTable->CommandCode)==0) Answer = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_FUNC);
    }
  }
  //возвращатся результат декодирования
  return Answer;  
}

/*******************************************************************************
Name: ModbusError
формирование сообщения об ошибке
Arg:
ModbusSlaveType* Slave - указатель на структуру ведомого
MesageSize - тип ошибки
Ret:
размер отправляемой посылки в байтах
*******************************************************************************/
tU16 ModbusError(ModbusSlaveType* Slave,tU8 ModbusErrorType)
{
  tU16 DataLength = ERROR_ANSWER_SIZE; //длинна отправляемой посылки
  Slave->Buffer[MB_COMMAND_CODE]+=MB_ERR_CODE_PREFIX; //modify command code
  Slave->Buffer[MB_ERROR_SECTION]=ModbusErrorType; //code err
  
  FrameEndCrc16(Slave->Buffer, DataLength);  
  return DataLength;
}

/*******************************************************************************
Функция копирования данных в ответ с перестановкой байт, для соответствия 
протоколу Modbus (старший байт вперед)
*******************************************************************************/
void ModbusSwapCopy(tU8* Src, tU16* Dst, tU16 RegNum)
{
  tU16Union w;
  while (RegNum) {
    RegNum--;
    //сваплю и копирую
    w.B[1] = *Src++;
    w.B[0] = *Src++;
    *Dst++ = w.I;
  }
}

/*******************************************************************************
Поиск нужной области памяти в карте данных
*******************************************************************************/
#define ERROR_INDEX 0xff
tU8 GetModbusMapIndex(ModbusMapType* Map, tU8 SpaceCode){
  tU8 Index=0;
  while(Map[Index].MemSpaceCode !=0)
  {
    if (Map[Index].MemSpaceCode == SpaceCode) break;
    else Index++; 
  } 
  if(Map[Index].MemSpaceCode==0) Index=ERROR_INDEX;  
  return Index;  
}


/*******************************************************************************
Функция обработки запроса строки ID (код операции 0x11) ПРОВЕРИТЬ!!!!!!!!!!!!!!!
*******************************************************************************/
tU8 ModbusReportSlaveID(ModbusSlaveType* Slave){  
  tU8 DataLength = 0; //возвращаемая длина отправляемой посылки
  
  //если не широковещательное
  if(!MB_IS_BROADCAST_MSG) 
  {
    //расчет длины строки
    while (DeviceID[DataLength++]!=0);    
    Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
    //копирование строки в буфер ответа
    for(tU8 i=0; i!=DataLength; i++) Slave->Buffer[MB_DATA_SECTION_CMD_11+i]=DeviceID[i];
    //расчет размера ответа    
    DataLength += MB_DATA_SECTION_CMD_11;//прибавить длину заголовка   
    DataLength += CRC_SIZE;//прибавить длину crc 
    //расчет контрольной суммы
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  }  
  return DataLength;
}

/*******************************************************************************
Макросы для удобства работы с префиксом и адресом в функциях чтения и записи
*******************************************************************************/
#define PREFIX_SHIFT 4
#define PREFIX_MASK  ((tU8)0xF0)
//старшая тетрада старшего байта адреса первого регистра данных (префикс)
#define MB_DATA_PREFIX   (Slave->Buffer[MB_START_ADDR_HI]>>PREFIX_SHIFT)
//младшая тетрада старшего байта адреса + младший байт (адрес данных)
#define MB_DATA_ADDR     ((((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + (Slave->Buffer[MB_START_ADDR_LO]))



/*******************************************************************************
Стандартные функции чтения регистров (holding и input registers)
*******************************************************************************/
#define REG_NUM_TO_READ_MAX 125
tU8 ModbusRegRead(ModbusSlaveType* Slave, tU8 DataType)
{
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //код области данных в карте 
  tU8 SpaceCode = MB_DATA_PREFIX + DataType;
  //начальный адрес данных  
  tU16 RegAddr = MB_DATA_ADDR;
  //количество данных (регистров) 
  tU16 RegNum = Slave->Buffer[MB_REG_NUM_LO_CMD_03]; 
  //индекс нужной области данных в карте
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //если сообщение широковещательное - не отвечаем
  if(!MB_IS_BROADCAST_MSG)
  {
    //если область данных не найдена - соответствующая ошибка
    if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
    //проверка количества запрашиваемых регистров 
    else if(RegNum>REG_NUM_TO_READ_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
    //далее проверка на соответствие адреса запрашиваемых регистров из найденой области
    else if((RegAddr+RegNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
    //все проверки прошли - читаем нужные данные, формируем посылку  
    else
    {
      //копируем нужные данные в буфер посылки
      ModbusSwapCopy( (Slave->ModbusMap[ModbusMapIndex].DataPointer+(RegAddr<<1)), (tU16*)&Slave->Buffer[MB_DATA_SECTION_CMD_03], RegNum);  
      //определяем размер отправляемого пакета данных в байтах
      DataLength=(tU16)(RegNum << 1);
      Slave->Buffer[MB_DATA_BYTE_NUM_CMD_03]= DataLength;
      DataLength += MB_DATA_SECTION_CMD_03 + CRC_SIZE; 
      //формируем конец пакета
      FrameEndCrc16((tU8*)Slave->Buffer, DataLength);    
    }
  }
  return DataLength;
}

//Read Holding Registers - 0x03
tU8 ModbusReadHoldingRegisters(ModbusSlaveType* Slave){ 
  return ModbusRegRead(Slave,MB_DATA_TYPE_HOLDING_REGISTERS);  
}

//Read Input Registers - 0x04
tU8 ModbusReadInputRegisters(ModbusSlaveType* Slave){ 
  return ModbusRegRead(Slave,MB_DATA_TYPE_INPUT_REGISTERS);
}


/*******************************************************************************
Стандартные функции чтения бит (coils и discrete inputs) ПРОВЕРИТЬ!!!!!!!!!!!!!!
*******************************************************************************/
#define BIT_NUM_TO_READ_MAX 2000

tU8 ModbusBitRead(ModbusSlaveType* Slave,tU8 DataType)
{ 
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //код области данных в карте 
  tU8 SpaceCode = MB_DATA_PREFIX + DataType;
  //начальный адрес данных  
  tU16 BitAddr = MB_DATA_ADDR;
  //количество данных (бит) 
  tU16 BitNum = (((tU16)(Slave->Buffer[MB_BITS_NUM_HI_CMD_01])) << 8) + Slave->Buffer[MB_BITS_NUM_LO_CMD_01]; 
  //индекс нужной области данных в карте
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //если сообщение широковещательное - не отвечаем
  if(!MB_IS_BROADCAST_MSG)
  {
    //если область данных не найдена - соответствующая ошибка
    if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
    //проверка количества запрашиваемых бит 
    else if(BitNum>BIT_NUM_TO_READ_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
    //далее проверка на соответствие адреса запрашиваемых бит из найденой области
    else if((BitAddr+BitNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
    //все проверки прошли - читаем нужные данные, формируем посылку  
    else
    {
      //источник
      tU8 *Src = (Slave->ModbusMap[ModbusMapIndex].DataPointer+(tU8)(BitAddr/8));
      //приемник
      tU8 *Dst = &Slave->Buffer[MB_DATA_SECTION_CMD_READ01];   
      //преобразование количества запрашиваемых бит в байты
      tU8 ByteNum;
      if ((BitNum % 8) == 0) ByteNum = (tU8)(BitNum / 8);
      else ByteNum = (tU8)(BitNum / 8) + 1;
      //определяем размер отправляемого пакета данных в байтах   
      DataLength = ByteNum;
      Slave->Buffer[MB_DATA_BYTE_NUM_CMD_READ01]= DataLength;
      DataLength += MB_DATA_SECTION_CMD_READ01 + CRC_SIZE;
      //собираем посылку
      tU8 BitOffset = (tU8)(BitAddr % 8);//номер бита в байте, с которого начинать собирать ответ
      tU8 Byte;//сформированный байт на приемник
      while (ByteNum){  
        ByteNum--;
        //по младшему байту берем старшую часть по офсету 
        Byte = *(Src++)>>BitOffset;
        //по старшему байту берем младшую часть по офсету
        Byte = Byte | ((*Src << ( 8 - BitOffset)) & 0xFF);
        //отрезать последний байт согласно количеству бит или отсчитали очередной байт из 8 бит
        if(ByteNum == 0) Byte = Byte & ((1<<BitNum)-1);    
        else  BitNum -= 8; 
        //отправялем все в буфер
        *Dst++ = Byte; 
      }  
      //формируем конец пакета
      FrameEndCrc16((tU8*)Slave->Buffer, DataLength);      
    }
  }
  return DataLength; 
}

//Read Coils - 0x01
tU8 ModbusReadCoils(ModbusSlaveType* Slave){
  return ModbusBitRead(Slave,MB_DATA_TYPE_COILS);
}
//Read Discrete Inputs - 0x02
tU8 ModbusReadDiscreteInputs(ModbusSlaveType* Slave){    
  return ModbusBitRead(Slave,MB_DATA_TYPE_DISCRETE_INPUTS);
}

/*******************************************************************************
Стандартные функции записи регистров (holding registers)
*******************************************************************************/
#define REG_NUM_TO_WRITE_MAX 123

tU8 ModbusRegWrite(ModbusSlaveType* Slave, tU16 RegNum, tU8 DataSectionIndex)
{
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //код области данных в карте 
  tU8 SpaceCode = MB_DATA_PREFIX + MB_DATA_TYPE_HOLDING_REGISTERS;
  //начальный адрес данных  
  tU16 RegAddr = MB_DATA_ADDR;
  //индекс нужной области данных в карте
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //если область данных не найдена - соответствующая ошибка
  if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
  //проверка количества запрашиваемых регистров 
  else if(RegNum>REG_NUM_TO_WRITE_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
  //далее проверка на соответствие адреса запрашиваемых регистров из найденой области
  else if((RegAddr+RegNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
  //все проверки прошли - записываем нужные данные  
  else
  {
    tU16* MemDestinationAddress=(tU16*)((Slave->ModbusMap[ModbusMapIndex].DataPointer) + (RegAddr << 1));
    ModbusSwapCopy((tU8*)&Slave->Buffer[DataSectionIndex], MemDestinationAddress, RegNum);
    // взвели флаг изменения данных в области
    Slave->ModbusMap[ModbusMapIndex].ModifyFlag = 1;
  }
  //если сообщение широковещательное - не отвечаем
  if(!MB_IS_BROADCAST_MSG)
  { 
    //размер ответа на запись 
    DataLength = WR_ANSWER_SIZE; 
    //формируем конец пакета
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength);     
  }
  return DataLength;  
}

//Write Single Register - 0x06
tU8 ModbusWriteSingleRegister(ModbusSlaveType* Slave)
{
  return ModbusRegWrite(Slave, 1, MB_DATA_SECTION_CMD_06);
}

//Write Multiple Registers - 0x10
tU8 ModbusWriteMultipleRegisters(ModbusSlaveType* Slave)
{  
  return ModbusRegWrite(Slave, Slave->Buffer[MB_REG_NUM_LO_CMD_10], MB_DATA_SECTION_CMD_10);
}

//Mask Write Register - 0x16
tU8 ModbusMaskWriteRegister(ModbusSlaveType* Slave){
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //код области данных в карте 
  tU8 SpaceCode = MB_DATA_PREFIX + MB_DATA_TYPE_HOLDING_REGISTERS;
  //начальный адрес данных  
  tU16 RegAddr = MB_DATA_ADDR;
  //индекс нужной области данных в карте
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //если область данных не найдена - соответствующая ошибка
  if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR); 
  //далее проверка на соответствие адреса запрашиваемых регистров из найденой области
  else if((RegAddr+1)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
  //все проверки прошли - записываем нужные данные, формируем посылку  
  else
  {
    //адрес регистра в памяти, с которым будем работать
    tU8* MemAddress=(tU8*)(Slave->ModbusMap[ModbusMapIndex].DataPointer + (RegAddr << 1));
    //переменные для проведения операций с маской перед записью
    tU16Union Mask;//Маска
    tU16 Data;//Данные
    //получили значение из памяти
    ModbusSwapCopy(MemAddress,&Data,1);
    //модифицировали
    Mask.B[0] = Slave->Buffer[MB_REG_AND_MASK_HI];
    Mask.B[1] = Slave->Buffer[MB_REG_AND_MASK_LO];  
    Data &= Mask.I;
    Mask.B[0] = Slave->Buffer[MB_REG_OR_MASK_HI];
    Mask.B[1] = Slave->Buffer[MB_REG_OR_MASK_LO];
    Data |= Mask.I;
    //записали обратно
    ModbusSwapCopy((tU8*)&Data, (tU16*)MemAddress, 1);
    // взвели флаг изменения данных в области
    Slave->ModbusMap[ModbusMapIndex].ModifyFlag = 1;
  }
  //если сообщение не широковещательное - отвечаем
  if(!MB_IS_BROADCAST_MSG)
  {
    //размер ответа на запись    
    DataLength = WR_ANSWER_SIZE; 
    //формируем конец пакета
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength);     
  }
  return DataLength;   
}

/*******************************************************************************
Стандартные функции записи бит (coils) ПРОВЕРИТЬ!!!!!!!!!!!!!!!
*******************************************************************************/
#define BIT_NUM_TO_WRITE_MAX 2000

tU8 ModbusBitWrite(ModbusSlaveType* Slave, tU16 BitNum, tU8 DataSectionIndex)
{
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //код области данных в карте 
  tU8 SpaceCode = MB_DATA_PREFIX + MB_DATA_TYPE_COILS;
  //начальный адрес данных  
  tU16 BitAddr = MB_DATA_ADDR;  
  //индекс нужной области данных в карте
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //если область данных не найдена - соответствующая ошибка
  if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
  //проверка количества запрашиваемых бит 
  else if(BitNum>BIT_NUM_TO_WRITE_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
  //далее проверка на соответствие адреса запрашиваемых бит из найденой области
  else if((BitAddr+BitNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
  //все проверки прошли - записываем нужные данные 
  else
  {
    tU16 i = 0;      
    while (i < BitNum)
    {
      //адрес текущего байта данных
      tU8 ByteAddr = (tU8) (BitAddr /8) ;      
      //адрес байта в памяти устройства
      tU8* MemDestinationAddress = Slave->ModbusMap[ModbusMapIndex].DataPointer + ByteAddr;   
      //маска для преобразования бита      
      tU8 BitMask = 1 << ((tU8) (BitAddr%8));
      //состояние бита, в которое нужно выставить
      tU8 BitState = (Slave->Buffer[DataSectionIndex + (tU8)(i/8)]) & (1<<(i%8));        
      //записываем состояние бита 
      if(BitState) *MemDestinationAddress |=  BitMask;
      else *MemDestinationAddress &= (~BitMask);          
      i+=1;     
      BitAddr += 1;// номер следующего бита  
    }
    // взвели флаг изменения данных в области
    Slave->ModbusMap[ModbusMapIndex].ModifyFlag = 1;    
  }
  //если сообщение не широковещательное - отвечаем
  if(!MB_IS_BROADCAST_MSG)
  {
    //размер ответа на запись
    DataLength = WR_ANSWER_SIZE; 
    //формируем конец пакета
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength); 
  }
  return DataLength; 
}

//Write Single Coil - 0x05
tU8 ModbusWriteSingleCoil(ModbusSlaveType* Slave)
{
  //длинна отправляемой посылки
  tU8 DataLength;
  //два состояния бита: FF - 1, 00 - 0 и в младшем байте только нули  
  //проверяем что состояние верное
  if ((Slave->Buffer[MB_BIT_STATE] != 0xFF) && (Slave->Buffer[MB_BIT_STATE] != 0) || (Slave->Buffer[MB_BIT_STATE +1] != 0))   
    DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_DATA);
  else
    DataLength = ModbusBitWrite(Slave, 1, MB_DATA_SECTION_CMD_05);
 
  return DataLength;    
}

//Write Multiple Coils - 0x0F
tU8 ModbusWriteMultipleCoils(ModbusSlaveType* Slave){
  
  //длинна отправляемой посылки
  tU8 DataLength;  
  //получили количество бит из запроса
  tU16 BitNum = (Slave->Buffer[MB_BITS_NUM_HI_CMD_0F] << 8) + Slave->Buffer[MB_BITS_NUM_LO_CMD_0F];   
  
  //если количество байт в запросе не совпало с количеством бит в запросе
  if ((1 + (BitNum -1) / 8) != Slave->Buffer[MB_DATA_BYTE_NUM_CMD_0F])
    DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_DATA);
  else     
    DataLength = ModbusBitWrite(Slave, BitNum, MB_DATA_SECTION_CMD_0F);
  
  return DataLength;    
}


