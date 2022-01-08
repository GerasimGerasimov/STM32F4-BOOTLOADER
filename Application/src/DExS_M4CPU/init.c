#include "init.h"
#include "stm32f4xx.h"

#include "ramdata.h"
#include "ModbusSlaveConf.h"
#include "STM32F4xx_Intmash_Flash.h"

#define IWDG_1ms_TICKS 8
#define IWDG_KEY_RELOAD 0xAAAA
#define IWDG_KEY_START  0xCCCC

ErrorStatus HSEStartUpStatus;

//--------------------------------------------------------------

void GPIO_Configuration(void){//��������� ������ �����-������
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |\
                          RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�� ��������� �������� 100���
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//�� ��������� ��� ��������
  
//------------------------------------------------------------------------------------------------------------   
  //�������������� �������� ������, �� ����������� � ��������� 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//������ ������ ����������
  //���� �                        DIR1            
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //���� B                        LED2        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_12;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //���� E                        DIR2        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_6;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  //���� F                        LED1        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_11;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  //���� H                        LED3, LED4        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_Init(GPIOH, &GPIO_InitStructure);  

//------------------------------------------------------------------------------------------------------------   
  //�������������� ����, ��������� � ����������:
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  //���������� �������������� ������� � ������ �����:
  //GPIOA->AFR[0]  |= 0x00007700;//USART2
  //���� ������ ������������� ����� ����������, ���� ����� ��������:
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //Rx
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //Tx 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //Rx
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //Tx 
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //Rx
  
  //����������� AF ����, ������� push-pull, ���� �����: 
  //���� �                        USART2_Tx    USART2_Rx    USART1_Tx    USART1_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //���� B                        USART3_Tx     USART3_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //���� C                        USART6_Tx     USART6_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

//******************************************************************************
//������ ��� ������ � MODBUS ���������
void LogTimerConfiguration(void)
{
    //��������� �������, ������� ����� �������������� ��� ������ � OSC � ������������ ��������
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //������������ 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
   /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//������� = TIM4CLK(168���)/10000 = 16,8k��.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 1680;//TimPeriod;//������� ������������ 16.8���/1680 = 10��
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM1, DISABLE);
  TIM1->SR = 0;
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
    
}

//******************************************************************************
//������ ������ ���������� � ����������� ��� � 0,001 ���
void TIM6_Configuration(void){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
   /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//������� = TIM4CLK(84���)/10 = 8,4���.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 8400;//������� ������������ 8,4���/8400 = 1���
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM6, DISABLE);
  
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);
  TIM6->SR = 0;  
}

/*******************************************************************************
* Function Name  : Watchdog_configuration
* Description    : ��������� ����������� �������
* Input          : u16 reset_time_ms - ����� �� ������������ � �� (�� 1 �� 511)
* Output         : None
* Return         : None
********************************************************************************/
void Watchdog_configuration(u16 reset_time_ms){
  RCC_LSICmd(ENABLE);
  while (!(RCC->CSR & RCC_CSR_LSIRDY)){};
  IWDG->KR = IWDG_WriteAccess_Enable;
  IWDG->RLR = reset_time_ms * IWDG_1ms_TICKS;
  IWDG->KR = IWDG_KEY_RELOAD;
  IWDG->KR = IWDG_KEY_START;
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
//��������� �������� �� �������� � ����������� ���������� ��������� ����������:
#define APP_START_ADDR 0x08008000

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  SCB->VTOR = APP_START_ADDR;//��������� ������ ������� ���������� �� ���������� ������
//#ifdef  VECT_TAB_RAM  
//  /* Set the Vector Table base location at 0x20000000 */
//  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
//#else  /* VECT_TAB_FLASH  */
//  /* Set the Vector Table base location at 0x08000000 */ 
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
//#endif
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* Enable TIM interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;//������ MOBBUS Slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
  NVIC_Init(&NVIC_InitStructure);
     
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;//������ ������ ����������(DIO)
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);  
      
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;// Modbus slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;// Modbus slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;// Modbus slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
}

void Start_init (void){//��������� �������������. 
  //��������� ������ ���(��� ������)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  GPIO_Configuration();//������������� ����
  
  ModbusClientInit();//�������������� ������ ����������� ������-�������  
  NVIC_Configuration();//������������� ����������
  //Watchdog_configuration(3);//������������� ������� �� 3 ��
  
  //��� �� ��������� ������ ���������� �������, ��� �������
  //����� ��� ������������ 0xffff ��� �������� ������� (16 ���)
  SysTick->LOAD  = 0xffff;      /* set reload register */  
  SysTick->VAL   = 0;           /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;   
}

//������������� ���������
void Init (void){ 
  Start_init();//��������� �������������, ����������� ��� ��������� ����������
  TIM6_Configuration();//������ ��� ���� ������� ��������� ����
}




