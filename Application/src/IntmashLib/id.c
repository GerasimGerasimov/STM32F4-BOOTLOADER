#include "id.h"
#include "Intmash_bastypes.h"
#include "resources_wrapper.h"

char * DeviceID;//������ ��� ������������ ID-������

void IDinit() { 
  DeviceID = getIDfromResources(); 
}