#include "STM32F4xx_Intmash_USART.h"

//��������� �������� ��������
const tU32 USARTbaudRate[8]={
    4800,//0 
    9600,//1 
    19200,//2 
    57600,//3 
    115200,//4
    230400,//5  
    460800,//6  
    921600//7 
};

/*Name: UsartDriverInit
	������������� �������� (USART � DMA) ���������� �� ��������� Intmash_Usart *UserUsartStr
Arg:
	Intmash_Usart *UserUsartStr - ��������� ������������ ��������, �� ������� ����� �������� ��� �������������
Ret:
Comment:
����� ������� ���� ������� ���������� ������� � ���������������� ���������, 
��. �������� ���������
*/
void UsartDriverInit (Intmash_Usart *UserUsartStr)
{
  DMA_InitTypeDef DMA_InitStructure;
  USART_InitTypeDef USART_InitStructure;  
  //���� ������������� ���������� ������� ������, ���� ������� �������� ��������� 
  DMA_DeInit(UserUsartStr->DMAy_StreamRX);
  DMA_DeInit(UserUsartStr->DMAy_StreamRX); 
  USART_DeInit(UserUsartStr->USARTx);
  //������� ���������� ��� �� ������ � ������
  DMA_InitStructure.DMA_Channel = UserUsartStr->DMA_channel;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(UserUsartStr->USARTx->DR);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(UserUsartStr->DMAy_StreamRX, &DMA_InitStructure);//USART2_Rx
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_Init(UserUsartStr->DMAy_StreamTX, &DMA_InitStructure);//USART2_Tx  
  //����� ���������� ��� ���� � �������� ���  
  USART_InitStructure.USART_BaudRate = UserUsartStr->USART_BaudRate;  
  if (UserUsartStr->USART_StopBits < 4) USART_InitStructure.USART_StopBits = (tU16)UserUsartStr->USART_StopBits << 12;
  else USART_InitStructure.USART_StopBits = USART_StopBits_1;
  if (UserUsartStr->USART_Parity == 2) USART_InitStructure.USART_Parity = USART_Parity_Even;//����� 
  else if (UserUsartStr->USART_Parity == 1) USART_InitStructure.USART_Parity = USART_Parity_Odd;//�������
  else USART_InitStructure.USART_Parity = USART_Parity_No; //��� �������� ��������
  if (USART_InitStructure.USART_Parity == USART_Parity_No)USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  else USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UserUsartStr->USARTx, &USART_InitStructure);
  
  UserUsartStr->USARTx->CR1 |=  USART_CR1_RE;//��������� �������
  UserUsartStr->USARTx->CR1 |=  USART_CR1_TE;//��������� ���������� 
  UserUsartStr->USARTx->CR1 |=  USART_CR1_UE;//��������� UART2   
}



/*Name: UsartTransmit
	������� ��������� �������� �� �������� ������ 
Arg:
	Intmash_Usart *UserUsartStr - ��������� ������������ ��������
	tU8* Buffer - ��������� �� ����� ������������ ������ (������ �����)
	tU8 Cnt - ���������� ������������ ���� 
Ret:
Comment:
	
*/
void UsartTransmit(Intmash_Usart *UserUsartStr, tU8* Buffer, tU16 Cnt) 
{//��������� DMA �� �������� ������ � UART
  //SetDIRToTX;
  if (UserUsartStr->PolarityDIR == POLARITY_HI) GPIO_SetBits(UserUsartStr->GPIOxDIR, UserUsartStr->GPIO_PinDIR);
  else if (UserUsartStr->PolarityDIR == POLARITY_LOW) GPIO_ResetBits(UserUsartStr->GPIOxDIR, UserUsartStr->GPIO_PinDIR);
  
  UserUsartStr->DMAy_StreamTX->CR &= ~(uint32_t)DMA_SxCR_EN;//�������� DMA ��� ��������� ������� � ���������
  UserUsartStr->USARTx->SR  &=  ~USART_SR_TC;   //�������� ���� ��������� �������� 
  DMA_ClearFlag(UserUsartStr->DMAy_StreamTX, UserUsartStr->DMA_FLAGS_TX);//�������� ����� ������ ���, ��� ����� �� ��������  
 // DMA1->HIFCR = (uint32_t) (DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6);//�������� ����� ������ ���, ��� ����� �� ��������  
  UserUsartStr->DMAy_StreamTX->NDTR = Cnt;
  UserUsartStr->DMAy_StreamTX->M0AR = (uint32_t)Buffer;
  UserUsartStr->USARTx->CR3 |=  USART_CR3_DMAT;
  UserUsartStr->USARTx->CR1 |=  USART_CR1_TE;   //��������� ����������
  UserUsartStr->DMAy_StreamTX->CR |= (uint32_t)DMA_SxCR_EN;//������� DMA
  UserUsartStr->USARTx->CR1 |=  USART_CR1_TCIE; //������� ���������� �� ��������� ��������
  
} 

