#ifndef ID_H
#define ID_H

extern char * DeviceID;

  #ifdef __cplusplus
    extern "C" void IDinit();//�������, ����������� ID-������
  #else
    extern void IDinit();//�������, ����������� ID-������
  #endif
#endif
