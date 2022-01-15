#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "Intmash_bastypes.h"
#include "ModbusSlave.h"

  #ifdef __cplusplus
    extern "C" tU16 BootLoader(ModbusSlaveType* Slave);
    extern "C" void jumpToApplication(void);
    extern "C" bool isBootLoaderMustBeStart(void);
    extern "C" bool isApplicationReadyToStart(void);
  #else
    extern tU16 BootLoader(ModbusSlaveType* Slave);
    extern void jumpToApplication(void);
    extern bool isBootLoaderMustBeStart(void);
    extern bool isApplicationReadyToStart(void);
  #endif

#endif