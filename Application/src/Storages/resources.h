#ifndef RESOURCES_H
#define RESOURCES_H

#include "stm32f4xx.h"// basic types

  #ifdef __cplusplus
    extern "C" void getResorcesTable(void);
    extern "C" char * getItemName(u16 idx);
    extern "C" u16 getResourcesSize(void);
    extern "C" u16 getResourcesNumberOfItems(void);
    extern "C" u8* getRes(void);
  #else
    extern void getResorcesTable(void);
    extern char * getItemName(u16 idx);
    extern u16 getResourcesSize(void);
    extern u16 getResourcesNumberOfItems(void);
    extern u8* getRes(void);
  #endif
#endif