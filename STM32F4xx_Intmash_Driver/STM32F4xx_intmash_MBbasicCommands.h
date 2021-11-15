#ifndef STM32F4XX_INTMASH_MBBASICCOMMANDS_H
#define STM32F4XX_INTMASH_MBBASICCOMMANDS_H

#include "ModbusSlave.h"

//запись ID устройства в буфер + crc. ответ на команду х11
tU8 GetDeviceID(ModbusSlaveType* Slave);
//чтение памяти RAM, FLASH или CD
tU8 ModbusMemRead(ModbusSlaveType* Slave);
//запись в определенный сектор памяти
tU8 ModbusMemWrite(ModbusSlaveType* Slave);
//запись в определенный сектор памяти одного регистра хранения
tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave);
//изменение одного регистра хранения по маске AND,OR
tU8 ModbusMemWriteMask(ModbusSlaveType* Slave);

tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum);
tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum);
tU8 ModbusFlashWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);
tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum);

#endif
