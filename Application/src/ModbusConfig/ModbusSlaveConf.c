#include "ModbusSlaveConf.h"
#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_USART.h"
#include "Intmash_bastypes.h" 
#include "DExS.M4CPU_BoardLEDs.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "RAMData.h"
#include "crc16.h"
#include "CalibrationData.h"
//#include "resources.h"
#include "str.h"

//��������� ��������� UART � �������
Intmash_Usart UARTtoUSB; // ����� �� USB
ModbusSlaveType USBslave; 

Intmash_Usart UARTtoRS485; // ����� �� RS485
ModbusSlaveType RS485slave;

Intmash_Usart UARTtoOptRS485; // ����� � ������������ RS485
ModbusSlaveType OptRS485slave;

tU16 ModbusMemWrite_VTEG(ModbusSlaveType* Slave);
tU16 ModbusMemRead_VTEG(ModbusSlaveType* Slave);
tU16 StartBootLoader(ModbusSlaveType* Slave);
tU16 getResurceItemName(ModbusSlaveType* Slave);


//����� ������������ ������ +1
ModbusCommandHandlerType ModbusCommands[6]={
  {ModbusMemRead_VTEG, 0x03},
  {ModbusMemWrite_VTEG, 0x10},
  {GetDeviceID, 0x11},
  {StartBootLoader, 0xB0},
  {getResurceItemName, 0xB1},
  {0, 0},
}; 


void ModbusClientInit(void) //������ ������������� ���������, ������
{ 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB1Periph_USART2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  //USART3, USB 
  UARTtoUSB.USARTx = USART3;
  if(FlashStatus.Bits.FLASH_DATA_ERR) {
    UARTtoUSB.USART_BaudRate = 115200;
    USBslave.SlaveAddress = 0x01;
  }
  else {
    //UARTtoUSB.USART_BaudRate = USARTbaudRate[CD_DATA.Modbus_USB.B[1]];;
    //USBslave.SlaveAddress = CD_DATA.Modbus_USB.B[0];
    UARTtoUSB.USART_BaudRate = 115200;
    USBslave.SlaveAddress = 0x01;
  }   
  UARTtoUSB.DMAy_StreamRX = DMA1_Stream1;
  UARTtoUSB.DMAy_StreamTX = DMA1_Stream3;
  UARTtoUSB.DMA_channel = DMA_Channel_4;
  UARTtoUSB.DMA_FLAGS_RX = DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1;
  UARTtoUSB.DMA_FLAGS_TX = DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3;  
  UARTtoUSB.PolarityDIR = WITHOUT_DIR; //���� ���
  UARTtoUSB.USART_StopBits=0;//����-��� �� ��������� 1
  UARTtoUSB.USART_Parity=0;//��� �������� �� ��������� �����������  
  UsartDriverInit (&UARTtoUSB); //������������� ���������
  UsartRecieve (&UARTtoUSB, &USBslave.Buffer[0]);//��������� �� ����� � ����
  
  //USART2
  UARTtoRS485.USARTx = USART2;
  UARTtoRS485.USART_BaudRate = 115200;//USARTbaudRate[CD_DATA.Modbus_RS485.B[1]];
  RS485slave.SlaveAddress = 1;//CD_DATA.Modbus_RS485.B[0];
  UARTtoRS485.DMAy_StreamRX = DMA1_Stream5;
  UARTtoRS485.DMAy_StreamTX = DMA1_Stream6;
  UARTtoRS485.DMA_channel = DMA_Channel_4;
  UARTtoRS485.DMA_FLAGS_RX = DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5;
  UARTtoRS485.DMA_FLAGS_TX = DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6;
  UARTtoRS485.GPIOxDIR = GPIOE;
  UARTtoRS485.GPIO_PinDIR = GPIO_Pin_6;
  UARTtoRS485.PolarityDIR = POLARITY_HI; //����������� ����������
  UARTtoRS485.USART_StopBits=0;//����-��� �� ��������� 1
  UARTtoRS485.USART_Parity=0;//��� �������� �� ��������� �����������
  UsartDriverInit (&UARTtoRS485); //������������� ���������
  UsartRecieve(&UARTtoRS485, &RS485slave.Buffer[0]);//��������� �� ����� � ����
  
  //USART1
  UARTtoOptRS485.USARTx = USART1;
  UARTtoOptRS485.USART_BaudRate = 115200;//USARTbaudRate[CD_DATA.Modbus_option.B[1]];
  OptRS485slave.SlaveAddress = 1;//CD_DATA.Modbus_option.B[0];
  UARTtoOptRS485.DMAy_StreamRX = DMA2_Stream2;
  UARTtoOptRS485.DMAy_StreamTX = DMA2_Stream7;
  UARTtoOptRS485.USART_StopBits=0;
  UARTtoOptRS485.USART_Parity=0;
  UARTtoOptRS485.DMA_channel = DMA_Channel_4;
  UARTtoOptRS485.DMA_FLAGS_RX = DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2 | DMA_FLAG_TCIF2;
  UARTtoOptRS485.DMA_FLAGS_TX = DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7;
  UARTtoOptRS485.GPIOxDIR = GPIOA;
  UARTtoOptRS485.GPIO_PinDIR = GPIO_Pin_8;
  UARTtoOptRS485.PolarityDIR = POLARITY_LOW; //��������� ���������� 
  UsartDriverInit (&UARTtoOptRS485); //������������� ���������
  UsartRecieve (&UARTtoOptRS485, &OptRS485slave.Buffer[0]);//��������� �� ����� � ����
  
}


