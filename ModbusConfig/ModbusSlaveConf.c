#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_USART.h"
#include "ModbusSlave.h"
#include "Intmash_bastypes.h" 
#include "DExS.M4CPU_BoardLEDs.h"
#include "FlashData.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "STM32F4xx_Intmash_RTC.h"
#include "RAMData.h"
#include "crc16.h"
#include "CalibrationData.h"

//структуры драйверов UART и слейвов
Intmash_Usart UARTtoUSB; // связь по USB
ModbusSlaveType USBslave; 

Intmash_Usart UARTtoRS485; // связь по RS485
ModbusSlaveType RS485slave;

Intmash_Usart UARTtoOptRS485; // связь с опциональным RS485
ModbusSlaveType OptRS485slave;

tU8 ModbusMemWrite_VTEG(ModbusSlaveType* Slave);
tU8 ModbusMemRead_VTEG(ModbusSlaveType* Slave);
tU8 GetDateTime(ModbusSlaveType* Slave);
tU8 SetDateTime(ModbusSlaveType* Slave);
tU8 WriteMemNotAnswer(ModbusSlaveType* Slave);//команда FA от системы резервирования (запись данных без ответа, но не широковещательная)

tU8 GetConfig_E100_RS485(ModbusSlaveType* Slave){
  
  /*
  длина пакета:
  1 - адрес
  2 - команда
  3 - модбас_адр
  4 - модбас_скорость
  5 - IP_0
  6 - IP_1
  7 - IP_2
  8 - IP_3
  9 - ram size
  10 - flash size
  9,10 - CRC
  */
  //длинна отправляемой посылки
  tU8 DataLength = 0; 
  //если сообщение широковещательное - не отвечаем
  if(!MB_IS_BROADCAST_MSG)
  {  
    DataLength = 10;
    // Addr
    Slave->Buffer[2] = (tU8) (FLASH_DATA_RC.fCOM1DABP & 0xFF);    
    //BaudRate
    Slave->Buffer[3] = (tU8) (FLASH_DATA_RC.fCOM1DABP >> 8) & 0xFF ;
    
    //u16 IpAddr = FLASH_DATA_RC.Ip_addr_LO;
    //IP addr
    Slave->Buffer[4] = (tU8) (FLASH_DATA_RC.Ip_addr_LO & 0xFF);
    //IpAddr = IpAddr >> 8;
    Slave->Buffer[5] = (tU8) (FLASH_DATA_RC.Ip_addr_LO >> 8 & 0xFF);
    //IpAddr = IpAddr >> 8;
    Slave->Buffer[6] = (tU8) (FLASH_DATA_RC.Ip_addr_HI & 0xFF);
    //IpAddr = IpAddr >> 8;
    Slave->Buffer[7] = (tU8) (FLASH_DATA_RC.Ip_addr_HI >> 8 & 0xFF);
    
    //размеры RAM + FLASH 
    Slave->Buffer[8] = (tU8) (RAM_DATA_SIZE);
    Slave->Buffer[9] = (tU8) (FLASH_DATA_SIZE);
    DataLength += CRC_SIZE; 
    //формируем конец пакета
    FrameEndCrc16((tU8*)Slave->Buffer, DataLength);       
  }  
  return DataLength;  
}

