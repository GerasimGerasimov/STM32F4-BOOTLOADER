#include "bootloader.h"
#include "crc16.h"
#include <string.h>
#include "str.h"
#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_Flash.h"
#include <vector>

//������� ����������
#define BOOT_CMD_CODE_OFFSET            0x02
#define BOOT_CMD_GET_PAGES_LIST         0x00
#define BOOT_CMD_SET_ERASED_PAGES       0x01
#define BOOT_CMD_SET_AREA_START_ADDR    0x02
#define BOOT_CMD_GET_MEMORY_FROM_ADDR   0x03 //�������� ������� ��������� ���-�� ���� ������ ������� � ���������� ������ 
#define BOOT_CMD_PUT_AREA_CODE          0x04

#define BOOT_PAGES_LIST_HEAD_SIZE    3
#define BOOT_GET_MEM_HEAD_SIZE       5

tU16 getPagesList(ModbusSlaveType* Slave);
tU16 setErasedPages(ModbusSlaveType* Slave);
tU16 readMemoryBlockFromAddr(ModbusSlaveType* Slave);
tU16 writeCodeToFlash(ModbusSlaveType* Slave);

tU16 BootLoader(ModbusSlaveType* Slave){
  tU8 cmd = Slave->Buffer[BOOT_CMD_CODE_OFFSET];
  switch (cmd) {
    case BOOT_CMD_GET_PAGES_LIST:
        return getPagesList(Slave);
    case BOOT_CMD_SET_ERASED_PAGES:
        return setErasedPages(Slave);
    case BOOT_CMD_GET_MEMORY_FROM_ADDR:
        return readMemoryBlockFromAddr(Slave);
    case BOOT_CMD_PUT_AREA_CODE:
        return writeCodeToFlash(Slave);
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
//Test string
const char SIDtext[] = "BOOTLOADER v1.0.0 07.12.2021 www.intmash.ru";
//const volatile char * id = (char *) &SIDtext;

tU16 readMemoryBlockFromAddr(ModbusSlaveType* Slave) {
  const volatile char * id = (char *) &SIDtext;
  /* TODO change address-endian at backend  */
  const tU32Union StartAddr = {
    .B[0] = Slave->Buffer[6],
    .B[1] = Slave->Buffer[5],
    .B[2] = Slave->Buffer[4],
    .B[3] = Slave->Buffer[3]
  };
  const tU16Union count = {
    .B[0] = Slave->Buffer[8],
    .B[1] = Slave->Buffer[7],
  };
  /*TODO  if count is more than the Slave->Buffer length,
          the count should be as the Slave->Buffer lenght*/
  u8_mem_cpy( (unsigned char *)StartAddr.I, &Slave->Buffer[BOOT_GET_MEM_HEAD_SIZE], count.I);
  Slave->Buffer[3] =  count.B[1];
  Slave->Buffer[4] =  count.B[0];
  
  tU16 DataLength = count.I;
  DataLength += BOOT_GET_MEM_HEAD_SIZE;//��������� ����� ���������   
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
  strcpy((char *) &Slave->Buffer[BOOT_PAGES_LIST_HEAD_SIZE], PagesList);
  DataLength += BOOT_PAGES_LIST_HEAD_SIZE;//��������� ����� ���������   
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}

//�������� �������� �������
//������
//01.B0.01.PS.AAAAAAAA.BBBBBBBB.CCCCCCCC.CRC
//PS - ���-�� �������
//AAAAAAAA.BBBBBBBB.CCCCCCCC -  ������ ����� �������
//
//�����:
//01.B0.01.CRC
std::vector<tU32> getPagesAddrList( tU8 * buff) {
    tU8 NumOfPages = buff[0];
    tU32 * pPages = (tU32 * ) &buff[1];
    std::vector<tU32> Pages (NumOfPages);
    for (auto & page: Pages) {
        page = *pPages ++;
    };
    
    return Pages;
}

FLASH_Status erasePages(const std::vector<tU32> Pages) {
  FLASH_Status status;
  StartFlashChange();
    for (const tU32 & page: Pages) {
       status = EraseFlashPage(page);
       if (status != FLASH_COMPLETE) break;
    }
  EndFlashChange();
  return status;
}

tU16 setErasedPages(ModbusSlaveType* Slave){
  const std::vector<tU32> Pages = getPagesAddrList((tU8 *) &Slave->Buffer[3]);
  FLASH_Status status = erasePages(Pages);
  Slave->Buffer[4] = status;
  tU16 DataLength  = 4;
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}

/*TODO starting write flash*/
/*
������:
01.B0.04.CCCC.AAAAAAAA.DD.DD.DD.DD.DD....DD.DD.CRC
CCCC - ���-�� ����
AAAAAAAA - ��������� �����
DD... - ������

�����:
01.B0.04
*/
void writeCodeSpase(tU32 startAddr, tU16 count, tU8 * buff) {
  StartFlashChange();
  while (count --) { 
    FLASH_ProgramByte(startAddr++, *buff++);
  }
  EndFlashChange();
}


tU16 writeCodeToFlash(ModbusSlaveType* Slave) {
  const tU16Union count = {
    .B[0] = Slave->Buffer[3],
    .B[1] = Slave->Buffer[4],
  };
  const tU32Union StartAddr = {
    .B[0] = Slave->Buffer[5],
    .B[1] = Slave->Buffer[6],
    .B[2] = Slave->Buffer[7],
    .B[3] = Slave->Buffer[8]
  };    
  tU8 * pData = (tU8 * ) &Slave->Buffer[9];
  writeCodeSpase(StartAddr.I, count.I, pData);
  tU16 DataLength  = 3;
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}