tU8 CheckFilesCMD=0;
//���������� USART �� USB
tU8 CheckMsgUSB=0;
void USART3_IRQHandler(void)
{
  tU16 MsgSize = UsartTxRxFinish(&UARTtoUSB);//���������� ��� ���������: ����� ��� ������� MsgSize ����. 
  if(MsgSize){//���� ��������  
    if(LED2_ST) LED2_ON; else LED2_OFF; //�������� ��������
    //�������� ������� �������������, ���������� ������ ������ ��� ��������
    MsgSize = ModbusCommandDecode(&USBslave, MsgSize, ModbusCommands); 
    //���� ���� ��� ���������� - ����������, ���� ��� - �������� �� �����.
    
    if (CheckFilesCMD)//���� � ��� ������� ������ � ������ - ������������ �� � main
    {
      CheckMsgUSB = MsgSize; //��������� ����� ������� 
      CheckFilesCMD=0;
      return; 
    } 

    if(MsgSize) UsartTransmit(&UARTtoUSB, &USBslave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoUSB, &USBslave.Buffer[0]);
  }  
  else {
    UsartRecieve(&UARTtoUSB, &USBslave.Buffer[0]);//���� ����������, �������� �� �����.
    //� ������� ��� �������� ����� �������� ������� �������� ��������. ����, � �� ��������� �� ���.  
  }
}


//���������� USART �� RS485
tU8 CheckMsgRS485=0;
void USART2_IRQHandler(void)
{
  
  tU16 MsgSize = UsartTxRxFinish(&UARTtoRS485);//���������� ��� ���������: ����� ��� ������� MsgSize ����.
  if(MsgSize){//���� ��������  
    if(LED2_ST) LED2_ON; else LED2_OFF; //�������� ��������
    //�������� ������� �������������, ���������� ������ ������ ��� ��������
    MsgSize = ModbusCommandDecode(&RS485slave, MsgSize, ModbusCommands); 
    
    if (CheckFilesCMD)
    {
      CheckMsgRS485 = MsgSize; //��������� ����� ������� 
      CheckFilesCMD=0;
      return; 
    }
    
    //���� ���� ��� ���������� - ����������, ���� ��� - �������� �� �����.
    if(MsgSize) UsartTransmit(&UARTtoRS485, &RS485slave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoRS485, &RS485slave.Buffer[0]);
  }  
  else {
    UsartRecieve(&UARTtoRS485, &RS485slave.Buffer[0]);//���� ����������, �������� �� �����.
    //� ������� ��� �������� ����� �������� ������� �������� ��������. ����, � �� ��������� �� ���.  
  }
  
  
}


