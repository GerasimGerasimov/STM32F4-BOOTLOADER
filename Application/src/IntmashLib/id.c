#include "id.h"
#include "Intmash_bastypes.h"
#include "resources_wrapper.h"

char * DeviceID;//массив для формирования ID-строки

void IDinit() { 
  DeviceID = getIDfromResources(); 
}