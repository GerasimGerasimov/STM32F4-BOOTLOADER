#include "bootloader.h"
#include "crc16.h"
#include <string.h>
#include "str.h"
#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_Flash.h"
#include <vector>

//Команды Бутлоадера
#define BOOT_CMD_CODE_OFFSET            0x02
#define BOOT_CMD_GET_PAGES_LIST         0x00
#define BOOT_CMD_SET_ERASED_PAGES       0x01
#define BOOT_CMD_SET_AREA_START_ADDR    0x02
#define BOOT_CMD_GET_MEMORY_FROM_ADDR   0x03 //передать участок указанное кол-во байт памяти начиная с указанного адреса 
#define BOOT_CMD_PUT_AREA_CODE          0x04

#define BOOT_PAGES_LIST_HEAD_SIZE    3
#define BOOT_GET_MEM_HEAD_SIZE       5

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

//Запрос:
//01.B0.03.AAAAAAAA.CCСС.CRC
//AAAAAAAA - начальный 32-битный адрес участка памяти
//СССС - запрашиваемое кол-во байт 0..64К
//Ответ:
//01.B0.03.CCCC.DD.DD.DD.DD.DD....DD.DD.CRC
//CCCC- кол-во байт в ответе, может совпадать с запросом
//      макс кол-во байт в ответе контролирует MCU
//      мастер, отправивший запрос, должен контролировать сколько байт возвращается
//В случае ошибки:
//01.B0.03.0000.CRC
//Test string
const char SIDtext[] = "BOOTLOADER v1.0.0 07.12.2021 www.intmash.ru";
//const volatile char * id = (char *) &SIDtext;

tU16 readMemoryBlockFromAddr(ModbusSlaveType* Slave) {
  const volatile char * id = (char *) &SIDtext;
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
  DataLength += BOOT_GET_MEM_HEAD_SIZE;//прибавить длину заголовка   
  DataLength += CRC_SIZE;//прибавить длину crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}

//Описание страниц должно соответсвовать JSON
//0x00000000 HE поддерживается JSON,
//поэтому hex значения надо кодировать в строки с помощью ""
//и кавычки в строке экранировать обратным слэшэм 

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
  tU16 DataLength = 0; //длинна отправляемой посылки
  DataLength = strlen(PagesList);
  //Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION + 0] = (DataLength >> 8) & 0x00FF;
  //Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION + 1] = (DataLength) & 0x00FF;
  //DataLength  = 5;
  strcpy((char *) &Slave->Buffer[BOOT_PAGES_LIST_HEAD_SIZE], PagesList);
  DataLength += BOOT_PAGES_LIST_HEAD_SIZE;//прибавить длину заголовка   
  DataLength += CRC_SIZE;//прибавить длину crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}

//Стирание заданных страниц
//Запрос
//01.B0.01.PS.AAAAAAAA.BBBBBBBB.CCCCCCCC.CRC
//PS - кол-во страниц
//AAAAAAAA.BBBBBBBB.CCCCCCCC -  адреса начал страниц
//
//Ответ:
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
  DataLength += CRC_SIZE;//прибавить длину crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}