//���������� USART �� ������������� RS485
tU8 CheckMsgOptRS485=0;
void USART1_IRQHandler(void)
{
  
  tU16 MsgSize = UsartTxRxFinish(&UARTtoOptRS485);//���������� ��� ���������: ����� ��� ������� MsgSize ����.
  if(MsgSize){//���� ��������  
    if(LED2_ST) LED2_ON; else LED2_OFF; //�������� ��������
    //�������� ������� �������������, ���������� ������ ������ ��� ��������
    MsgSize = ModbusCommandDecode(&OptRS485slave, MsgSize, ModbusCommands); 
    
    if (CheckFilesCMD)//���� � ��� ������� ������ � ������ - ������������ �� � main
    {
      CheckMsgOptRS485 = MsgSize; //��������� ����� ������� 
      CheckFilesCMD=0;
      return; 
    }
    //���� ���� ��� ���������� - ����������, ���� ��� - �������� �� �����.
    if(MsgSize) UsartTransmit(&UARTtoOptRS485, &OptRS485slave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoOptRS485, &OptRS485slave.Buffer[0]);
  }  
  else {
    UsartRecieve(&UARTtoOptRS485, &OptRS485slave.Buffer[0]);//���� ����������, �������� �� �����.
    //� ������� ��� �������� ����� �������� ������� �������� ��������. ����, � �� ��������� �� ���.  
  }
  
}


tU8 ModbusFilesCMD(ModbusSlaveType* Slave)
{
  CheckFilesCMD = 1; //���� ������� ������� ������ � �������
  return Slave->Buffer[MB_COMMAND_CODE]; //���������� ����� �������
}

void ModbusSlaveProc(void)
{
  tU16 MsgSize = 0;
  if(CheckMsgUSB){   
    if(MsgSize) UsartTransmit(&UARTtoUSB, &USBslave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoUSB, &USBslave.Buffer[0]);
    CheckMsgUSB=0;
  }

  if(CheckMsgRS485){
    if(MsgSize) UsartTransmit(&UARTtoRS485, &RS485slave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoRS485, &RS485slave.Buffer[0]);
    CheckMsgRS485=0;
  }
  
  if(CheckMsgOptRS485){
    if(MsgSize) UsartTransmit(&UARTtoOptRS485, &OptRS485slave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoOptRS485, &OptRS485slave.Buffer[0]);
    CheckMsgOptRS485=0;
  } 
}


tU8 ModbusCDRCWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum){ //u32 DATA_BASE
  tU8 DataLength = 0; //������ ������������ �������
  
  tU16 *ModbusAddrSet = (tU16*)((tU32)&CD_DATA + ((tU32)RegAddr << 1));
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

#define PREFIX_SHIFT 4
#define PREFIX_MASK  ((tU8)0xF0)


//����� ������� ������ � ������������ ������ ������
tU16 ModbusMemWrite_VTEG(ModbusSlaveType* Slave){
  
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
      if(LastRegAddr<RAM_DATA_SIZE ) 
      {
        DataLength=ModbusRamWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,RegAddr,RegNum);   
      }
      else Error=1;//����� ��� ���������� ����       
    break;
    
    case CD_DATA_PREFIX:
      if(LastRegAddr<CD_DATA_SIZE ) {
          DataLength=ModbusCDWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum); 
      }
      else Error=1;//����� ��� ���������� ����         
    break; 
    
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


