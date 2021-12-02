#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "Intmash_bastypes.h"
#include "ModbusSlave.h"

  #ifdef __cplusplus
    extern "C" tU16 BootLoader(ModbusSlaveType* Slave);
  #else
    extern tU16 BootLoader(ModbusSlaveType* Slave);
  #endif

#endif