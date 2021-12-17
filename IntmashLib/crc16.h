#ifndef CRC16_H
#define CRC16_H

  #ifdef __cplusplus
    extern "C" unsigned short crc16(unsigned char *puchMsg, int DataLen);
    extern "C" void FrameEndCrc16(unsigned char *Frame, unsigned int FrameSize);
  #else
    extern unsigned short crc16(unsigned char *puchMsg, int DataLen);
    extern void FrameEndCrc16(unsigned char *Frame, unsigned int FrameSize);
  #endif

#endif 