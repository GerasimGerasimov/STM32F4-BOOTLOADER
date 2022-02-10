#include "resources.h"
//http://microsin.net/programming/pc/faq-how-to-mix-c-and-cpp.html
void TIntResources::init() {

}

pResources TIntResources::getRes() {
  return TIntResources::ResAddr;
}

u32 TIntResources::getID() {
  return (u32)TIntResources::ResAddr;//ID;//12345678;
}

char * TIntResources::getItemName(u16 idx) {
  return ResAddr->Items[idx].Name;
}