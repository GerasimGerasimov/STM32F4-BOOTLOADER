#ifndef RESOURCES_H
#define RESOURCES_H

#include "stm32f4xx.h"// basic types

typedef struct ResourceTableItem{
  u32 BinaryDataAddr;
  u32 BinaryDataSize;
  char Name[14];
  u16 crc;
} TResourceTableItem;

typedef TResourceTableItem* pItems;

typedef struct Resources {
  u16 Size;
  u16 NumberOfItems;
  TResourceTableItem Items[];
} TResources;

typedef TResources* pResources;

#define RESOURCES_ADDRESS 0x08080000

struct TIntResources {
  public:
    static constexpr pResources ResAddr = (pResources) RESOURCES_ADDRESS;
    void init();
    pResources getRes();
    char * getItemName(u16 idx);
    static u32 getID(); 
};

/*
  #ifdef __cplusplus
    extern "C" void getResorcesTable(void);
    extern "C" u16 getResourcesSize(void);
    extern "C" u16 getResourcesNumberOfItems(void);
    extern "C" u8* getRes(void);
  #else
    extern void getResorcesTable(void);
    extern u16 getResourcesSize(void);
    extern u16 getResourcesNumberOfItems(void);
    extern u8* getRes(void);
  #endif
*/
#endif