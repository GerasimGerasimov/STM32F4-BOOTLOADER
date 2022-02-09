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

//структуры драйверов UART и слейвов
Intmash_Usart UARTtoUSB; // связь по USB
ModbusSlaveType USBslave; 

Intmash_Usart UARTtoRS485; // связь по RS485
ModbusSlaveType RS485slave;

Intmash_Usart UARTtoOptRS485; // связь с опциональным RS485
ModbusSlaveType OptRS485slave;

tU16 ModbusMemWrite_VTEG(ModbusSlaveType* Slave);
tU16 ModbusMemRead_VTEG(ModbusSlaveType* Slave);
tU16 StartBootLoader(ModbusSlaveType* Slave);
tU16 getResurceItemName(ModbusSlaveType* Slave);


//число используемых команд +1
ModbusCommandHandlerType ModbusCommands[6]={
  {ModbusMemRead_VTEG, 0x03},
  {ModbusMemWrite_VTEG, 0x10},
  {GetDeviceID, 0x11},
  {StartBootLoader, 0xB0},
  {getResurceItemName, 0xB1},
  {0, 0},
}; 


void ModbusClientInit(void) //фукция инициализации структуры, пример
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
  UARTtoUSB.PolarityDIR = WITHOUT_DIR; //дира нет
  UARTtoUSB.USART_StopBits=0;//стоп-бит по умолчанию 1
  UARTtoUSB.USART_Parity=0;//бит четности по умолчанию отсутствует  
  UsartDriverInit (&UARTtoUSB); //инициализация периферии
  UsartRecieve (&UARTtoUSB, &USBslave.Buffer[0]);//настроили на прием и ждем
  
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
  UARTtoRS485.PolarityDIR = POLARITY_HI; //неинверсная полярность
  UARTtoRS485.USART_StopBits=0;//стоп-бит по умолчанию 1
  UARTtoRS485.USART_Parity=0;//бит четности по умолчанию отсутствует
  UsartDriverInit (&UARTtoRS485); //инициализация периферии
  UsartRecieve(&UARTtoRS485, &RS485slave.Buffer[0]);//настроили на прием и ждем
  
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
  UARTtoOptRS485.PolarityDIR = POLARITY_LOW; //инверсная полярность 
  UsartDriverInit (&UARTtoOptRS485); //инициализация периферии
  UsartRecieve (&UARTtoOptRS485, &OptRS485slave.Buffer[0]);//настроили на прием и ждем
  
}


tU8 CheckFilesCMD=0;
//прерывание USART от USB
tU8 CheckMsgUSB=0;
void USART3_IRQHandler(void)
{
  tU16 MsgSize = UsartTxRxFinish(&UARTtoUSB);//определили что произошло: прием или принято MsgSize байт. 
  if(MsgSize){//если принятно  
    if(LED2_ST) LED2_ON; else LED2_OFF; //моргнули светиком
    //вызываем функцию декодирования, определяем размер пакета для отправки
    MsgSize = ModbusCommandDecode(&USBslave, MsgSize, ModbusCommands); 
    //если есть что отправлять - отправляем, если нет - заряжаем на прием.
    
    if (CheckFilesCMD)//если у нас функция работы с файлом - обрабатываем ее в main
    {
      CheckMsgUSB = MsgSize; //сохранили номер команды 
      CheckFilesCMD=0;
      return; 
    } 

    if(MsgSize) UsartTransmit(&UARTtoUSB, &USBslave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoUSB, &USBslave.Buffer[0]);
  }  
  else {
    UsartRecieve(&UARTtoUSB, &USBslave.Buffer[0]);//если отправлено, заряжаем на прием.
    //в будущем при отправке будем вызывать функцию файловых операций. типа, а не отправить ли еще.  
  }
}


