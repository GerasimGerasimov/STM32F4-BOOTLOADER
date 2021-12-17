#include "Intmash_bastypes.h"

const char DigitsChars[10]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};//������ �������� ����
char DeviceID[256];//������ ��� ������������ ID-������

/**
  * @brief  ������������ ID-������ � ������� xxxxxxxx IDtext, 
  * ��� xxxxxxxx - �������� ����� ����������. ���� ����� �� 8-������� �����,
  * ����� ������� ����� ���� ����, ����� �������� ���� 8 ��������. 
  * �������� 00000365 DExS.M4CPU v0.03 03.06.2013 www.intmash.ru
  * �������������� ������ �������� � ���������� ������� DeviceID.
  * @param  u32 SN - �������� ����� ����������, ����� �� 1 �� 99999999
  * @retval none.
  */
#define SN_SIZE 8 //������ ��������� ������
#define START_DIVIDER 10000000 //��������� ��������, 8-� ������
#define TEXT_INDEX_SHIFT 9 //�������� ��� ������
void IDinit(unsigned int SN, char* ID_text)
{ 
  tU8  Index=0;//������ ��� ����������� �� ������� ID
  /*������� ��������� �������� �����*/
  tU32 Divider=START_DIVIDER;//�������� 
  tU8  Digit;//����� � ������� �������  
  while(Index<SN_SIZE){//��������� 8-������� �������� �����
	Digit=SN/Divider;//����� � ������� �������  
	SN=SN%Divider;//������ �������� ����� ��������� �� ���� ����� 
	DeviceID[Index]=DigitsChars[Digit];//��������� ������ ��������
	Divider=Divider/10;//�������� ��������� �� 10, ��������� � ����� ������
	Index++;//��������� � ���������� �������
  } 
  
  /*����� ������ ������*/
  DeviceID[Index]=' ';
  Index++;
  
  /*������ ��������� ��������� ������*/
  while(ID_text[Index-TEXT_INDEX_SHIFT] != 0) {//���� ������ �� ���������
	DeviceID[Index]=ID_text[Index-TEXT_INDEX_SHIFT];//�������� �����������
	Index++;//��������� � ���������� �������
  }
  
  /*��������� ������ ID*/
  DeviceID[Index]=0;  
}