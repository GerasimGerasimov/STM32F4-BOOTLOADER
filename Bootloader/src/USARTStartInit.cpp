#include "USARTStartInit.h"

//структуры драйверов UART и слейвов
Intmash_Usart UARTtoUSB; // связь по USB
Intmash_Usart UARTtoRS485_1; // связь по RS485_1

#define UART_BaudRate 115200

void initGPIO_USART(GPIO_InitTypeDef* GPIO_InitStructureUsart){
  GPIO_InitStructureUsart->GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructureUsart->GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructureUsart->GPIO_OType = GPIO_OType_PP;
}

void initGPIO_USART1(){
  GPIO_InitTypeDef GPIO_InitStructureUsart;
  initGPIO_USART(&GPIO_InitStructureUsart);
    //порт A                             USART1_Tx    USART1_Rx
  GPIO_InitStructureUsart.GPIO_Pin  =  GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructureUsart);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //Rx  
}

void initGPIO_USART3(){
  GPIO_InitTypeDef GPIO_InitStructureUsart;
  initGPIO_USART(&GPIO_InitStructureUsart);
    //порт B                            USART3_Tx    USART3_Rx
  GPIO_InitStructureUsart.GPIO_Pin  =  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructureUsart);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //Tx 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //Rx
}

void init_NVIC_USART1(){
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;// Modbus slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
}

void init_NVIC_USART3(){
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;// Modbus slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
}

void initUsart1(){
  initGPIO_USART1();
  init_NVIC_USART1();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  //USART1

  UARTtoUSB.USARTx = USART1;
  UARTtoUSB.USART_BaudRate = UART_BaudRate;
  UARTtoUSB.USART_StopBits = 0;
  UARTtoUSB.USART_Parity = 0;
  UARTtoUSB.DMAy_StreamRX = DMA2_Stream5;
  UARTtoUSB.DMAy_StreamTX = DMA2_Stream7;
  UARTtoUSB.DMA_channel = DMA_Channel_4;
  UARTtoUSB.DMA_FLAGS_RX = DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5;
  UARTtoUSB.DMA_FLAGS_TX = DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7;
  UARTtoUSB.PolarityDIR = WITHOUT_DIR; 
  UsartDriverInit (&UARTtoUSB); 
}

void initUsart3(){
  initGPIO_USART3();
  init_NVIC_USART3();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  UARTtoRS485_1.USARTx = USART3;
  UARTtoRS485_1.USART_BaudRate = UART_BaudRate;
  UARTtoRS485_1.USART_StopBits = 0;
  UARTtoRS485_1.USART_Parity = 0;
  UARTtoRS485_1.DMAy_StreamRX = DMA1_Stream1;
  UARTtoRS485_1.DMAy_StreamTX = DMA1_Stream3;
  UARTtoRS485_1.DMA_channel = DMA_Channel_4;
  UARTtoRS485_1.DMA_FLAGS_RX = DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1;
  UARTtoRS485_1.DMA_FLAGS_TX = DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3;
  UARTtoRS485_1.GPIOxDIR = GPIOB;
  UARTtoRS485_1.GPIO_PinDIR = GPIO_Pin_12;
  UARTtoRS485_1.PolarityDIR = POLARITY_HI; 
  UsartDriverInit (&UARTtoRS485_1);
}