//прерывание USART от RS485
tU8 CheckMsgRS485=0;
void USART2_IRQHandler(void)
{
  
  tU16 MsgSize = UsartTxRxFinish(&UARTtoRS485);//определили что произошло: прием или принято MsgSize байт.
  if(MsgSize){//если принятно  
    if(LED2_ST) LED2_ON; else LED2_OFF; //моргнули светиком
    //вызываем функцию декодирования, определяем размер пакета для отправки
    MsgSize = ModbusCommandDecode(&RS485slave, MsgSize, ModbusCommands); 
    
    if (CheckFilesCMD)
    {
      CheckMsgRS485 = MsgSize; //сохранили номер команды 
      CheckFilesCMD=0;
      return; 
    }
    
    //если есть что отправлять - отправляем, если нет - заряжаем на прием.
    if(MsgSize) UsartTransmit(&UARTtoRS485, &RS485slave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoRS485, &RS485slave.Buffer[0]);
  }  
  else {
    UsartRecieve(&UARTtoRS485, &RS485slave.Buffer[0]);//если отправлено, заряжаем на прием.
    //в будущем при отправке будем вызывать функцию файловых операций. типа, а не отправить ли еще.  
  }
  
  
}


//прерывание USART от опционального RS485
tU8 CheckMsgOptRS485=0;
void USART1_IRQHandler(void)
{
  
  tU16 MsgSize = UsartTxRxFinish(&UARTtoOptRS485);//определили что произошло: прием или принято MsgSize байт.
  if(MsgSize){//если принятно  
    if(LED2_ST) LED2_ON; else LED2_OFF; //моргнули светиком
    //вызываем функцию декодирования, определяем размер пакета для отправки
    MsgSize = ModbusCommandDecode(&OptRS485slave, MsgSize, ModbusCommands); 
    
    if (CheckFilesCMD)//если у нас функция работы с файлом - обрабатываем ее в main
    {
      CheckMsgOptRS485 = MsgSize; //сохранили номер команды 
      CheckFilesCMD=0;
      return; 
    }
    //если есть что отправлять - отправляем, если нет - заряжаем на прием.
    if(MsgSize) UsartTransmit(&UARTtoOptRS485, &OptRS485slave.Buffer[0], MsgSize);
    else UsartRecieve(&UARTtoOptRS485, &OptRS485slave.Buffer[0]);
  }  
  else {
    UsartRecieve(&UARTtoOptRS485, &OptRS485slave.Buffer[0]);//если отправлено, заряжаем на прием.
    //в будущем при отправке будем вызывать функцию файловых операций. типа, а не отправить ли еще.  
  }
  
}


tU8 ModbusFilesCMD(ModbusSlaveType* Slave)
{
  CheckFilesCMD = 1; //была вызвана команда работы с файлами
  return Slave->Buffer[MB_COMMAND_CODE]; //возвращает номер команды
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
  tU8 DataLength = 0; //длинна отправляемой посылки
  
  tU16 *ModbusAddrSet = (tU16*)((tU32)&CD_DATA + ((tU32)RegAddr << 1));
  //чтение буфера в память
  ModbusSwapCopy((tU8*)&Buffer[BufDataIdx], ModbusAddrSet, RegNum);
  /* если BufDataIdx = 0, значит мы работаем не с буфером slave, а с переменной. 
  считать CRC в этом случае не нужно  */  
  if(BufDataIdx != 0)
  {
    DataLength = WR_ANSWER_SIZE; //размер ответа на запись регистров (0x10, 0x06)
    FrameEndCrc16((tU8*)Buffer, DataLength);
  }
  return DataLength; //возвращает размер отправляемой посылки
}

#define PREFIX_SHIFT 4
#define PREFIX_MASK  ((tU8)0xF0)


//выбор функции записи в определенный сектор памяти
tU16 ModbusMemWrite_VTEG(ModbusSlaveType* Slave){
  
  //старшая тетрада старшего байта адреса первого регистра данных (префикс)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT;  
  //начальный адрес регистра данных
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //количество данных 
  tU16 RegNum =  (((tU16)Slave->Buffer[MB_REG_NUM_HI_CMD_10])<<8) + Slave->Buffer[MB_REG_NUM_LO_CMD_10];  
  //последний считываемый регистр  
  tU16 LastRegAddr = RegAddr + RegNum -1;
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //переменная для определения ошибки обращения к несуществующему адресу
  tU8  Error=0;
  
    //выбор программы записи памяти
  switch (MemSpacePrefix)
  {
    case RAM_DATA_PREFIX:
      if(LastRegAddr<RAM_DATA_SIZE ) 
      {
        DataLength=ModbusRamWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,RegAddr,RegNum);   
      }
      else Error=1;//вдрес вне допустимой зоны       
    break;
    
    case CD_DATA_PREFIX:
      if(LastRegAddr<CD_DATA_SIZE ) {
          DataLength=ModbusCDWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum); 
      }
      else Error=1;//вдрес вне допустимой зоны         
    break; 
    
    default:   //не нашёл подходящей области 
      Error=1;         
    break;  
  }
  //если широковещательное
  if(MB_IS_BROADCAST_MSG) DataLength = 0;  
  //если нет - смотрим была ли ошибка обращения к несуществующему адресу
  else if(Error) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR); 
  
  return (DataLength);
}


