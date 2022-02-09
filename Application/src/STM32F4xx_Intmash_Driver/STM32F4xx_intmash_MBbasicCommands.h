#ifndef STM32F4XX_INTMASH_MBBASICCOMMANDS_H
#define STM32F4XX_INTMASH_MBBASICCOMMANDS_H

#include "ModbusSlave.h"

#ifdef __cplusplus
  //������ ID ���������� � ����� + crc. ����� �� ������� �11
  extern "C" tU16 GetDeviceID(ModbusSlaveType* Slave);
  //������ ������ RAM, FLASH ��� CD
  extern "C" tU8 ModbusMemRead(ModbusSlaveType* Slave);
  //������ � ������������ ������ ������
  extern "C" tU8 ModbusMemWrite(ModbusSlaveType* Slave);
  //������ � ������������ ������ ������ ������ �������� ��������
  extern "C" tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave);
  //��������� ������ �������� �������� �� ����� AND,OR
  extern "C" tU8 ModbusMemWriteMask(ModbusSlaveType* Slave);
  //
  extern "C" tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum);
  extern "C" tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum);
  extern "C" tU8 ModbusFlashWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
  extern "C" tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
#else
  //������ ID ���������� � ����� + crc. ����� �� ������� �11
  extern tU16 GetDeviceID(ModbusSlaveType* Slave);
  //������ ������ RAM, FLASH ��� CD
  extern tU8 ModbusMemRead(ModbusSlaveType* Slave);
  //������ � ������������ ������ ������
  extern tU8 ModbusMemWrite(ModbusSlaveType* Slave);
  //������ � ������������ ������ ������ ������ �������� ��������
  extern tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave);
  //��������� ������ �������� �������� �� ����� AND,OR
  extern tU8 ModbusMemWriteMask(ModbusSlaveType* Slave);
  //
  extern tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum);
  extern tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum);
  extern tU8 ModbusFlashWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
  extern tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
#endif
  
#endif
