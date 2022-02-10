#include "resources_wrapper.h"
#include "resources.h"

char * getItemName(u16 idx) {
  return TIntResources::getItemName(idx);
}

char * getIDfromResources(void) {
  return TIntResources::getID();
}