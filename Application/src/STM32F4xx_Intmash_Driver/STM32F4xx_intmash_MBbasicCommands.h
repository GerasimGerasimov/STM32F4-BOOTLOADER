#ifndef STM32F4XX_INTMASH_MBBASICCOMMANDS_H
#define STM32F4XX_INTMASH_MBBASICCOMMANDS_H

#include "ModbusSlave.h"

#ifdef __cplusplus
  //запись ID устройства в буфер + crc. ответ на команду х11
  extern "C" tU16 GetDeviceID(ModbusSlaveType* Slave);
  //чтение памяти RAM, FLASH или CD
  extern "C" tU8 ModbusMemRead(ModbusSlaveType* Slave);
  //запись в определенный сектор памяти
  extern "C" tU8 ModbusMemWrite(ModbusSlaveType* Slave);
  //запись в определенный сектор памяти одного регистра хранения
  extern "C" tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave);
  //изменение одного регистра хранения по маске AND,OR
  extern "C" tU8 ModbusMemWriteMask(ModbusSlaveType* Slave);
  //
  extern "C" tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum);
  extern "C" tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum);
  extern "C" tU8 ModbusFlashWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
  extern "C" tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
#else
  //запись ID устройства в буфер + crc. ответ на команду х11
  extern tU16 GetDeviceID(ModbusSlaveType* Slave);
  //чтение памяти RAM, FLASH или CD
  extern tU8 ModbusMemRead(ModbusSlaveType* Slave);
  //запись в определенный сектор памяти
  extern tU8 ModbusMemWrite(ModbusSlaveType* Slave);
  //запись в определенный сектор памяти одного регистра хранения
  extern tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave);
  //изменение одного регистра хранения по маске AND,OR
  extern tU8 ModbusMemWriteMask(ModbusSlaveType* Slave);
  //
  extern tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum);
  extern tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum);
  extern tU8 ModbusFlashWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
  extern tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
#endif
  
#endif
