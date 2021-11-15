#ifndef STM32F4XX_INTMASH_MBBASICCOMMANDS_H
#define STM32F4XX_INTMASH_MBBASICCOMMANDS_H

#include "ModbusSlave.h"

//������ ID ���������� � ����� + crc. ����� �� ������� �11
tU8 GetDeviceID(ModbusSlaveType* Slave);
//������ ������ RAM, FLASH ��� CD
tU8 ModbusMemRead(ModbusSlaveType* Slave);
//������ � ������������ ������ ������
tU8 ModbusMemWrite(ModbusSlaveType* Slave);
//������ � ������������ ������ ������ ������ �������� ��������
tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave);
//��������� ������ �������� �������� �� ����� AND,OR
tU8 ModbusMemWriteMask(ModbusSlaveType* Slave);

tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum);
tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum);
tU8 ModbusFlashWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);

#endif