//число используемых команд +1
ModbusCommandHandlerType ModbusCommands[8]={
  {ModbusMemRead_VTEG, 0x03},
  {ModbusMemWrite_VTEG, 0x10},
  {GetDeviceID, 0x11},
  {GetDateTime, 0x29},//чтение/установка времени и даты
  {SetDateTime, 0x2a},
  {GetConfig_E100_RS485, 0xEC}, 
  {WriteMemNotAnswer, 0xFA},
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
    UARTtoUSB.USART_BaudRate = USARTbaudRate[CD_DATA_RC.Modbus_USB.B[1]];;
    USBslave.SlaveAddress = CD_DATA_RC.Modbus_USB.B[0];;
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
  UARTtoRS485.USART_BaudRate = USARTbaudRate[CD_DATA_RC.Modbus_RS485.B[1]];
  RS485slave.SlaveAddress = CD_DATA_RC.Modbus_RS485.B[0];
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
  UARTtoOptRS485.USART_BaudRate = USARTbaudRate[CD_DATA_RC.Modbus_option.B[1]];
  OptRS485slave.SlaveAddress = CD_DATA_RC.Modbus_option.B[0];
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
  tU8 MsgSize = UsartTxRxFinish(&UARTtoUSB);//определили что произошло: прием или принято MsgSize байт. 
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
  
  tU8 MsgSize = UsartTxRxFinish(&UARTtoRS485);//определили что произошло: прием или принято MsgSize байт.
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
  
  tU8 MsgSize = UsartTxRxFinish(&UARTtoOptRS485);//определили что произошло: прием или принято MsgSize байт.
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
  tU8 MsgSize = 0;
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

//функция чтения/установки времени
/*
Команда чтения времени
запрос:
[0] - addr
[1] - cmd = 0x29
[2],[3] - crc
ответ:
[0] - addr
[1] - cmd = 0x29
[2] - Year
[3] - Month
[4] - Day
[5] - Hours
[6] - Minutes
[7] - Seconds
[8],[9] - crc
*/
tU8 GetDateTime(ModbusSlaveType* Slave)
{
  tU8 DataLength = 0; //длинна отправляемой посылки
  TDateTimeIntRTC * ActualTimeDate;//элемент структуры для получения времени в MDLL
  ActualTimeDate=(TDateTimeIntRTC *)RAM_DATA.TimeBuf;
  Slave->Buffer[2] = ActualTimeDate->Year;
  Slave->Buffer[3] = ActualTimeDate->Month;
  Slave->Buffer[4] = ActualTimeDate->Day;
  Slave->Buffer[5] = ActualTimeDate->Hours;
  Slave->Buffer[6] = ActualTimeDate->Minutes;
  Slave->Buffer[7] = ActualTimeDate->Seconds; 
  //в любом случае, команда отдаёт текущее время из структуры 
  DataLength = 10; //размер ответа
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength; //возвращает размер отправляемой посылки
}

/*Команда записи времени
запрос/ответ:
[0] - addr
[1] - cmd = 0x2a
[2] - Year
[3] - Month
[4] - Day
[5] - Hours
[6] - Minutes
[7] - Seconds
[8],[9] - crc
*/
tU8 SetDateTime(ModbusSlaveType* Slave){ //u32 DATA_BASE
  tU8 DataLength = 0; //длинна отправляемой посылки
  TDateTimeIntRTC * ActualTimeDate;//элемент структуры для получения времени в MDLL
  ActualTimeDate=(TDateTimeIntRTC *)RAM_DATA.TimeBuf;  
  //Установка времени
  ActualTimeDate->Options = 222;
  ActualTimeDate->Year    = Slave->Buffer[2];
  ActualTimeDate->Month   = Slave->Buffer[3];
  ActualTimeDate->Day     = Slave->Buffer[4];
  ActualTimeDate->DayOfWeek = 1;
  ActualTimeDate->Hours   = Slave->Buffer[5];
  ActualTimeDate->Minutes = Slave->Buffer[6];
  ActualTimeDate->Seconds = Slave->Buffer[7];    
  IntmashRTC_SetDate(ActualTimeDate);//задать новое время
  IntmashRTC_SetTime(ActualTimeDate);//и дату
  ActualTimeDate->Options = 0;   
  //в любом случае, команда отдаёт текущее время из структуры 
  DataLength = 10; //размер ответа
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength; //возвращает размер отправляемой посылки
}

//таймер отсутствия связи с Арбитром (авто переход в состояние "ВЕДУЩИЙ")
//при при отсутствии связи с Арбитром
void RCMD_Rst_Timer_run(void){
  if (RAM_DATA.RCMD_Rst_Timer == 0) {//счётчик стал НОЛЬ
    RAM_DATA.RegCMD = 0x0000;//стал ведущим по окончании связи с Арбитром
	RAM_DATA.FLAGS.BA.ext_master_lnk = 0;//нет связи с арбитром
  }
  else
    RAM_DATA.RCMD_Rst_Timer--;//а если связи нет, то отнимаю по едтиничке до нуля
}

//команда FA от системы резервирования (запись данных без ответа, но не широковещательная)
tU8 WriteMemNotAnswer (ModbusSlaveType* Slave){
  Slave->Buffer[MB_DEVICE_ADDRESS] = BROADCAST_MSG_ADDR;//меняю адрес на широковещательный
                                                        //чтобы на выходе из функции записи не генерился ответ
  RAM_DATA.RCMD_Rst_Timer = 1000;//1 сек.время таймаута с Арбитром
  RAM_DATA.FLAGS.BA.ext_master_lnk = 1;//есть связь с арбитром
  return ModbusMemWrite_VTEG(Slave);//и вызываю функцию записи
}


//функция записи данных в RAM память
tU8 ModbusFlashRCWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum){ //u32 DATA_BASE
  tU8 DataLength = 0; //длинна отправляемой посылки
  
  tU16 *ModbusAddrSet = (tU16*)((tU32)&FLASH_DATA_RC + ((tU32)RegAddr << 1));
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


tU8 ModbusCDRCWrite(tU8* Buffer,tU8 BufDataIdx, tU8 RegAddr, tU8 RegNum){ //u32 DATA_BASE
  tU8 DataLength = 0; //длинна отправляемой посылки
  
  tU16 *ModbusAddrSet = (tU16*)((tU32)&CD_DATA_RC + ((tU32)RegAddr << 1));
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
tU8 ModbusMemWrite_VTEG(ModbusSlaveType* Slave){
  
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
    
    case FLASH_DATA_PREFIX:
      if(LastRegAddr<FLASH_DATA_SIZE ) 
      {
        if (FlashDataLoaded())//если уставки скопированы в ОЗУ
         DataLength=ModbusFlashRCWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum); 
        else
        {
          DataLength=ModbusFlashWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum);
        }
		RAM_DATA.FLAGS0.bits.FLASH_WRITE = 1;//флэш был модифицирован
      }
      else Error=1;//вдрес вне допустимой зоны          
    break;
    
    case CD_DATA_PREFIX:
      if(LastRegAddr<CD_DATA_SIZE ) 
      {
        if (CDataLoaded())//если уставки выгружены в ОЗУ
          DataLength=ModbusCDRCWrite(Slave->Buffer,MB_DATA_SECTION_CMD_10,(tU8)RegAddr,(tU8)RegNum);
        else
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
tU8 ModbusMemRead_VTEG(ModbusSlaveType* Slave)
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
    
    case FLASH_DATA_PREFIX:
      if(LastRegAddr < FLASH_DATA_SIZE)
      {
        if (FlashDataLoaded()) Source=(tU8*)&FLASH_DATA_RC;//если уставки скопированы в ОЗУ         
        else Source=(tU8*)&FLASH_DATA;    
        isFlashRead = true;
      }
    break;
    case CD_DATA_PREFIX:
      if(LastRegAddr < CD_DATA_SIZE) 
      {
       if (CDataLoaded()) Source=(tU8*)&CD_DATA_RC;
       else Source=(tU8*)&CD_DATA;
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













