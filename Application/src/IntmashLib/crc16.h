#ifndef CRC16_H
#define CRC16_H

#include "stm32f4xx.h"

  #ifdef __cplusplus
    extern "C" u16 crc16(u8 *puchMsg, u16 DataLen);
    extern "C" void FrameEndCrc16(u8 *Frame, u16 FrameSize);
  #else
    extern u16 crc16(u8 *puchMsg, u16 DataLen);
    extern void FrameEndCrc16(u8 *Frame, u16 FrameSize);
  #endif

#endif 