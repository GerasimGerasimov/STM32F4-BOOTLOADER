/**
  ******************************************************************************
  * @file    ID.h
  * @author  IMD, Sledin
  * @version V1.0.0
  * @date    18-06-2013
  * @brief  ������, ����������� ID-������ ����������
  ******************************************************************************
  * @�������:
  *
  * 
  * 
  * 
  * 
  * 
  *
  * 
  ******************************************************************************
  */
#ifndef ID_H
#define ID_H

extern char DeviceID[256];//���������� ����������� ������� ��� ID-������

  #ifdef __cplusplus
    extern "C" void IDinit(unsigned int SN, char* ID_text);//�������, ����������� ID-������
  #else
    extern void IDinit(unsigned int SN, char* ID_text);//�������, ����������� ID-������
  #endif
#endif