//������� ������ ������� ������ ������ RAM, FLASH ��� CD 
tU16 ModbusMemRead_VTEG(ModbusSlaveType* Slave)
{
  //������� ������� �������� ����� ������ ������� �������� ������ (�������)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  tU8 DataLength = 0; //������ ������������ �������
  //��������� ����� �������� ������
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //���������� ������������� ���������  
  tU16 RegNum =  (((tU16)Slave->Buffer[MB_REG_NUM_HI_CMD_03]) << 8) + Slave->Buffer[MB_REG_NUM_LO_CMD_03];
  //��������� ����������� �������
  tU16 LastRegAddr = RegAddr + RegNum -1;
  //��������� �� ������� ������ ��� ������
  tU8* Source=0; 
  bool isFlashRead = false;
  
  //����� ��������� ������ ������  
  switch (MemSpacePrefix)
  {
  case RAM_DATA_PREFIX:
    if(LastRegAddr < RAM_DATA_SIZE) {
      Source=(tU8*)&RAM_DATA;  
    }
    break;
    
  case CD_DATA_PREFIX:
      if(LastRegAddr < CD_DATA_SIZE) {
        Source=(tU8*)&CD_DATA;
      }
    break; 
    
  default:
    //�� ����� ���������� �������      
    break;
  }
    //���� ��������� ����� - ������ �� ������, ���� �� ����� - ���������� ������
  if(Source==0) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR);
  else DataLength = ModbusDataRead(Source, Slave->Buffer, RegAddr, RegNum);
  
  if (isFlashRead) RAM_DATA.FLAGS0.bits.FLASH_WRITE = 0;//������� ��������� 
  return (DataLength);  
}

__no_init char BootLoaderStart[6] @ "BOOT_CMD";
tU16 StartBootLoader(ModbusSlaveType* Slave) {
  BootLoaderStart[0] = 0xA5;
  BootLoaderStart[1] = 0x5A;
  BootLoaderStart[2] = 0xA5;
  BootLoaderStart[3] = 0x5A; 
  //FrameEndCrc16((tU8*)BootLoaderStart, 4);
  NVIC_SystemReset();
  return 4;
}

void BootLoadCmdFillZero(void) {
  BootLoaderStart[0] = 0x00;
  BootLoaderStart[1] = 0x00;
  BootLoaderStart[2] = 0x00;
  BootLoaderStart[3] = 0x00; 
  BootLoaderStart[4] = 0x00;
  BootLoaderStart[5] = 0x00;   
}

/*
tU16 getResourceHeader(ModbusSlaveType* Slave) { 
  tU8 DataLength = 0; //������ ������������ �������
  u16 ResSize = getResourcesSize();
  u16 NumberOfItems = getResourcesNumberOfItems();
  DataLength = 4;  
  Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
  
  Slave->Buffer[MB_DATA_SECTION_CMD_11+0]= (ResSize >> 0) & 0x00FF;
  Slave->Buffer[MB_DATA_SECTION_CMD_11+1]= (ResSize >> 8) & 0x00FF;
  
  Slave->Buffer[MB_DATA_SECTION_CMD_11+2]= (NumberOfItems >> 0) & 0x00FF;
  Slave->Buffer[MB_DATA_SECTION_CMD_11+3]= (NumberOfItems >> 8) & 0x00FF;
  
  //DataLength += 2;
  DataLength += MB_DATA_SECTION_CMD_11;//��������� ����� ���������   
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);

  return DataLength;
}
*/
tU16 getResurceItemName(ModbusSlaveType* Slave) {
  tU8 i=0;  
  tU8 DataLength = 0;
  u16 idx = (u16)Slave->Buffer[2];
  char * data = 0;//TIntResources::getItemName(idx);
  DataLength = getStrLenght(data);  
  Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
  do
    Slave->Buffer[MB_DATA_SECTION_CMD_11+i]=data[i];
  while ((i++)!=DataLength);
  DataLength += MB_DATA_SECTION_CMD_11;//��������� ����� ���������   
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);

  return DataLength;
}

//gets the Resources Header and Table
/*
tU16 getResourcesHeaderAndTable(ModbusSlaveType* Slave) {
  tU8 i=0;  
  tU8 DataLength = 0; //������ ������������ �������
  u8 * data = getRes();
  DataLength = 60;  
  Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
  do
  Slave->Buffer[MB_DATA_SECTION_CMD_11+i]=data[i];
  while ((i++)!=DataLength);
  DataLength += MB_DATA_SECTION_CMD_11;//��������� ����� ���������   
  DataLength += CRC_SIZE;//��������� ����� crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);

  return DataLength;
}


*/





