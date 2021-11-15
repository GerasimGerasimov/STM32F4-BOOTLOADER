#include "init.h"
#include "stm32f4xx.h"

#include "ramdata.h"
#include "flashdata.h"
#include "ModbusSlaveConf.h"
#include "STM32F4xx_Intmash_Flash.h"

#define IWDG_1ms_TICKS 8
#define IWDG_KEY_RELOAD 0xAAAA
#define IWDG_KEY_START  0xCCCC

ErrorStatus HSEStartUpStatus;

//--------------------------------------------------------------

void GPIO_Configuration(void){//настройка портов ввода-вывода
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |\
                          RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//по умолчанию скорость 100МГц
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//по умолчанию без притяжки
  
//------------------------------------------------------------------------------------------------------------   
  //инициализируем аналоговые входы/выходы
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; 
  //порт А                        Upwr_ca      Ipwr         Ipwr         Upwr_ab      ref          Usg_bc
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //порт B                        Upwr_bc      Upwr_ca        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //порт C                        Iload_1.2    In_1         In2          In3          Usg_ab       Usg_ca
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //порт F                        Istat        Istat_max    Ustat        Uload        Iload_1.1
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
//------------------------------------------------------------------------------------------------------------  
  
  //инициализируем цифровые входы, не привязанные к периферии 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  //порт А                        TR2.1         TR2.2 
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //порт E                        CFI1         CFI2         CFU2         CFU1 
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  //порт G                        TR1.1        TR1.2         TR1.3         TR1.4         TR1.5         TR1.6
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  //порт H                        SYNC1        SYNC2        SYNC3        TR2.3         TR2.4         TR2.5 
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  //порт I                        TR2.6        FSN           FSP 
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_Init(GPIOI, &GPIO_InitStructure);
//------------------------------------------------------------------------------------------------------------   
  //инициализируем цифровые выходы, не привязанные к периферии 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//сначала выходы с открытым коллектором  
  
  //прединициализация GPIO для искл ложного включения реле
  GPIOB->BSRRL = GPIO_Pin_13;//DO_OE_UP
    GPIOB->BSRRL = GPIO_Pin_15;//DO_RST_UP
      GPIOB->BSRRL = GPIO_Pin_14;//DO_LCLK_UP
  //порт B                        OUT_OE        OUT_ST        OUT_MR
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //порт G                        IN_CP        IN_PL        IN_CE        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  //порт H                        FR_CLK       FR_DATA      FR_CS           
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  
  
  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//теперь выходы пушпульные
  //порт А                        DIR1         SELECTOR_2   
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_8 | GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //порт B                        S2.2         S2.1         S1.1         S1.2         LED2        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //порт E                        DIR2        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_6;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  //порт F                        LED1        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_11;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  //порт H                        S2.6          S2.5          S2.4      
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  //порт I                        S2.3         SELECTOR_1   S1.6         S1.3         S1.4         S1.5      
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOI, &GPIO_InitStructure);

//------------------------------------------------------------------------------------------------------------   
  //Инициализируем ноги, связанные с переферией:
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  //подключаем альтернативные функции к нужным пинам:
  //GPIOA->AFR[0]  |= 0x00007700;//USART2
  //этот способ инициализации более громоздкий, зато более понятный:
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4); //ШИМ 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //Rx
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //Tx 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //Rx
  
  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); //Tx 
  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); //Rx
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //Tx 
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //Rx
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1); //SCK - OUT_SH(DIO_CLK)
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1); //MISO - IN_D  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1); //MOSI - OUT_D 
  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//FSMC - D2
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);//FSMC - D3
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);//FSMC - OE
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);//FSMC - WE
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);//FSMC - NE1
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);//FSMC - D13
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);//FSMC - D14
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);//FSMC - D15
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);//FSMC - A16
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);//FSMC - A17
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);//FSMC - A18
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);//FSMC - D0
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);//FSMC - D1
  
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_FSMC);//FSMC - LB
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_FSMC);//FSMC - UB
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);//FSMC - D4
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);//FSMC - D5
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);//FSMC - D6
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);//FSMC - D7
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);//FSMC - D8
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);//FSMC - D9
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);//FSMC - D10
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);//FSMC - D11
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);//FSMC - D12
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);//FSMC - A0
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FSMC);//FSMC - A1
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FSMC);//FSMC - A2
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);//FSMC - A3
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FSMC);//FSMC - A4
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FSMC);//FSMC - A5
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);//FSMC - A6
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FSMC);//FSMC - A7
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FSMC);//FSMC - A8
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC);//FSMC - A9
  
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC);//FSMC - A10
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC);//FSMC - A11
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FSMC);//FSMC - A12
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource3, GPIO_AF_FSMC);//FSMC - A13
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_FSMC);//FSMC - A14
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_FSMC);//FSMC - A15
  
  //настраиваем AF пины, которые push-pull, либо входы: 
  //порт А                        USART2_Tx    USART2_Rx    USART1_Tx    USART1_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //порт B                        ШИМ на БИТ   USART3_Tx     USART3_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //порт C                        USART6_Tx     USART6_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //порт D                        FSMC_pins
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  //порт E                        FSMC_pins
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  //порт F                        FSMC_pins
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  //порт G                        FSMC_pins
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  //настраиваем AF пины, которые open drain, либо входы:
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  //порт B                        OUT_SH(DIO_CLK)   IN_D         OUT_D       
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3    |   GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //продолжить на остальные порты!!!  
}

//******************************************************************************
//Таймер для работы с MODBUS клиентами
void LogTimerConfiguration(void)
{
    //настройка таймера, который будет использоваться для записи в OSC с определенной частотой
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //тактирование 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
   /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//частота = TIM4CLK(168МГц)/10000 = 16,8kГц.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 1680;//TimPeriod;//частота переполнения 16.8кГц/1680 = 10Гц
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM1, DISABLE);
  TIM1->SR = 0;
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
    
}

