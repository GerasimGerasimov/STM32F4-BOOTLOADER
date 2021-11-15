#include "ModbusSlave.h"
#include "crc16.h"
#include "id.h"

/*
��� ������� ������ ������ �������� ���

� ModbusSlaveConf.h ��������, ��� ��������� ������:
#include "ModbusSlave.h"

� ModbusSlaveConf.c ��������, ��� ��������� ������:
#include "ModbusSlaveConf.h"

��������� ���� �����:
ModbusMapType ModbusMap[7]={
//      1b                                 +       1b                1b              1b             
//      type                               +       prefix           size         bit_modify        pointer
  {(MB_DATA_TYPE_COILS)                 + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},
  {(MB_DATA_TYPE_COILS)                 + FLASH_DATA_PREFIX,  FLASH_DATA_SIZE,    0       ,(tU8*)&FLASH_DATA},
  {(MB_DATA_TYPE_DISCRETE_INPUTS)       + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},
  {(MB_DATA_TYPE_HOLDING_REGISTERS) + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},
  {(MB_DATA_TYPE_HOLDING_REGISTERS) + FLASH_DATA_PREFIX,  FLASH_DATA_SIZE,    0       ,(tU8*)&FLASH_DATA},
  {(MB_DATA_TYPE_INPUT_REGISTERS)   + RAM_DATA_PREFIX,    RAM_DATA_SIZE,      0       ,(tU8*)&RAM_DATA},  
  {0,0,0,0}
};
FLASD_DATA_PREFIX, CD_DATA_PREFIX ������ �������� �� �������� ����� (FLASD_DATA_PREFIX 0x2), (CD_DATA_PREFIX 0xC)


� ��������� ����� ������
���� ��������� ������ ����� �������������� ������
//����� ������������ ������ +1
ModbusCommandHandlerType ModbusCommands[11]={
  {ModbusReadCoils, 0x01},
  {ModbusReadDiscreteInputs, 0x02},
  {ModbusReadHoldingRegisters, 0x03},
  {ModbusReadInputRegisters, 0x04},
  {ModbusWriteSingleCoil, 0x05},  
  {ModbusWriteSingleRegister, 0x06}, 
  {ModbusWriteMultipleCoils, 0x0F},  
  {ModbusWriteMultipleRegisters, 0x10},  
  {ModbusReportSlaveID, 0x11},    
  {ModbusMaskWriteRegister, 0x16},  
  {0, 0x00},
}; 
*/


/*******************************************************************************
Name: ModbusCommandDecode
������������� ��������� ������� � �� ����������, ���� ��� ������� ��������
Arg:
ModbusSlaveType* Slave - ��������� �� ��������� ��������
MesageSize - ������ �������� �������
ModbusCommandHandlerType* HandlerTable - ��������� �� ��������� ������������ ������
Ret:
������ ������������ ������� � ������
*******************************************************************************/

tU8 ModbusCommandDecode(ModbusSlaveType* Slave, tU8 MesageSize, ModbusCommandHandlerType* HandlerTable)
{
  tU8 Answer=0;
  //��������� crc
  //�������� �� ������� ������ � ������, ���� ����������� ����� ������� �������
  if (crc16((tU8*)Slave->Buffer, MesageSize) == 0) 
  {
    //�������� ������ �������� � ��������� 
    if (Slave->SlaveAddress == Slave->Buffer[MB_DEVICE_ADDRESS] || MB_IS_BROADCAST_MSG ) 
    { 
      //���� ����� ������, ���� ��������� ����������������� - ����� �����������
      while (HandlerTable->CommandCode)
      {
        //���� ���������� ������� - �������������� ������
        if (HandlerTable->CommandCode == Slave->Buffer[MB_COMMAND_CODE]) 
        {
          Answer = HandlerTable->Handler(Slave);
          break;
        }
        //���� ��� - ����� ������������ 
        else HandlerTable++;
      }
      //���� ����� ����� �� ����� ������� - ���������� �� ������, ������
      if((HandlerTable->CommandCode)==0) Answer = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_FUNC);
    }
  }
  //����������� ��������� �������������
  return Answer;  
}

