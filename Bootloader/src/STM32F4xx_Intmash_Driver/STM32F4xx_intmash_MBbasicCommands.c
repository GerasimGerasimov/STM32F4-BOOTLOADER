#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "stm32f4xx.h"
#include "id.h"
#include "crc16.h"
#include "RAMdata.h"
#include "ModbusSlave.h"
#include "str.h"

#define CRC_SIZE 2 //������ crc-2 �����
#define WR_ANSWER_SIZE 8 //������ ������ �� ������ ������ 
#define MASK_ANSWER_SIZE 10 //������ ������ �� ������ ����� 
#define PREFIX_SHIFT 4
#define PREFIX_MASK  ((tU8)0xF0)

//������ ID ���������� � ����� + crc. ����� �� ������� �11
tU16 GetDeviceID(ModbusSlaveType* Slave){
  tU8 i=0;  
  tU8 DataLength = 0; //������ ������������ �������
  
  DataLength = getStrLenght(DeviceID);  
  Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
  do
  Slave->Buffer[MB_DATA_SECTION_CMD_11+i]=DeviceID[i];
  while ((i++)!=DataLength);
  DataLength += MB_DATA_SECTION_CMD_11;//��������� ����� ���������   
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);

  return DataLength;
}



//������� ������ ������ �� ������ �������� ������
//������ ���������� ModbusRamRead, ��� �� ���� ��������� - �� ������ ��� �� ������ ������� � ����������� �� ������
tU8 ModbusDataRead(tU8* DATA_BASE,  tU8* Buffer, tU16 RegAddr, tU16 RegNum)
{  
  //����� � ����� ������ ������ ��� �������� ������� ������ ������
  ModbusSwapCopy( (DATA_BASE+(RegAddr<<1)), (tU16*)&Buffer[MB_DATA_SECTION_CMD_03], RegNum);  
  //���������� ������ ������������� ������ ������ � ������
  tU16 DataLength=RegNum<<1;
  Buffer[MB_DATA_BYTE_NUM_CMD_03]= DataLength;
  DataLength += MB_DATA_SECTION_CMD_03 + CRC_SIZE; //+ ������ ��������� + ������ crc 
  //��������� ����� ������
  FrameEndCrc16((tU8*)Buffer, DataLength);
  return DataLength; //���������� ������ ������������ �������
}


//������� ������ ������� ������ ������ RAM, FLASH ��� CD 
tU8 ModbusMemRead(ModbusSlaveType* Slave)
{
  //������� ������� �������� ����� ������ ������� �������� ������ (�������)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  //������ ������������ �������
  tU8 DataLength = 0; 
  //��������� ����� �������� ������
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //���������� ������������� ���������  
  tU16 RegNum =  (((tU16)Slave->Buffer[MB_REG_NUM_HI_CMD_03]) << 8) + Slave->Buffer[MB_REG_NUM_LO_CMD_03];
  //��������� ����������� �������
  tU16 LastRegAddr = RegAddr + RegNum -1;
  //��������� �� ������� ������ ��� ������
  tU8* Source=0;
  
  //����� ������� ������ ��� ������ 
  //���� ������� ������ �� ������� ��� ������ �� ������������� ������� �������,
  //�� ��������� ������ ������ ��������� ������ 0.
  switch (MemSpacePrefix)
  {
    case RAM_DATA_PREFIX:
      if(LastRegAddr < RAM_DATA_SIZE) Source=(tU8*)&RAM_DATA;  
    break;
      
  /*  
  case CD_DATA_PREFIX:
      if(LastRegAddr < CD_DATA_SIZE) Source=(tU8*)&CD_DATA;
    break; 
  */  
    default:
      break;
  }
  
  //���� ��������� ����� - ������ �� ������, ���� �� ����� - ���������� ������
  if(Source==0) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR);
  else DataLength = ModbusDataRead(Source, Slave->Buffer, RegAddr, RegNum);
  
  
  return (DataLength);  
}

