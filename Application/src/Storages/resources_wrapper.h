#ifndef RESOURCES_WRAPPER_H
#define RESOURCES_WRAPPER_H

#include "stm32f4xx.h"// basic types

  #ifdef __cplusplus
    extern "C" char * getItemName(u16 idx);
    extern "C" char * getIDfromResources(void);
  #else
    extern char * getItemName(u16 idx);
    extern char * getIDfromResources(void);
  #endif
#endif