//******************************************************************************
//настройка таймеров для СИФУ
//все таймера настраиваются одинаково, частота выбрана исходя их моделирования работы СИФУ,
//время одного тика 3,92857Е-7. У каждого таймера включены 2 канала сравнения для выставления и сброса импульса.
// после инициализации таймеры НЕ включаются. включаются только при появлении синхронизации
// сразу разрешаем прерывания по переполнению, срабатыванию каналов сравнения 1 и 2
void TIM2_3_4_5_9_12_Configuration (void){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5 | RCC_APB1Periph_TIM12 , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
   // Time Base configuration 
  TIM_TimeBaseStructure.TIM_Prescaler = 33-1;//частота = TIM_APB1CLK(84МГц)/33, один тик 3,92857Е-7.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM2, DISABLE);
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM3, DISABLE);
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM4, DISABLE);
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM5, DISABLE);
  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM12, DISABLE);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 66-1;//частота = TIM_APB2CLK(168МГц)/66, один тик 3,92857Е-7.
  TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM2, DISABLE); 
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;       
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;  
  TIM_OCInitStructure.TIM_Pulse = 65535;
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1Init(TIM5, &TIM_OCInitStructure);
  TIM_OC2Init(TIM5, &TIM_OCInitStructure);
  TIM_OC1Init(TIM9, &TIM_OCInitStructure);
  TIM_OC2Init(TIM9, &TIM_OCInitStructure);
  TIM_OC1Init(TIM12, &TIM_OCInitStructure);
  TIM_OC2Init(TIM12, &TIM_OCInitStructure);
  
    
  TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE);// 
  TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
  TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
  TIM_ITConfig(TIM9, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
  TIM_ITConfig(TIM12,TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
  TIM_Cmd(TIM2, DISABLE);
  TIM_Cmd(TIM3, DISABLE);
  TIM_Cmd(TIM4, DISABLE);
  TIM_Cmd(TIM5, DISABLE);
  TIM_Cmd(TIM9, DISABLE);
  TIM_Cmd(TIM12, DISABLE); 
   
  TIM2->SR = 0;
  TIM3->SR = 0;
  TIM4->SR = 0;
  TIM5->SR = 0;
  TIM9->SR = 0;
  TIM12->SR = 0;
  //
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
  
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);
  TIM6->SR = 0;  
}

/*******************************************************************************/
void EXTI_Configuration(void){
  //перед дальнейшей настройкой очистим регистры EXTI
  EXTI_DeInit();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  //подключаем необходимые порты к линиям EXTI
  //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//CFI1 линия 2
  //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//CFI2 линия 3
  //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//CFU2 линия 4
  //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);//CFU1 линия 5
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource7);//SYNC1 линия 7
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource8);//SYNC2 линия 8
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource9);//SYNC3 линия 9
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource10);//FSN линия 10
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource11);//FSP линия 11
     
  //прерывания по фронту у  сигналов синхронизации и CFU1 - EXTI_Line5 CFI1 - EXTI_Line2 
  EXTI->RTSR = EXTI_Line7 | EXTI_Line8 | EXTI_Line9 /*| EXTI_Line5*/; //
  //прерывания по спаду у  сигналов синхронизации и CFI1
  EXTI->FTSR = EXTI_Line7 | EXTI_Line8 | EXTI_Line9 /* | EXTI_Line2*/; //
  //очистка флогов прерываний 
  EXTI->PR = EXTI_Line0 |/*EXTI_Line2 |*/ EXTI_Line3 | EXTI_Line4 | /*EXTI_Line5 |*/ EXTI_Line7 | EXTI_Line8 | EXTI_Line9 | EXTI_Line10 | EXTI_Line11; 
  //далее разрешаем линии, которые должны будут в будущем вызывать прерывания  
  EXTI->IMR = EXTI_Line0 |/*EXTI_Line2 |*/ EXTI_Line3 | EXTI_Line4 | /*EXTI_Line5 |*/ EXTI_Line7 | EXTI_Line8 | EXTI_Line9 | EXTI_Line10 | EXTI_Line11;
  
}
/*******************************************************************************
* Function Name  : Watchdog_configuration
* Description    : Настройка сторожевого таймера
* Input          : u16 reset_time_ms - время до перезагрузки в мс (от 1 до 511)
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
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;//таймер MOBBUS Slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;//Таймер ШИМ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
   
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;//таймер общего назначения(DIO)
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;//таймер для LnkMngr
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//LnkMngr
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
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
    
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;//данные с АЦП приняты
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream4_IRQn;//данные с АЦП приняты
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
   
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//запуск расчёта данных синхронизации с платы DExS.CPU.VTEG
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn; //SD-card
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_Init(&NVIC_InitStructure);
  
}

void Start_init (void){//начальная инициализация. 
  //разрешаем работу ДМА(для уартов)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  GPIO_Configuration();//конфигурируем порт
  
  ModbusClientInit();//инициализируем работу интерфейсов модбас-клиента  
  NVIC_Configuration();//конфигурируем прерывания
  //Watchdog_configuration(3);//конфигурируем вотчдог на 3 мс
  
  //так же разрешаем работу системного таймера, для отладки
  //число для перезагрузки 0xffff для удобства расчета (16 бит)
  SysTick->LOAD  = 0xffff;      /* set reload register */  
  SysTick->VAL   = 0;           /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;   
}

//инициализация периферии
void Init (void){ 
  Start_init();//начальная инициализация, достаточная для настройки устройства
  TIM6_Configuration();//перенёс для искл ложного включения реле
  EXTI_Configuration(); 
}




