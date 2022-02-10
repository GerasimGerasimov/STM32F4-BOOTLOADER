#ifndef RESOURCES_H
#define RESOURCES_H

#include "stm32f4xx.h"
#include <vector>

typedef struct ResourceTableItem{
  u32 BinaryDataAddr;
  u32 BinaryDataSize;
  char Name[14];
  u16 crc;
} TResourceTableItem;

typedef TResourceTableItem* pItem;

typedef struct Resources {
  u16 Size;
  u16 NumberOfItems;
  TResourceTableItem Items[];
} TResources;

typedef TResources* pResources;

#define RESOURCES_ADDRESS 0x08080000

struct TIntResources {
  public:
    static void init();
    static char * getItemName(u16 idx);
    static char * getID(); 
  private:
    static constexpr pResources Root = (pResources) RESOURCES_ADDRESS;
    static std::vector<pItem> ValidItems;
    static pItem getItemByName(char* Name);
};

#endif