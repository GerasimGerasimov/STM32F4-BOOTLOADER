#include "init.h"
#include "stm32f4xx.h"

#include "ramdata.h"
#include "ModbusSlaveConf.h"
#include "STM32F4xx_Intmash_Flash.h"

ErrorStatus HSEStartUpStatus;


void GPIO_INIT_Configuration (void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//притянут кверху
  //порт A                       USB_P
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//--------------------------------------------------------------

void GPIO_Configuration(void){//настройка портов ввода-вывода
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                          RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//
 
//------------------------------------------------------------------------------------------------------------     
  //инициализируем выходы
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//теперь выходы пушпульные
    
  //порт А                        LED5         DIR1
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_8;            
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //порт B                        LED_LNK       LED_RUN       LED_ALRM      LED_WRN        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

//------------------------------------------------------------------------------------------------------------   
  //Инициализируем ноги, связанные с переферией:
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //Rx
  //настраиваем AF пины, которые push-pull, либо входы: 
  //порт А                         USART1_Tx    USART1_Rx
  GPIO_InitStructure.GPIO_Pin  =   GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//******************************************************************************
//Таймер общего назначения с прерыванием раз в 0,001 сек
void TIM6_Configuration(void){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
   /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//частота = TIM4CLK(84МГц)/10 = 8,4МГц.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 8400;//частота переполнения 8,4Мгц/8400 = 1кГц
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM6, DISABLE);
  TIM6->SR = 0;
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
//разрешаем переходы по векторам и настраиваем приоритеты следующих прерываний:
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* Enable TIM interrupts */
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;//таймер общего назначения(DIO)
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//  Modbus slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
}

void Init (void){//начальная инициализация. 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  GPIO_Configuration();//конфигурируем порт
  ModbusClientInit();//инициализируем работу интерфейсов модбас-клиента  
  TIM6_Configuration();//перенёс для искл ложного включения реле  
  NVIC_Configuration();//конфигурируем прерывания
}