//������� ������ ������ � RAM ������
tU8 ModbusRamWrite(tU8* Buffer,tU8 BufDataIdx, tU16 RegAddr, tU16 RegNum){
  tU8 DataLength = 0; //������ ������������ �������

  tU16 *ModbusAddrSet = (tU16*)((tU32)&RAM_DATA + ((tU32)RegAddr << 1));
  //������ ������ � ������
  ModbusSwapCopy((tU8*)&Buffer[BufDataIdx], ModbusAddrSet, RegNum);
  /* ���� BufDataIdx = 0, ������ �� �������� �� � ������� slave, � � ����������. 
  ������� CRC � ���� ������ �� �����  */  
  if(BufDataIdx != 0)
  {
    DataLength = WR_ANSWER_SIZE; //������ ������ �� ������ ��������� (0x10, 0x06)
    FrameEndCrc16((tU8*)Buffer, DataLength);
  }
  return DataLength; //���������� ������ ������������ �������
}

/*
//������� ������ � CD
tU8 ModbusCDWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum)// 
{
  tU8 DataLength = 0; //������ ������������ �������

  //���� ������ �� ���� ��������� -�� �����, � ������ ��������� �����
  if(FlashStatus.Bits.FLASH_WRITE_DIS){
    if(BufDataIdx != 0)
    {
      DataLength = WR_ANSWER_SIZE; //������ ������ �� ������ ��������� (0x10, 0x06)
      FrameEndCrc16((tU8*)Buffer, DataLength);
    } 
    return DataLength; //���������� ������ ������������ �������
  }
  CopyFlashToTmpBuffer((tU32*)&CD_DATA);//DATA_BASE
  //����� �� ��������� ������
  tU16 *ModbusAddrSet = (tU16*)((tU32)&FlashTmpBuffer + ((tU32)RegAddr << 1));
  //������ ��������� �� ��������� ����� �� ������ ����
  ModbusSwapCopy((tU8*)&Buffer[BufDataIdx], (tU16*) ModbusAddrSet, RegNum);
  //������ ��������� �� ��������� ������, ������:
  //�������� ������ �� ���������� ������
  __disable_irq(); // handles nested interrupt
  FlashSectorWrite((tU32*)&CD_DATA, (tU32*) FlashTmpBuffer);
  __enable_irq(); // handles nested interrupt
    // ���� BufDataIdx = 0, ������ �� �������� �� � ������� slave, � � ����������. 
    //������� CRC � ���� ������ �� �����  
  if(BufDataIdx != 0)
  {
    DataLength = WR_ANSWER_SIZE; //������ ������ �� ������ ��������� (0x10, 0x06)
    FrameEndCrc16((tU8*)Buffer, DataLength);
  }
  return DataLength; //���������� ������ ������������ �������
}
*/
//����� ������� ������ � ������������ ������ ������
tU8 ModbusMemWrite(ModbusSlaveType* Slave){  
  //������� ������� �������� ����� ������ ������� �������� ������ (�������)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT;  
  //��������� ����� �������� ������
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //���������� ������ 
  tU16 RegNum =  (((tU16)Slave->Buffer[MB_REG_NUM_HI_CMD_10])<<8) + Slave->Buffer[MB_REG_NUM_LO_CMD_10];  
  //��������� ����������� �������  
  tU16 LastRegAddr = RegAddr + RegNum -1;
  //������ ������������ �������
  tU8 DataLength = 0; 
  //���������� ��� ����������� ������ ��������� � ��������������� ������
  tU8  Error=0;
  
  //����� ��������� ������ ������
  switch (MemSpacePrefix)
  {
    case RAM_DATA_PREFIX:
      if(LastRegAddr<RAM_DATA_SIZE ) DataLength=ModbusRamWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,RegAddr,RegNum);        
      else Error=1;//����� ��� ���������� ����       
    break;
    
  /*  
  case CD_DATA_PREFIX:
      if(LastRegAddr<CD_DATA_SIZE ) DataLength=ModbusCDWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum);
      else Error=1;//����� ��� ���������� ����         
    break; 
    */
    default:   //�� ����� ���������� ������� 
      Error=1;         
    break;  
  }
  //���� �����������������
  if(MB_IS_BROADCAST_MSG) DataLength = 0;  
  //���� ��� - ������� ���� �� ������ ��������� � ��������������� ������
  else if(Error) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR); 
  
  return (DataLength);
}

