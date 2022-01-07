#ifndef ModbusASCII_H
#define ModbusASCII_H

#include "ModbusSlave.h"
#include "ModbusMaster.h"
#include "Intmash_bastypes.h"

//��������� �������, ������� ������� ��� ����, ����� ����� ���� ��������� � � �������� � �� �������
#define STATE_RX_RCV       0x1    //�������� ������ ������ ���������
#define STATE_RX_WAIT_EOF  0x2    //�������� ��������� ���������  

#define ASKII_TIME_PAUSE 10000 //1 sec

//��������� ������� ��� ������ �� ��������� ����
typedef struct 
{
  tU8 BufferAscii[MODBUS_BUFFER_SIZE]; //����� ��� �������� ������������� ������, ��������, ����� ����������� ��������� � �������� ������� ����� �������
  tU8 DataAsciiCnt; //������� ��� ������ ����������� ������- � ������� ����� - ����
  tU8 Status; //������ ������� 
  void (*SetTimer)(tU16 Time);//��������� ������� �������
  void (*StopTimer)(void);//��������� ��������� �������
} ModbusAsciiType;

extern tU8 MBAsciiSlaveProc(ModbusSlaveType* Slave, ModbusAsciiType* MBASlave, tU8 MesageSize, ModbusCommandHandlerType* HandlerTable);
void TimeOutCheck(tU8* Status);
extern tU8 MBAsciiMasterProc(MBmasterType* Master, tU8 MesageSize, ModbusAsciiType* MBAclient);

extern tU8 MBAsciiTx(tU8* OutBuffer, tU8 MesageSize);
extern void ModbusCopy(tU8* Src, tU8* Dst, tU16 ByteNum);

#endif
