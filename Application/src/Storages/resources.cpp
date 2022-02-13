#include "resources.h"
#include "crc16.h"
#include <string.h>
#include "ramdata.h"

//http://microsin.net/programming/pc/faq-how-to-mix-c-and-cpp.html

const char * IDtext = "Application v1.0.0 07.01.2022 www.intmash.ru";

//иницифлизация статического чдена класса
std::vector<pItem> TIntResources::ValidItems = std::vector<pItem>();

void TIntResources::init() {
  u16 i = 0;
  RAM_DATA.r1 = Root->Size;
  RAM_DATA.r2 = Root->NumberOfItems;
  /*TODO если CRC любого ресурса не совпадает,то дропить весь список*/
  while (i < Root->NumberOfItems) {
    pItem p = &Root->Items[i++];
    if (crc16((u8*) p, sizeof (TResourceTableItem)) == 0) {
      ValidItems.push_back(p);
    }
  }
}

bool isRequiredName(char * Name1, char * Name2) {
  return (bool)(strcmp(Name1, Name2) == 0);
}

pItem TIntResources::getItemByName(char* Name) {
  pItem res = NULL;
  for (const pItem & item: ValidItems) {
    if (isRequiredName(item->Name, Name)) {
      return item;
    }
  }
  return res;
}

const char * unknown = "unknown";

std::string getStringFormResource(pItem item) {
  const u32 Addr = item->BinaryDataAddr;
  const u32 Size = item->BinaryDataSize;
  std::string str;
  str.assign((char*) Addr, Size);
  return str;
}

char * TIntResources::getID() {
  pItem item = getItemByName((char*)"ID");
  return (item != NULL)
    ? (char *)getStringFormResource(item).c_str()
    : (char *) unknown;
}

char * TIntResources::getItemName(u16 idx) {
  return Root->Items[idx].Name;
}