/*******************************************************************************
Name: ModbusError
������������ ��������� �� ������
Arg:
ModbusSlaveType* Slave - ��������� �� ��������� ��������
MesageSize - ��� ������
Ret:
������ ������������ ������� � ������
*******************************************************************************/
tU8 ModbusError(ModbusSlaveType* Slave,tU8 ModbusErrorType)
{
  tU8 DataLength = ERROR_ANSWER_SIZE; //������ ������������ �������
  Slave->Buffer[MB_COMMAND_CODE]+=MB_ERR_CODE_PREFIX; //modify command code
  Slave->Buffer[MB_ERROR_SECTION]=ModbusErrorType; //code err
  
  FrameEndCrc16(Slave->Buffer, DataLength);  
  return DataLength;
}

/*******************************************************************************
������� ����������� ������ � ����� � ������������� ����, ��� ������������ 
��������� Modbus (������� ���� ������)
*******************************************************************************/
void ModbusSwapCopy(tU8* Src, tU16* Dst, tU16 RegNum)
{
  tU16Union w;
  while (RegNum) {
    RegNum--;
    //������ � �������
    w.B[1] = *Src++;
    w.B[0] = *Src++;
    *Dst++ = w.I;
  }
}

/*******************************************************************************
����� ������ ������� ������ � ����� ������
*******************************************************************************/
#define ERROR_INDEX 0xff
tU8 GetModbusMapIndex(ModbusMapType* Map, tU8 SpaceCode){
  tU8 Index=0;
  while(Map[Index].MemSpaceCode !=0)
  {
    if (Map[Index].MemSpaceCode == SpaceCode) break;
    else Index++; 
  } 
  if(Map[Index].MemSpaceCode==0) Index=ERROR_INDEX;  
  return Index;  
}


/*******************************************************************************
������� ��������� ������� ������ ID (��� �������� 0x11) ���������!!!!!!!!!!!!!!!
*******************************************************************************/
tU8 ModbusReportSlaveID(ModbusSlaveType* Slave){  
  tU8 DataLength = 0; //������������ ����� ������������ �������
  
  //���� �� �����������������
  if(!MB_IS_BROADCAST_MSG) 
  {
    //������ ����� ������
    while (DeviceID[DataLength++]!=0);    
    Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
    //����������� ������ � ����� ������
    for(tU8 i=0; i!=DataLength; i++) Slave->Buffer[MB_DATA_SECTION_CMD_11+i]=DeviceID[i];
    //������ ������� ������    
    DataLength += MB_DATA_SECTION_CMD_11;//��������� ����� ���������   
    DataLength += CRC_SIZE;//��������� ����� crc 
    //������ ����������� �����
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  }  
  return DataLength;
}

/*******************************************************************************
������� ��� �������� ������ � ��������� � ������� � �������� ������ � ������
*******************************************************************************/
#define PREFIX_SHIFT 4
#define PREFIX_MASK  ((tU8)0xF0)
//������� ������� �������� ����� ������ ������� �������� ������ (�������)
#define MB_DATA_PREFIX   (Slave->Buffer[MB_START_ADDR_HI]>>PREFIX_SHIFT)
//������� ������� �������� ����� ������ + ������� ���� (����� ������)
#define MB_DATA_ADDR     ((((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + (Slave->Buffer[MB_START_ADDR_LO]))



