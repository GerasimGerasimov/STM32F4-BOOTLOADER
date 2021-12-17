#ifndef STM32F4xx_Intmash_FLASH_H
#define STM32F4xx_Intmash_FLASH_H

#include "Intmash_bastypes.h"
#include "stm32f4xx.h"

//буфер временного хранения данных сектора FALSH
#define FlashTmpBufferSize 256
#define FlashTmpBufferSize_w  (FlashTmpBufferSize/2) //размер буфера в словах
#define FlashTmpBufferSize_dw (FlashTmpBufferSize/4) //размер буфера в двойных словах

//статусные флаги 
#define FLASH_WRITE_DIS  B0    //флаг запрета записи во флеш
#define FLASH_CHANGE    B1     //флаг изменения данных во флеш
#define FLASH_DATA_ERR  B2     //флаг ошибки FLASH секции 
#define BACKUP_DATA_ERR B3     //флаг ошибки BACKUPFLASH секции 

extern tU8 FlashTmpBuffer[FlashTmpBufferSize];
extern tU8Union FlashStatus;

  #ifdef __cplusplus
    extern "C" void FlashSectorWrite(tU32* FlashSectorAddr, tU32* Buffer);
    extern "C" void CopyFlashToTmpBuffer(tU32* Addr);
    extern "C" void FlashDataProtectedWrite(tU32* FlashDataSector, tU32* BackupDataSector);
    extern "C" void CheckFlashData(tU32* FlashDataSector, tU32* BackupDataSector);

    extern "C" void StartFlashChange();
    extern "C" void EndFlashChange();
    extern "C" FLASH_Status EraseFlashPage(tU32 Addr);
  #else
    extern void FlashSectorWrite(tU32* FlashSectorAddr, tU32* Buffer);
    extern void CopyFlashToTmpBuffer(tU32* Addr);
    extern void FlashDataProtectedWrite(tU32* FlashDataSector, tU32* BackupDataSector);
    extern void CheckFlashData(tU32* FlashDataSector, tU32* BackupDataSector);

    extern void StartFlashChange();
    extern void EndFlashChange();
    extern FLASH_Status EraseFlashPage(tU32 Addr);
  #endif
    




#endif /*STM32F4xx_Intmash_USART_H */