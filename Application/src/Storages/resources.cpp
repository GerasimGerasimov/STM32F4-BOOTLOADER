#include "resources.h"


#define RESOURCES_ADDRESS 0x08080000



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

pResources Res = (pResources) RESOURCES_ADDRESS;

char * getItemName(u16 idx) {
  return Res->Items[idx].Name;
}

u8* getRes(void) {
  return (u8*) Res;
}

void getResorcesTable(void) {
  Res->Size = 0;
  Res->Items[1].crc = 0;
}

u16 getResourcesSize(void) {
  return Res->Size;
}

u16 getResourcesNumberOfItems(void) {
  return Res->NumberOfItems;
}