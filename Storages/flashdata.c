#include "flashdata.h"
#include "bastypes.h"//�������� �������� �������� ������
#include "ramdata.h"
#include "Calibrationdata.h"
#include "crc16.h"
#include "STM32F4xx_Intmash_Flash.h"

#pragma location = ".fdsection"
const TFLASH_DATA FLASH_DATA = //@ ".fdsection" 
{     
    0x0401, //Modbus_RS485; 
};


#pragma location = ".bkfdsection" //section
const TFLASH_DATA BKFLASH_DATA  = //@ ".bkfdsection"
{
    0x0401, //Modbus_RS485;
    0x0401, //Modbus_option; 
    0x0401, //Modbus_USB;    
};

//����� ������� � ����������� ������
struct TFLASH_DATA FLASH_DATA_RC;
////////////////////////////////////////////////////////////////////////////////
static bool bFlashDataLoaded = FALSE;//
////////////////////////////////////////////////////////////////////////////////
void FlashDataMkRamCpy (void);//�������� ����� ������� � ���
bool FlashDataChk      (void);//��������� ������� � �� ������. TRUE ���� ���� �������
void FlashDataSave     (void);//���������� ����� ������� �� flash
bool FlashDataLoaded   (void);//�������� �� �� ��� ������� ��������� � ���
////////////////////////////////////////////////////////////////////////////////
//�������� ����� ������� � ���
void u8_mem_cpy( u8 *s, u8 *d, u8 c){
  while (c) {
    c--;
    *d++ = *s++;
  }
}
//������ �� ���� ������ ������� Iz
//Source  ������� ��������� ����� ������ ������� ���� ��������
//ValueFlashAddr ����� ��������� �� ������
//Count ������� ���� ��������
void DataToFlash(u32 Source, u32 ValueFlashAddr, u16 Count){
//����������� ������� ���������� ������� � �����
//������ ��������� � ������ ������
//��������� �����. ����� ����������� ������
//�������� ����� ����� � ����� ������
//--���������� ���������
//������� ������ �����
//�������� ������
//��������� ��� ������
//--���������� ���������
//�������� ��� FRAM
  //������� �� ��������� ����� ���� ������
  //DataToFlash((u32)&RAM_DATA.Iz, (u32) FLASH_DATA.fIstart, 2);
  CopyFlashToTmpBuffer((tU32*)&FLASH_DATA);
  //���������� � ��������� ������ � ������
  u32 Offset = ValueFlashAddr-(u32)&FLASH_DATA;
  u8 *Dest   = (u8 *)((u32)&FlashTmpBuffer + Offset);//����������� �������� ����� �� ��������� ������
  u8 *Src    = (u8 *)Source;
  //������� � ����� ��������� ������
  u8 i = Count;
  while (i) {
    *Dest++ = *Src++;
    i--;
  }  
  //������ ��������� �� ��������� ������, ������:
  //���������� ����������� ����� ���������� ������
   FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //������� ��������� ������ �����
  //�������� ���� ������ �� ���������� ������
   __disable_irq(); // handles nested interrupt
   //�������� ��� �������� ����������
   //����� ������ ������ ���� ��� ������ ����
   disableAllImpulses();
   ////////////////////////////////////////////////////////////////////////////////
   FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*) &FlashTmpBuffer);
  //6.1)��������� CRC ������, ���� �� ���������� ������ ������
     if (crc16((u8*)&FlashTmpBuffer, FlashTmpBufferSize)==0){
       //7)������� �������� ������ �����
       //8)�������� ���� ������ �� ���������� ������
       FlashSectorWrite((tU32*)&FLASH_DATA, (tU32*) &FlashTmpBuffer);
     }
     else {
        //���� �������� ������, ��������� ������ ����� ��������.
        //���������� ������������ �� ���������, ���� �� ���������
        //�� �������� ���������� �������� ������
        FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*)&FLASH_DATA);
        //��� �� ���� ����� ������ ������ �� ������
     };
  __enable_irq(); // handles nested interrupt  
  RAM_DATA.FLAGS0.bits.FLASH_WRITE = 1;//���� ��� �������������
}
//������ �� ���� ������ ����� ������� �� ���
void SaveFlash(void){
//����������� ������� ���������� ������� � �����
//������ ��������� � ������ ������
//��������� �����. ����� ����������� ������
//�������� ����� ����� � ����� ������
//--���������� ���������
//������� ������ �����
//�������� ������
//��������� ��� ������
//--���������� ���������
//�������� ��� FRAM
  //������� �� ��������� ����� ���� ������
  CopyFlashToTmpBuffer((tU32*)&FLASH_DATA_RC);
  //������ ��������� �� ��������� ������, ������:
  //���������� ����������� ����� ���������� ������
    FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //������� ��������� ������ �����
  //�������� ���� ������ �� ���������� ������
   __disable_irq(); // handles nested interrupt
   ////////////////////////////////////////////////////////////////////////////////
   FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*) &FlashTmpBuffer);
  //6.1)��������� CRC ������, ���� �� ���������� ������ ������
     if (crc16((u8*)&FlashTmpBuffer, FlashTmpBufferSize)==0){
       //7)������� �������� ������ �����
       //8)�������� ���� ������ �� ���������� ������
       FlashSectorWrite((tU32*)&FLASH_DATA, (tU32*) &FlashTmpBuffer);
     }
     else {
        //���� �������� ������, ��������� ������ ����� ��������.
        //���������� ������������ �� ���������, ���� �� ���������
        //�� �������� ���������� �������� ������
        FlashSectorWrite((tU32*)&BKFLASH_DATA, (tU32*)&FLASH_DATA);
        //��� �� ���� ����� ������ ������ �� ������
     };
  __enable_irq(); // handles nested interrupt  
  RAM_DATA.FLAGS0.bits.FLASH_WRITE = 1;//���� ��� �������������
}

//������ �� ���� ������ ����� ���������� �� ���
void SaveCD(void){
  //�������� ������ �� ���������� ������
  __disable_irq(); // handles nested interrupt
  FlashSectorWrite((tU32*)&CD_DATA, (tU32*) &CD_DATA_RC);  
  __enable_irq(); // handles nested interrupt
}

//��������� ����������� ���� �������
//�������� 16 ��� �������, � ������� ���-�� ���� �� 2 ���������� �������
bool ChkBuffersByWord(u32 Buff1, u32 Buff2, u16 WCount){
  u16 i =  WCount;//�������� WORD���
  u16* c = (u16*)Buff1;//����� "�����"
  u16* s = (u16*)Buff2;//����� "��������"
  while (i) {
    if (*c++ != *s++) {
      return TRUE;//����� �����������
    }
    i--;
  }
  return FALSE;//����� ���������
}

void FlashDataMkRamCpy (void){
  u8_mem_cpy( (u8*)&FLASH_DATA, (u8*)&FLASH_DATA_RC, sizeof (TFLASH_DATA));
  bFlashDataLoaded = TRUE;//������� ��� �������� ������� � ���
}

//��������� ������� � �� ������. TRUE ���� ���� �������
bool FlashDataChk      (void){
  return ChkBuffersByWord((u32)&FLASH_DATA,
                           (u32)&FLASH_DATA_RC,
                              sizeof (TFLASH_DATA) / 2);
  //TRUE  - ����� �����������
  //FALSE - ����� ���������
}

//���������� ����� ������� �� flash
void FlashDataSave     (void){
  if (FlashDataChk()) {//���� ������ ���������� 
    SaveFlash();   //�� ��������� ��
  }
}

//�������� �� �� ��� ������� ��������� � ���
bool FlashDataLoaded   (void){
  return bFlashDataLoaded;
}