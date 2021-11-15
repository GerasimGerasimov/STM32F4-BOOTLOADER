#ifndef _BKUP_SRAM_H
#define _BKUP_SRAM_H

#include "stm32f4xx.h"

  #ifdef __cplusplus
    extern "C" void BkpSram_Configuration(void);
    extern "C" u8 BkpSramWrite( u8 *Src, u16 BytesNum, u16 BkpSramAddr);
    extern "C" u8 BkpSramRead( u8 *Dst, u16 BytesNum, u16 BkpSramAddr);
  #else
    extern void BkpSram_Configuration(void);
    extern u8 BkpSramWrite( u8 *Src, u16 BytesNum, u16 BkpSramAddr);
    extern u8 BkpSramRead( u8 *Dst, u16 BytesNum, u16 BkpSramAddr);
  #endif
#endif