//функция выбора функции чтения памяти RAM, FLASH или CD 
tU16 ModbusMemRead_VTEG(ModbusSlaveType* Slave)
{
  //старшая тетрада старшего байта адреса первого регистра данных (префикс)
  tU8 MemSpacePrefix = (Slave->Buffer[MB_START_ADDR_HI])>>PREFIX_SHIFT; 
  tU8 DataLength = 0; //длинна отправляемой посылки
  //начальный адрес регистра данных
  tU16 RegAddr = (((tU16)(Slave->Buffer[MB_START_ADDR_HI] & (~PREFIX_MASK))) << 8) + Slave->Buffer[MB_START_ADDR_LO];
  //количество запрашиваемых регистров  
  tU16 RegNum =  (((tU16)Slave->Buffer[MB_REG_NUM_HI_CMD_03]) << 8) + Slave->Buffer[MB_REG_NUM_LO_CMD_03];
  //последний считываемый регистр
  tU16 LastRegAddr = RegAddr + RegNum -1;
  //указатель на область памяти для чтения
  tU8* Source=0; 
  bool isFlashRead = false;
  
  //выбор программы чтения памяти  
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
    //не нашёл подходящей области      
    break;
  }
    //если указатель задан - читаем из памяти, если не задан - возвращаем ошибку
  if(Source==0) DataLength = ModbusError(Slave,MB_ERR_CODE_ILLEGAL_ADDR);
  else DataLength = ModbusDataRead(Source, Slave->Buffer, RegAddr, RegNum);
  
  if (isFlashRead) RAM_DATA.FLAGS0.bits.FLASH_WRITE = 0;//уставки прочитаны 
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
  tU8 DataLength = 0; //длинна отправляемой посылки
  u16 ResSize = getResourcesSize();
  u16 NumberOfItems = getResourcesNumberOfItems();
  DataLength = 4;  
  Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
  
  Slave->Buffer[MB_DATA_SECTION_CMD_11+0]= (ResSize >> 0) & 0x00FF;
  Slave->Buffer[MB_DATA_SECTION_CMD_11+1]= (ResSize >> 8) & 0x00FF;
  
  Slave->Buffer[MB_DATA_SECTION_CMD_11+2]= (NumberOfItems >> 0) & 0x00FF;
  Slave->Buffer[MB_DATA_SECTION_CMD_11+3]= (NumberOfItems >> 8) & 0x00FF;
  
  //DataLength += 2;
  DataLength += MB_DATA_SECTION_CMD_11;//прибавить длину заголовка   
  DataLength += CRC_SIZE;//прибавить длину crc 
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
  DataLength += MB_DATA_SECTION_CMD_11;//прибавить длину заголовка   
  DataLength += CRC_SIZE;//прибавить длину crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);

  return DataLength;
}

//gets the Resources Header and Table
/*
tU16 getResourcesHeaderAndTable(ModbusSlaveType* Slave) {
  tU8 i=0;  
  tU8 DataLength = 0; //длинна отправляемой посылки
  u8 * data = getRes();
  DataLength = 60;  
  Slave->Buffer[MB_DATA_BYTE_CNT_CMD_11]=DataLength;
  do
  Slave->Buffer[MB_DATA_SECTION_CMD_11+i]=data[i];
  while ((i++)!=DataLength);
  DataLength += MB_DATA_SECTION_CMD_11;//прибавить длину заголовка   
  DataLength += CRC_SIZE;//прибавить длину crc 
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);

  return DataLength;
}


*/





