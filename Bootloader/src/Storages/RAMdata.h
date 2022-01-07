#ifndef RAMDATA_H
#define RAMDATA_H

#include "bastypes.h"

typedef 
  union   {
    u32	   i;
    __packed struct {//����� ������
      //r0000.L
      unsigned DExS_PWR_LNK     : 1;//������� ����� � ������ DExS.PWR2 (��� �������� ����������� � ������������� ���������)
      unsigned DExS_PWR_Ready   : 1;//���������� ����� DExS.PWR (� �.�. ���������� � �������)
      unsigned SyncRect         : 1;//1-������������� � ����� 0-�������������� �����������
      unsigned ExtRegTimeOut    : 1;//1-������������ �������� ��������� (��������)
      unsigned R_INSL_LOW       : 1;//����������������� ����� ��������
      unsigned PWR1             : 1;//��������/��������� �������� ���-� ������������
      unsigned Blank            : 1;//������� ���� ���������������
      unsigned CLM              : 1;//Current Limit Mode - ����� ����������� ����
      //r0000.H
      unsigned imp_block        : 1;//���������� ��������� ��� �������� ������������ �����    
      unsigned IExcRef_min      : 1;//������� ���� ����������� ���������� �������� ������������ �������              
      unsigned IExcRef_max      : 1;//������� ���� ����������� ���������� �������� ������������� �������
      unsigned FreqRealay       : 1;//���� ������� ���������� ����������
      unsigned FS_P             : 1;//������ ���������� +
      unsigned FS_M             : 1;//������ ���������� -    
      unsigned UstLow           : 1;//�������� ���������� ����
      unsigned UstFail          : 1;//���������� ���������� ����
      //r0001.L
      unsigned ForceByUst       : 1;//���������� ��� �������� ���������� �������
      unsigned cfi_enable       : 1;//���� ����������� �������� ���� ������� (������)
      unsigned cfu_enable       : 1;//���� ����������� �������� ���������� ������� (������ + ������� ������ ������� "����������")
      unsigned i2tR             : 1;// 1-i2t ������ � �������� ������
      unsigned TestMode         : 1;// ����� �����������
      unsigned Tougle           : 1;//
      unsigned Up               : 1;//
      unsigned ThGFBReady       : 1;//���� ������� ���� (�����������) ��������
      //r0001.H
      unsigned b23              : 1;//    
      unsigned b24              : 1;//    
      unsigned BackPower        : 1;//��������� ��������
      unsigned QminLimIrUp      : 1;//���������� ����������� ���������� ��������, ��������� �������� ��� �����������
      unsigned QmaxLimIrDn      : 1;//���������� ������������ ���������� ��������, ��������� ������� ��� �����������
      unsigned T1000Hz          : 1;//������������ ��� ��������� 0.01 ��� �������� ������� ��������� ���������� ��������� ���������
      unsigned FS_OK            : 1;//��������� �������� �������
      unsigned FLASH_WRITE      : 1;// 1-���� ������������� (������������ ����� ���������� ������� ������ �����)      
    } bits ;
} baFLAGS0;//����� ���������� � ���������;


//��������� ������������ � RAM
typedef struct TRAM_DATA {
    baFLAGS0 FLAGS0;
    u16 X;   //04 
    u16 Y;   //05
} TRAM_DATA;

//��������� ������������ RAM
extern struct TRAM_DATA RAM_DATA;

//����������� ��������� ��������� ���������
#define RAM_DATA_PREFIX 0x0
#define RAM_DATA_SIZE 0xf0

#endif