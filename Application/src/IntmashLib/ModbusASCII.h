#ifndef ModbusASCII_H
#define ModbusASCII_H

#include "ModbusSlave.h"
#include "ModbusMaster.h"
#include "Intmash_bastypes.h"

//состояния клиента, старшая тетрада для того, чтобы можно было совмещать и с мастером и со слейвом
#define STATE_RX_RCV       0x1    //ожидание начала приема сообщений
#define STATE_RX_WAIT_EOF  0x2    //ожидание окончания сообщения  

#define ASKII_TIME_PAUSE 10000 //1 sec

//структура клиента для работа по протоколу аски
typedef struct 
{
  tU8 BufferAscii[MODBUS_BUFFER_SIZE]; //буфер для хранения промежуточных данных, например, когда принимаемое сообщение с большими паузами между байтами
  tU8 DataAsciiCnt; //счетчик для буфера принимаемых данных- у каждого уарта - свой
  tU8 Status; //статус клиента 
  void (*SetTimer)(tU16 Time);//процедура запуска таймера
  void (*StopTimer)(void);//процедура остановки таймера
} ModbusAsciiType;

extern tU8 MBAsciiSlaveProc(ModbusSlaveType* Slave, ModbusAsciiType* MBASlave, tU8 MesageSize, ModbusCommandHandlerType* HandlerTable);
void TimeOutCheck(tU8* Status);
extern tU8 MBAsciiMasterProc(MBmasterType* Master, tU8 MesageSize, ModbusAsciiType* MBAclient);

extern tU8 MBAsciiTx(tU8* OutBuffer, tU8 MesageSize);
extern void ModbusCopy(tU8* Src, tU8* Dst, tU16 ByteNum);

#endif
