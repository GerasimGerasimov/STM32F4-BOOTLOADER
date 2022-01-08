#ifndef STM32F4xx_Intmash_USART_H
#define STM32F4xx_Intmash_USART_H

#include "stm32f4xx.h"
#include "Intmash_bastypes.h" 


#define BOOTLOADER_BUFFER 16384
#define URXBUFFSIZE  BOOTLOADER_BUFFER+8 //размер буфера приёмника
//значения для состояния DIR
#define WITHOUT_DIR  0 // ноги дир нет, ничего не делаем
#define POLARITY_LOW 1 // нога используется, инверсный сигнал.
#define POLARITY_HI  2 // нога используется, прямой сигнал. 
/*
Структура конфигурации драйвера
для каждого драйвера - своя структура и своя инициализация

//пример структуры и ее инициализации
Intmash_Usart USERusart; // создали структуру
void InitUserUsartData(void) //фукция инициализации структуры
{
  USERusart.USARTx = USART1;
  USERusart.USART_BaudRate = 115200;
  USERusart.DMAy_StreamRX = DMA2_Stream2;
  USERusart.DMAy_StreamTX = DMA2_Stream7;
  USERusart.DMA_channel = DMA_Channel_4;
  USERusart.DMA_FLAGS_RX = DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2 | DMA_FLAG_TCIF2;
  USERusart.DMA_FLAGS_TX = DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7;
  USERusart.GPIOxDIR = GPIOA;
  USERusart.GPIO_PinDIR = GPIO_Pin_8;
  USERusart.PolarityDIR = 2; //неинверсная полярность
}
*/
typedef struct 
{
  USART_TypeDef* USARTx; //указатель на используемый USART
  DMA_Stream_TypeDef* DMAy_StreamTX; //указатель на поток DMA на передачу данных этого USART
  DMA_Stream_TypeDef* DMAy_StreamRX; //указатель на поток DMA на прием данных этого USART
  tU32 DMA_channel; //канал ДМА 
  tU32 DMA_FLAGS_TX; //флаги прерывания, отвечающие за передачу данных
  tU32 DMA_FLAGS_RX; //флаги прерывания, отвечающие за прием данных
  tU32 USART_BaudRate; //скорость передачи, например 9600
  GPIO_TypeDef* GPIOxDIR; //порт, пин которого используется для переключения напрявления передачи данных
  tU16 GPIO_PinDIR; //номер пина, который используется для переключения напрявления передачи данных
  tU16 PolarityDIR; //2 - неинверсный, 1 - инверсный, 0 - не используется
  tU8 USART_StopBits; //количество стоповых бит 0-1/1-0,5/2-2/3-1,5 
  tU8 USART_Parity; // 0 - 0/ 2 - Even / 1- Odd
} Intmash_Usart;  



void UsartDriverInit (Intmash_Usart *UserUsartStr);
void UsartTransmit(Intmash_Usart *UserUsartStr, tU8* Buffer, tU16 Cnt);
void UsartRecieve (Intmash_Usart *UserUsartStr, tU8* Buffer);
tU16 UsartTxRxFinish(Intmash_Usart *UserUsartStr);

extern const tU32 USARTbaudRate[8];

#endif /*STM32F4xx_Intmash_USART_H */