#define SINGLE_WRITE_REGNUM 1
tU8 ModbusMemWriteSingle(ModbusSlaveType* Slave){
  
  //������� ������� �������� ����� ������ ������� �������� ������ (�������)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  //��������� ����� �������� ������  
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //������ ������������ �������
  tU8 DataLength = 0; 
  //���������� ��� ����������� ������ ��������� � ��������������� ������  
  tU8  Error=0;
  
  
  //����� ��������� ������ ������
  switch (MemSpacePrefix)
  {
    case RAM_DATA_PREFIX:
      if(RegAddr<RAM_DATA_SIZE ) DataLength = ModbusRamWrite(Slave->Buffer,MB_DATA_SECTION_CMD_06,RegAddr,SINGLE_WRITE_REGNUM); 
      else Error=1;//����� ��� ���������� ����
    break;
    
    /*
    case CD_DATA_PREFIX:
      if(RegAddr<CD_DATA_SIZE ) DataLength = ModbusCDWrite(Slave->Buffer,MB_DATA_SECTION_CMD_06,(tU8)RegAddr,SINGLE_WRITE_REGNUM);
      else Error=1;//����� ��� ���������� ����
    break; 
    */
    default: //�� ����� ���������� ������� 
      Error=1; 
    break;  
  }
  //���� �����������������
  if(MB_IS_BROADCAST_MSG) DataLength = 0;  
  //���� ��� - ������� ���� �� ������ ��������� � ��������������� ������
  else if(Error) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR); 
  
  return (DataLength);
}


//������ ������ �������� �� �����
#define MASK_WRITE_REGNUM 1
tU8 ModbusMemWriteMask(ModbusSlaveType* Slave)
{
  //������� ������� �������� ����� ������ ������� �������� ������ (�������)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  //��������� ����� �������� ������
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //������ ������������ �������
  tU8 DataLength = 0; 
  //���������� ��� ����������� ������ ��������� � ��������������� ������
  tU8  Error=0;
  
  /*�������� �����*/
  tU16Union Mask_AND;
  tU16Union Mask_OR;  
  Mask_AND.B[0] = Slave->Buffer[MB_REG_AND_MASK_HI];
  Mask_AND.B[1] = Slave->Buffer[MB_REG_AND_MASK_LO];  
  Mask_OR.B[0] = Slave->Buffer[MB_REG_OR_MASK_HI];
  Mask_OR.B[1] = Slave->Buffer[MB_REG_OR_MASK_LO];  
  
  tU16 Data;  
  switch (MemSpacePrefix)       
  {
    case RAM_DATA_PREFIX: 
      if(RegAddr<RAM_DATA_SIZE){
        ModbusSwapCopy (((tU8*)&RAM_DATA)+(RegAddr<<1),&Data,MASK_WRITE_REGNUM);
        Data = (Data & Mask_AND.I) | Mask_OR.I;
        ModbusRamWrite((tU8*)&Data,0,RegAddr,MASK_WRITE_REGNUM);
        DataLength = MASK_ANSWER_SIZE;      
      }
      else Error=1;//����� ��� ���������� ����
    break;
    
    /*
    case CD_DATA_PREFIX:
      if(RegAddr<CD_DATA_SIZE){
        ModbusSwapCopy (((tU8*)&CD_DATA)+(RegAddr<<1),&Data,MASK_WRITE_REGNUM);          
        Data = (Data & Mask_AND.I) | Mask_OR.I;
        ModbusCDWrite((tU8*)&Data,0,(tU8)RegAddr,MASK_WRITE_REGNUM);
        DataLength = MASK_ANSWER_SIZE;
      }
      else Error=1;//����� ��� ���������� ����
    break; 
    */
    default:  //�� ����� ���������� �������    
      Error=1;
    break;
  }
  //���� �����������������
  if(MB_IS_BROADCAST_MSG) DataLength = 0;  
  //���� ��� - ������� ���� �� ������ ��������� � ��������������� ������
  else if(Error) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR); 
  
  return (DataLength);  
}