/*Name: UsartRecieve
	������� ��������� �������� �� ����� ������ 
Arg:
	Intmash_Usart *UserUsartStr - ��������� ������������ ��������
	tU8* Buffer - ��������� �� ����� ����������� ������ (���� ����������)
Ret:
Comment:
	
*/
void UsartRecieve (Intmash_Usart *UserUsartStr, tU8* Buffer) 
{//��������� DMA �� ������ ������ �� UART
 // SetDIRToRX;//����������� �� ����? ��������, ��� ���� ������ ���������
  if (UserUsartStr->PolarityDIR == POLARITY_HI) GPIO_ResetBits(UserUsartStr->GPIOxDIR, UserUsartStr->GPIO_PinDIR);
  else if (UserUsartStr->PolarityDIR == POLARITY_LOW) GPIO_SetBits(UserUsartStr->GPIOxDIR, UserUsartStr->GPIO_PinDIR);
  
  UserUsartStr->DMAy_StreamRX->CR &= ~(uint32_t)DMA_SxCR_EN;//�������� DMA ��� ��������� ������� � ���������
  DMA_ClearFlag(UserUsartStr->DMAy_StreamRX, UserUsartStr->DMA_FLAGS_RX);//�������� ����� ������ ���, ��� ����� �� ��������  
  //DMA1->HIFCR = (uint32_t) (DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5);
  UserUsartStr->USARTx->CR3 |=  USART_CR3_DMAR;
  UserUsartStr->DMAy_StreamRX->NDTR = URXBUFFSIZE;
  UserUsartStr->DMAy_StreamRX->M0AR = (uint32_t)Buffer;
  UserUsartStr->DMAy_StreamRX->CR |= (uint32_t)DMA_SxCR_EN;//������� DMA
  UserUsartStr->USARTx->CR1 |=  USART_CR1_IDLEIE;//��������� ���������� �� ����� ������
  UserUsartStr->USARTx->CR1 |=  USART_CR1_RE;//��������� �������
}

/*Name: UsartTxRxFinish
	������� ��������� ������/�������� ������ 
Arg:
	Intmash_Usart *UserUsartStr - ��������� ������������ ��������
Ret:
	0 - ����� ��������� �������� ������
	���������� �������� ���� - ����� ��������� ������ ������ (u8)
Comment:
���������� ����� ��������� ������ ��� �������� ������, 
� ���������� ������������� USART 
	������: 	
		void USART1_IRQHandler(void)
		{
		  UsartTxRxFinish(&USERusart);
		}
*/
tU16 UsartTxRxFinish(Intmash_Usart *UserUsartStr)
{
  static tU32 IIR;
  IIR = UserUsartStr->USARTx->SR;
    if ((IIR & USART_SR_TC) && (UserUsartStr->USARTx->CR1 & USART_CR1_TCIE)) // �������� �������� (��������� ���� ��������� ������� � ����)
      {   
        UserUsartStr->USARTx->SR  &=  ~USART_SR_TC;   //�������� ���� ��������� ��������
        UserUsartStr->USARTx->CR1 &=  ~USART_CR1_TCIE;//��������� ���������� �� ��������� ��������
        UserUsartStr->USARTx->CR3 &=  ~USART_CR3_DMAT;//��������� UART-�� ���������� �� DMA
        UserUsartStr->DMAy_StreamTX->CR &= ~(uint32_t)DMA_SxCR_EN;//��������� DMA �����������         

        return 0;
      }
    if ((IIR & USART_SR_IDLE) & (UserUsartStr->USARTx->CR1 & USART_CR1_IDLEIE)) // ����� ������� ��� ����� ���������� ����� � 1 IDLE ����
      {
        UserUsartStr->USARTx->DR; //����� ����� IDLE
        UserUsartStr->USARTx->CR1 &=  ~USART_CR1_RE;    //��������� �������
        UserUsartStr->USARTx->CR1 &=  ~USART_CR1_IDLEIE;//��������� ���������� �� ����� ������
        UserUsartStr->USARTx->CR3 &=  ~USART_CR3_DMAR;  //��������� DMA RX
        UserUsartStr->DMAy_StreamRX->CR &= ~(uint32_t)DMA_SxCR_EN;//��������� DMA �� ����       
        return (URXBUFFSIZE - (tU16)UserUsartStr->DMAy_StreamRX->NDTR);//���-�� �������� ����
      }
    return 0;
}
