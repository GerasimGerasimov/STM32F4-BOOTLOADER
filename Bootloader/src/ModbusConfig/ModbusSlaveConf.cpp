#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_USART.h"
#include "ModbusSlave.h"
#include "Intmash_bastypes.h" 
#include "BoardLEDs.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "RAMData.h"
#include "crc16.h"
#include "bootloader.h"
#include "USARTStartInit.h"

//структуры драйверов UART и слейвов
ModbusSlaveType USBslave;
ModbusSlaveType RS485slave_1;

//число используемых команд +1
ModbusCommandHandlerType ModbusCommands[5]={
  {ModbusMemRead, 0x03},
  {ModbusMemWrite, 0x10},
  {GetDeviceID, 0x11},
  {BootLoader, 0xB0},
  {0, 0},
};

void USART1SlaveInit(){
  initUsart1();
  USBslave.SlaveAddress = 1;
  // USBslave.ModbusMap = ModbusMap;
  UsartRecieve (&UARTtoUSB, &USBslave.Buffer[0]);  
}

void USART3SlaveInit(){
  initUsart3();
  RS485slave_1.SlaveAddress = 1;
  // RS485slave_1.ModbusMap = ModbusMap;
  UsartRecieve (&UARTtoRS485_1, &RS485slave_1.Buffer[0]);  
}

extern "C" void ModbusClientInit(){
//фукция инициализации структуры
  USART1SlaveInit();
  USART3SlaveInit();
}

/***обработка прерывания по интерфейсу*****************************************/
void ModbusSlaveTransaction(Intmash_Usart* UARTstr, ModbusSlaveType* SlaveStr, GPIO_TypeDef* LED_Port, u16 LED_Pin){
  //определили что произошло: прием или принято MsgSize байт.
  u16 MsgSize = UsartTxRxFinish(UARTstr);
  if(MsgSize)
  {
    if(LED_Port->ODR & LED_Pin){
      //LED_ON
      LED_Port->BSRRH = LED_Pin;
    }
    else{
      //LED_OFF
      LED_Port->BSRRL = LED_Pin;
    }

    //если принятно  
    //вызываем функцию декодирования, определяем размер пакета для отправки
    MsgSize = ModbusCommandDecode(SlaveStr, MsgSize, ModbusCommands); 
    //если есть что отправлять - отправляем, если нет - заряжаем на прием.
    if(MsgSize) UsartTransmit(UARTstr, SlaveStr->Buffer, MsgSize);
    else UsartRecieve(UARTstr, SlaveStr->Buffer);
  }  
  else 
  { //если отправлено, заряжаем на прием.
    UsartRecieve (UARTstr, SlaveStr->Buffer);
  }
}

extern "C" void USART1_IRQHandler(){
  //прерывание USART от USB
  ModbusSlaveTransaction(&UARTtoUSB, &USBslave, GPIOC, GPIO_Pin_3);
}

extern "C" void USART3_IRQHandler(){
  //прерывание USART от RS485-1
  ModbusSlaveTransaction(&UARTtoRS485_1, &RS485slave_1, GPIOC, GPIO_Pin_2);
}
