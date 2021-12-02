#include "bootloader.h"
#include "crc16.h"
#include <string.h>
#include "STM32F4xx_intmash_MBbasicCommands.h"

//Команды Бутлоадера
#define BOOT_CMD_CODE_OFFSET            0x02
#define BOOT_CMD_GET_PAGES_LIST         0x00
#define BOOT_CMD_SET_ERASED_PAGES       0x01
#define BOOT_CMD_SET_AREA_START_ADDR    0x02
#define BOOT_CMD_PUT_AREA_CODE          0x03

#define BOOT_PAGES_LIST_DATA_SECTION    3

tU16 getPagesList(ModbusSlaveType* Slave);

tU16 BootLoader(ModbusSlaveType* Slave){
  tU8 cmd = Slave->Buffer[BOOT_CMD_CODE_OFFSET];
  tU16 DataLength = 0;
  switch (cmd) {
    case BOOT_CMD_GET_PAGES_LIST:
        DataLength = getPagesList(Slave);
      break;
    default:
      break;
  }
  
  return DataLength;
}

//Описание страниц должно соответсвовать JSON
//0x00000000 HE поддерживается JSON,
//поэтому hex значения надо кодировать в строки с помощью ""
//и кавычки в строке экранировать обратным слэшэм 
const char PagesList[] = "[{\"start\": \"0x00000000\" , \"size\": 4095}," \
                          "{\"start\": \"0x00001000\" , \"size\": 4095}," \
                          "{\"start\": \"0x00002000\" , \"size\": 4095}," \
                          "{\"start\": \"0x00003000\" , \"size\": 4095}," \
                          "{\"start\": \"0x00004000\" , \"size\": 4095}," \
                          "{\"start\": \"0x00005000\" , \"size\": 4095}]";
                             
tU16 getPagesList(ModbusSlaveType* Slave){
  //return GetDeviceID(Slave);
  tU16 DataLength = 0; //длинна отправляемой посылки
  DataLength = strlen(PagesList);
  //Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION + 0] = (DataLength >> 8) & 0x00FF;
  //Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION + 1] = (DataLength) & 0x00FF;
  //DataLength  = 2;
  strcpy((char *) &Slave->Buffer[BOOT_PAGES_LIST_DATA_SECTION], PagesList);
  DataLength += BOOT_PAGES_LIST_DATA_SECTION;//прибавить длину заголовка   
  DataLength += CRC_SIZE;//прибавить длину crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}
