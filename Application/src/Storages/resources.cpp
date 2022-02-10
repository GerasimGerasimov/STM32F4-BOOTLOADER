#include "resources.h"
#include "crc16.h"
#include <string.h>
#include "ramdata.h"

//http://microsin.net/programming/pc/faq-how-to-mix-c-and-cpp.html

const char * IDtext = "Application v1.0.0 07.01.2022 www.intmash.ru";

//иницифлизация статического чдена класса
std::vector<pItem> TIntResources::ValidItems = std::vector<pItem>();

u16 g = 0;

void TIntResources::init() {
  u16 i = 0;
  RAM_DATA.r1 = Root->Size;
  RAM_DATA.r2 = Root->NumberOfItems;
  /*TODO если CRC любого ресурса не совпадает,то дропить весь список*/
  while (i < Root->NumberOfItems) {
    TResourceTableItem item = Root->Items[i++];
    if (crc16((u8*) &item, sizeof (TResourceTableItem)) == 0) {
      pItem p = &item;
      ValidItems.push_back(p);
      //p = ValidItems.back();
      //memcpy((char *)&RAM_DATA.a8[g++], p->Name, strlen(p->Name));
    }
  }
  g = 0;
  pItem z;
  for (auto e: ValidItems) {
    z = ValidItems[g];
    memcpy((char *)&RAM_DATA.a8[g++], z->Name, strlen(z->Name));
  } 
}

bool isRequiredName(char * Name1, char * Name2) {
  return (bool)(strcmp(Name1, Name2) == 0);
}

pItem TIntResources::getItemByName(char* Name) {
  RAM_DATA.r2 = 0xA5A5;
  pItem res = NULL;
  for (const pItem & item: ValidItems) {
    if (isRequiredName(item->Name, Name)) {
      return item;
    }
  }
  return res;
}

const char * unknown = "unknown";

char * TIntResources::getID() {
  pItem item = getItemByName((char*)"INI");
  if (item != NULL) {
    RAM_DATA.r1 = 0xEEEE;
    return item->Name;
  }
  return (char *) unknown;
}

char * TIntResources::getItemName(u16 idx) {
  return Root->Items[idx].Name;
}