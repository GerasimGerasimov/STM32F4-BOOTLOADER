#include "bootloader.h"
#include "crc16.h"
#include <string.h>
#include "str.h"
#include "STM32F4xx_intmash_MBbasicCommands.h"

//������� ����������
#define BOOT_CMD_CODE_OFFSET            0x02
#define BOOT_CMD_GET_PAGES_LIST         0x00
#define BOOT_CMD_SET_ERASED_PAGES       0x01
#define BOOT_CMD_SET_AREA_START_ADDR    0x02
#define BOOT_CMD_GET_MEMORY_FROM_ADDR   0x03 //�������� ������� ��������� ���-�� ���� ������ ������� � ���������� ������ 
#define BOOT_CMD_PUT_AREA_CODE          0x04

#define BOOT_PAGES_LIST_DATA_SECTION    3

tU16 getPagesList(ModbusSlaveType* Slave);
tU16 setErasedPages(ModbusSlaveType* Slave);
tU16 readMemoryBlockFromAddr(ModbusSlaveType* Slave);

tU16 BootLoader(ModbusSlaveType* Slave){
  tU8 cmd = Slave->Buffer[BOOT_CMD_CODE_OFFSET];
  switch (cmd) {
    case BOOT_CMD_GET_PAGES_LIST:
        return getPagesList(Slave);
    case BOOT_CMD_SET_ERASED_PAGES:
        return setErasedPages(Slave);
    case BOOT_CMD_GET_MEMORY_FROM_ADDR:
        return readMemoryBlockFromAddr(Slave);
    default:
      return 0;
  }
  
}

//������:
//01.B0.03.AAAAAAAA.CC��.CRC
//AAAAAAAA - ��������� 32-������ ����� ������� ������
//���� - ������������� ���-�� ���� 0..64�
//�����:
//01.B0.03.CCCC.DD.DD.DD.DD.DD....DD.DD.CRC
//CCCC- ���-�� ���� � ������, ����� ��������� � ��������
//      ���� ���-�� ���� � ������ ������������ MCU
//      ������, ����������� ������, ������ �������������� ������� ���� ������������
//� ������ ������:
//01.B0.03.0000.CRC
tU16 readMemoryBlockFromAddr(ModbusSlaveType* Slave) {
  const tU32Union StartAddr = {
    .B[0] = Slave->Buffer[3],
    .B[1] = Slave->Buffer[4],
    .B[2] = Slave->Buffer[5],
    .B[3] = Slave->Buffer[6]
  };
  const tU16Union count = {
    .B[0] = Slave->Buffer[7],
    .B[1] = Slave->Buffer[8],
  };
  /*TODO  if count is more than the Slave->Buffer length,
          the count should be as the Slave->Buffer lenght*/
  u8_mem_cpy( (unsigned char *)StartAddr.I, &Slave->Buffer[5], count.I);
  Slave->Buffer[3] =  count.B[0];
  Slave->Buffer[4] =  count.B[1];
  
  tU16 DataLength = count.I;
  DataLength += BOOT_PAGES_LIST_DATA_SECTION;//��������� ����� ���������   
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}

//�������� ������� ������ �������������� JSON
//0x00000000 HE �������������� JSON,
//������� hex �������� ���� ���������� � ������ � ������� ""
//� ������� � ������ ������������ �������� ������ 

const char PagesList[] = 
"[{\"start\": \"0x08000000\", \"size\": 16383}," \
 "{\"start\": \"0x08004000\", \"size\": 16383}," \
 "{\"start\": \"0x08008000\", \"size\": 16383}," \
 "{\"start\": \"0x0800C000\", \"size\": 16383}," \
 "{\"start\": \"0x08010000\", \"size\": 65535}," \
 "{\"start\": \"0x08020000\", \"size\": 131071}," \
 "{\"start\": \"0x08040000\", \"size\": 131071}," \
 "{\"start\": \"0x08060000\", \"size\": 131071}," \
 "{\"start\": \"0x08080000\", \"size\": 131071}," \
 "{\"start\": \"0x080A0000\", \"size\": 131071}," \
 "{\"start\": \"0x080C0000\", \"size\": 131071}," \
 "{\"start\": \"0x080E0000\", \"size\": 131071}]";

tU16 getPagesList(ModbusSlaveType* Slave){
  //return GetDeviceID(Slave);
  tU16 DataLength = 0; //������ ������������ �������
  DataLength = strlen(PagesList);
  //Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION + 0] = (DataLength >> 8) & 0x00FF;
  //Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION + 1] = (DataLength) & 0x00FF;
  //DataLength  = 5;
  strcpy((char *) &Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION], PagesList);
  DataLength += BOOT_PAGES_LIST_DATA_SECTION;//��������� ����� ���������   
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}

tU16 setErasedPages(ModbusSlaveType* Slave){
  tU16 DataLength = 0; //������ ������������ �������
  Slave->Buffer[2] = 0x01;
  DataLength  = 3;
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}