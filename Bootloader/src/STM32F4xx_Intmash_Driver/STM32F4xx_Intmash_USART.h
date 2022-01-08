#ifndef STM32F4xx_Intmash_USART_H
#define STM32F4xx_Intmash_USART_H

#include "stm32f4xx.h"
#include "Intmash_bastypes.h" 


#define BOOTLOADER_BUFFER 16384
#define URXBUFFSIZE  BOOTLOADER_BUFFER+8 //������ ������ ��������
//�������� ��� ��������� DIR
#define WITHOUT_DIR  0 // ���� ��� ���, ������ �� ������
#define POLARITY_LOW 1 // ���� ������������, ��������� ������.
#define POLARITY_HI  2 // ���� ������������, ������ ������. 
/*
��������� ������������ ��������
��� ������� �������� - ���� ��������� � ���� �������������

//������ ��������� � �� �������������
Intmash_Usart USERusart; // ������� ���������
void InitUserUsartData(void) //������ ������������� ���������
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
  USERusart.PolarityDIR = 2; //����������� ����������
}
*/
typedef struct 
{
  USART_TypeDef* USARTx; //��������� �� ������������ USART
  DMA_Stream_TypeDef* DMAy_StreamTX; //��������� �� ����� DMA �� �������� ������ ����� USART
  DMA_Stream_TypeDef* DMAy_StreamRX; //��������� �� ����� DMA �� ����� ������ ����� USART
  tU32 DMA_channel; //����� ��� 
  tU32 DMA_FLAGS_TX; //����� ����������, ���������� �� �������� ������
  tU32 DMA_FLAGS_RX; //����� ����������, ���������� �� ����� ������
  tU32 USART_BaudRate; //�������� ��������, �������� 9600
  GPIO_TypeDef* GPIOxDIR; //����, ��� �������� ������������ ��� ������������ ����������� �������� ������
  tU16 GPIO_PinDIR; //����� ����, ������� ������������ ��� ������������ ����������� �������� ������
  tU16 PolarityDIR; //2 - �����������, 1 - ���������, 0 - �� ������������
  tU8 USART_StopBits; //���������� �������� ��� 0-1/1-0,5/2-2/3-1,5 
  tU8 USART_Parity; // 0 - 0/ 2 - Even / 1- Odd
} Intmash_Usart;  



void UsartDriverInit (Intmash_Usart *UserUsartStr);
void UsartTransmit(Intmash_Usart *UserUsartStr, tU8* Buffer, tU16 Cnt);
void UsartRecieve (Intmash_Usart *UserUsartStr, tU8* Buffer);
tU16 UsartTxRxFinish(Intmash_Usart *UserUsartStr);

extern const tU32 USARTbaudRate[8];

#endif /*STM32F4xx_Intmash_USART_H */