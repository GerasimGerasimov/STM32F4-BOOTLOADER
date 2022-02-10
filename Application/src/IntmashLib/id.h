#ifndef ID_H
#define ID_H

extern char * DeviceID;

  #ifdef __cplusplus
    extern "C" void IDinit();//функция, формирующая ID-строку
  #else
    extern void IDinit();//функция, формирующая ID-строку
  #endif
#endif