/*******************************************************************************
����������� ������� ������ ��������� (holding � input registers)
*******************************************************************************/
#define REG_NUM_TO_READ_MAX 125
tU8 ModbusRegRead(ModbusSlaveType* Slave, tU8 DataType)
{
  //������ ������������ �������
  tU8 DataLength = 0; 
  //��� ������� ������ � ����� 
  tU8 SpaceCode = MB_DATA_PREFIX + DataType;
  //��������� ����� ������  
  tU16 RegAddr = MB_DATA_ADDR;
  //���������� ������ (���������) 
  tU16 RegNum = Slave->Buffer[MB_REG_NUM_LO_CMD_03]; 
  //������ ������ ������� ������ � �����
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //���� ��������� ����������������� - �� ��������
  if(!MB_IS_BROADCAST_MSG)
  {
    //���� ������� ������ �� ������� - ��������������� ������
    if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
    //�������� ���������� ������������� ��������� 
    else if(RegNum>REG_NUM_TO_READ_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
    //����� �������� �� ������������ ������ ������������� ��������� �� �������� �������
    else if((RegAddr+RegNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
    //��� �������� ������ - ������ ������ ������, ��������� �������  
    else
    {
      //�������� ������ ������ � ����� �������
      ModbusSwapCopy( (Slave->ModbusMap[ModbusMapIndex].DataPointer+(RegAddr<<1)), (tU16*)&Slave->Buffer[MB_DATA_SECTION_CMD_03], RegNum);  
      //���������� ������ ������������� ������ ������ � ������
      DataLength=(tU16)(RegNum << 1);
      Slave->Buffer[MB_DATA_BYTE_NUM_CMD_03]= DataLength;
      DataLength += MB_DATA_SECTION_CMD_03 + CRC_SIZE; 
      //��������� ����� ������
      FrameEndCrc16((tU8*)Slave->Buffer, DataLength);    
    }
  }
  return DataLength;
}

//Read Holding Registers - 0x03
tU8 ModbusReadHoldingRegisters(ModbusSlaveType* Slave){ 
  return ModbusRegRead(Slave,MB_DATA_TYPE_HOLDING_REGISTERS);  
}

//Read Input Registers - 0x04
tU8 ModbusReadInputRegisters(ModbusSlaveType* Slave){ 
  return ModbusRegRead(Slave,MB_DATA_TYPE_INPUT_REGISTERS);
}


/*******************************************************************************
����������� ������� ������ ��� (coils � discrete inputs) ���������!!!!!!!!!!!!!!
*******************************************************************************/
#define BIT_NUM_TO_READ_MAX 2000

tU8 ModbusBitRead(ModbusSlaveType* Slave,tU8 DataType)
{ 
  //������ ������������ �������
  tU8 DataLength = 0; 
  //��� ������� ������ � ����� 
  tU8 SpaceCode = MB_DATA_PREFIX + DataType;
  //��������� ����� ������  
  tU16 BitAddr = MB_DATA_ADDR;
  //���������� ������ (���) 
  tU16 BitNum = (((tU16)(Slave->Buffer[MB_BITS_NUM_HI_CMD_01])) << 8) + Slave->Buffer[MB_BITS_NUM_LO_CMD_01]; 
  //������ ������ ������� ������ � �����
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //���� ��������� ����������������� - �� ��������
  if(!MB_IS_BROADCAST_MSG)
  {
    //���� ������� ������ �� ������� - ��������������� ������
    if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
    //�������� ���������� ������������� ��� 
    else if(BitNum>BIT_NUM_TO_READ_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
    //����� �������� �� ������������ ������ ������������� ��� �� �������� �������
    else if((BitAddr+BitNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
    //��� �������� ������ - ������ ������ ������, ��������� �������  
    else
    {
      //��������
      tU8 *Src = (Slave->ModbusMap[ModbusMapIndex].DataPointer+(tU8)(BitAddr/8));
      //��������
      tU8 *Dst = &Slave->Buffer[MB_DATA_SECTION_CMD_READ01];   
      //�������������� ���������� ������������� ��� � �����
      tU8 ByteNum;
      if ((BitNum % 8) == 0) ByteNum = (tU8)(BitNum / 8);
      else ByteNum = (tU8)(BitNum / 8) + 1;
      //���������� ������ ������������� ������ ������ � ������   
      DataLength = ByteNum;
      Slave->Buffer[MB_DATA_BYTE_NUM_CMD_READ01]= DataLength;
      DataLength += MB_DATA_SECTION_CMD_READ01 + CRC_SIZE;
      //�������� �������
      tU8 BitOffset = (tU8)(BitAddr % 8);//����� ���� � �����, � �������� �������� �������� �����
      tU8 Byte;//�������������� ���� �� ��������
      while (ByteNum){  
        ByteNum--;
        //�� �������� ����� ����� ������� ����� �� ������ 
        Byte = *(Src++)>>BitOffset;
        //�� �������� ����� ����� ������� ����� �� ������
        Byte = Byte | ((*Src << ( 8 - BitOffset)) & 0xFF);
        //�������� ��������� ���� �������� ���������� ��� ��� ��������� ��������� ���� �� 8 ���
        if(ByteNum == 0) Byte = Byte & ((1<<BitNum)-1);    
        else  BitNum -= 8; 
        //���������� ��� � �����
        *Dst++ = Byte; 
      }  
      //��������� ����� ������
      FrameEndCrc16((tU8*)Slave->Buffer, DataLength);      
    }
  }
  return DataLength; 
}

//Read Coils - 0x01
tU8 ModbusReadCoils(ModbusSlaveType* Slave){
  return ModbusBitRead(Slave,MB_DATA_TYPE_COILS);
}
//Read Discrete Inputs - 0x02
tU8 ModbusReadDiscreteInputs(ModbusSlaveType* Slave){    
  return ModbusBitRead(Slave,MB_DATA_TYPE_DISCRETE_INPUTS);
}

/*******************************************************************************
����������� ������� ������ ��������� (holding registers)
*******************************************************************************/
#define REG_NUM_TO_WRITE_MAX 123

tU8 ModbusRegWrite(ModbusSlaveType* Slave, tU16 RegNum, tU8 DataSectionIndex)
{
  //������ ������������ �������
  tU8 DataLength = 0; 
  //��� ������� ������ � ����� 
  tU8 SpaceCode = MB_DATA_PREFIX + MB_DATA_TYPE_HOLDING_REGISTERS;
  //��������� ����� ������  
  tU16 RegAddr = MB_DATA_ADDR;
  //������ ������ ������� ������ � �����
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //���� ������� ������ �� ������� - ��������������� ������
  if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
  //�������� ���������� ������������� ��������� 
  else if(RegNum>REG_NUM_TO_WRITE_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
  //����� �������� �� ������������ ������ ������������� ��������� �� �������� �������
  else if((RegAddr+RegNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
  //��� �������� ������ - ���������� ������ ������  
  else
  {
    tU16* MemDestinationAddress=(tU16*)((Slave->ModbusMap[ModbusMapIndex].DataPointer) + (RegAddr << 1));
    ModbusSwapCopy((tU8*)&Slave->Buffer[DataSectionIndex], MemDestinationAddress, RegNum);
    // ������ ���� ��������� ������ � �������
    Slave->ModbusMap[ModbusMapIndex].ModifyFlag = 1;
  }
  //���� ��������� ����������������� - �� ��������
  if(!MB_IS_BROADCAST_MSG)
  { 
    //������ ������ �� ������ 
    DataLength = WR_ANSWER_SIZE; 
    //��������� ����� ������
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength);     
  }
  return DataLength;  
}

//Write Single Register - 0x06
tU8 ModbusWriteSingleRegister(ModbusSlaveType* Slave)
{
  return ModbusRegWrite(Slave, 1, MB_DATA_SECTION_CMD_06);
}

//Write Multiple Registers - 0x10
tU8 ModbusWriteMultipleRegisters(ModbusSlaveType* Slave)
{  
  return ModbusRegWrite(Slave, Slave->Buffer[MB_REG_NUM_LO_CMD_10], MB_DATA_SECTION_CMD_10);
}

//Mask Write Register - 0x16
tU8 ModbusMaskWriteRegister(ModbusSlaveType* Slave){
  //������ ������������ �������
  tU8 DataLength = 0; 
  //��� ������� ������ � ����� 
  tU8 SpaceCode = MB_DATA_PREFIX + MB_DATA_TYPE_HOLDING_REGISTERS;
  //��������� ����� ������  
  tU16 RegAddr = MB_DATA_ADDR;
  //������ ������ ������� ������ � �����
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //���� ������� ������ �� ������� - ��������������� ������
  if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR); 
  //����� �������� �� ������������ ������ ������������� ��������� �� �������� �������
  else if((RegAddr+1)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
  //��� �������� ������ - ���������� ������ ������, ��������� �������  
  else
  {
    //����� �������� � ������, � ������� ����� ��������
    tU8* MemAddress=(tU8*)(Slave->ModbusMap[ModbusMapIndex].DataPointer + (RegAddr << 1));
    //���������� ��� ���������� �������� � ������ ����� �������
    tU16Union Mask;//�����
    tU16 Data;//������
    //�������� �������� �� ������
    ModbusSwapCopy(MemAddress,&Data,1);
    //��������������
    Mask.B[0] = Slave->Buffer[MB_REG_AND_MASK_HI];
    Mask.B[1] = Slave->Buffer[MB_REG_AND_MASK_LO];  
    Data &= Mask.I;
    Mask.B[0] = Slave->Buffer[MB_REG_OR_MASK_HI];
    Mask.B[1] = Slave->Buffer[MB_REG_OR_MASK_LO];
    Data |= Mask.I;
    //�������� �������
    ModbusSwapCopy((tU8*)&Data, (tU16*)MemAddress, 1);
    // ������ ���� ��������� ������ � �������
    Slave->ModbusMap[ModbusMapIndex].ModifyFlag = 1;
  }
  //���� ��������� �� ����������������� - ��������
  if(!MB_IS_BROADCAST_MSG)
  {
    //������ ������ �� ������    
    DataLength = WR_ANSWER_SIZE; 
    //��������� ����� ������
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength);     
  }
  return DataLength;   
}

/*******************************************************************************
����������� ������� ������ ��� (coils) ���������!!!!!!!!!!!!!!!
*******************************************************************************/
#define BIT_NUM_TO_WRITE_MAX 2000

tU8 ModbusBitWrite(ModbusSlaveType* Slave, tU16 BitNum, tU8 DataSectionIndex)
{
  //������ ������������ �������
  tU8 DataLength = 0; 
  //��� ������� ������ � ����� 
  tU8 SpaceCode = MB_DATA_PREFIX + MB_DATA_TYPE_COILS;
  //��������� ����� ������  
  tU16 BitAddr = MB_DATA_ADDR;  
  //������ ������ ������� ������ � �����
  tU8 ModbusMapIndex = GetModbusMapIndex(Slave->ModbusMap, SpaceCode);
  //���� ������� ������ �� ������� - ��������������� ������
  if(ModbusMapIndex == ERROR_INDEX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);  
  //�������� ���������� ������������� ��� 
  else if(BitNum>BIT_NUM_TO_WRITE_MAX) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);
  //����� �������� �� ������������ ������ ������������� ��� �� �������� �������
  else if((BitAddr+BitNum)>(Slave->ModbusMap[ModbusMapIndex].Size)) DataLength = ModbusError(Slave, MB_ERR_CODE_ILLEGAL_ADDR);    
  //��� �������� ������ - ���������� ������ ������ 
  else
  {
    tU16 i = 0;      
    while (i < BitNum)
    {
      //����� �������� ����� ������
      tU8 ByteAddr = (tU8) (BitAddr /8) ;      
      //����� ����� � ������ ����������
      tU8* MemDestinationAddress = Slave->ModbusMap[ModbusMapIndex].DataPointer + ByteAddr;   
      //����� ��� �������������� ����      
      tU8 BitMask = 1 << ((tU8) (BitAddr%8));
      //��������� ����, � ������� ����� ���������
      tU8 BitState = (Slave->Buffer[DataSectionIndex + (tU8)(i/8)]) & (1<<(i%8));        
      //���������� ��������� ���� 
      if(BitState) *MemDestinationAddress |=  BitMask;
      else *MemDestinationAddress &= (~BitMask);          
      i+=1;     
      BitAddr += 1;// ����� ���������� ����  
    }
    // ������ ���� ��������� ������ � �������
    Slave->ModbusMap[ModbusMapIndex].ModifyFlag = 1;    
  }
  //���� ��������� �� ����������������� - ��������
  if(!MB_IS_BROADCAST_MSG)
  {
    //������ ������ �� ������
    DataLength = WR_ANSWER_SIZE; 
    //��������� ����� ������
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength); 
  }
  return DataLength; 
}

//Write Single Coil - 0x05
tU8 ModbusWriteSingleCoil(ModbusSlaveType* Slave)
{
  //������ ������������ �������
  tU8 DataLength;
  //��� ��������� ����: FF - 1, 00 - 0 � � ������� ����� ������ ����  
  //��������� ��� ��������� ������
  if ((Slave->Buffer[MB_BIT_STATE] != 0xFF) && (Slave->Buffer[MB_BIT_STATE] != 0) || (Slave->Buffer[MB_BIT_STATE +1] != 0))   
    DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_DATA);
  else
    DataLength = ModbusBitWrite(Slave, 1, MB_DATA_SECTION_CMD_05);
 
  return DataLength;    
}

//Write Multiple Coils - 0x0F
tU8 ModbusWriteMultipleCoils(ModbusSlaveType* Slave){
  
  //������ ������������ �������
  tU8 DataLength;  
  //�������� ���������� ��� �� �������
  tU16 BitNum = (Slave->Buffer[MB_BITS_NUM_HI_CMD_0F] << 8) + Slave->Buffer[MB_BITS_NUM_LO_CMD_0F];   
  
  //���� ���������� ���� � ������� �� ������� � ����������� ��� � �������
  if ((1 + (BitNum -1) / 8) != Slave->Buffer[MB_DATA_BYTE_NUM_CMD_0F])
    DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_DATA);
  else     
    DataLength = ModbusBitWrite(Slave, BitNum, MB_DATA_SECTION_CMD_0F);
  
  return DataLength;    
}


