#include "stm32f4xx.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "DExS.M4CPU_DIO.h"
#include "DExS.M4CPU_FRAMkey.h"
#include "RAMdata.h"
#include "flashdata.h"
#include "ModbusSlaveConf.h"
#include "DExS.M4CPU_BoardLEDs.h"


/***Конфигурация портов ввода/вывода*******************************************/

void GPIO_Configuration(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |\
                          RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
  
  
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//по умолчанию скорость 100МГц
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//по умолчанию без притяжки
  
//------------------------------------------------------------------------------------------------------------   
  //инициализируем аналоговые входы/выходы
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; 
  //порт А                        AIN8         AIN5         AIN2         AIN4
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //порт B                        AIN6         AIN7                 
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1;//
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //порт C                        Iload_1.2    In_1         In2          In3          AIN1         AIN3
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
  
  DIO_Reset();
  GPIOI->BSRRL=GPIO_Pin_1;//отключаем селекторы заранее
  GPIOA->BSRRL=GPIO_Pin_15;
  
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
  //порт H                       LED3         LED4         S2.6          S2.5          S2.4      
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  //порт I                        S2.3         SELECTOR_1   S1.6         S1.3         S1.4         S1.5      
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIOI->BSRRL=GPIO_Pin_1;
  GPIO_Init(GPIOI, &GPIO_InitStructure);

//------------------------------------------------------------------------------------------------------------   
  //Инициализируем ноги, связанные с переферией:
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  //подключаем альтернативные функции к нужным пинам:
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //Rx
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //Tx 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); //Rx
  
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
  //порт А                        UART4_Tx     UART4_Rx     USART2_Tx    USART2_Rx    USART1_Tx    USART1_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //порт B                        USART3_Tx     USART3_Rx
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_10 | GPIO_Pin_11;
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
  
}
/********************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
//разрешаем переходы по векторам и настраиваем преоритеты следующих прерываний:
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
  
  /*!< 3 bits for pre-emption priority, 1 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  /*Разрешаем следующие прерывания*/
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;//таймер общего назначения
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
  
  //все уарты
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
  
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
    
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;// 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;// 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
  
  //EXTI0  - для логирвоания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);  
  
  //прерывания от сигналов с платы синхронизации
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//Sync
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
  
  //завершение преобразований АЦП всех сигналов (или только сигналов платы DExS.M4CPU)
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 

  //завершения преобразовний АЦП сигналов с опциональной аналоговой платы 
  //(на случай, если используются только они)
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  //ддля sd карты
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_Init(&NVIC_InitStructure);
  
}



/***Базовая конфигурация АЦП для DExS.M4CPU************************************/
#define ADC_TIM_CLK ((tU32) 168000000)
#define PERIOD_10KHZ 16800
void BasicADCconfiguration(tU32 AinBufferAddr, tU32 AinOptBufferAddr, tU32 SamplingFrequency)
{ 
  //структура для кофигурации стрима DMA
  DMA_InitTypeDef DMA_InitStructure; 
  //сначала общие настройки любого стрима для работы с АЦП
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  
  //теперь зададим общие настройки работы АЦП
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC3, ENABLE);//в работе АЦП1,3   
  //объявление структур конфигурации и инициализация значениями по умолчанию
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_CommonStructInit(&ADC_CommonInitStructure);
  ADC_InitTypeDef ADC_InitStructure;
  ADC_StructInit(&ADC_InitStructure);
  //настройка взаимной работы АЦП
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//работают независимо
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//ADC_CLK = PCLK2/4=84MHz/4=24Mhz
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;//задержка между семплами  
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  //общие настройки для каждого АЦП
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;// режим скан 
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;//запускается по фронту испульса с Тим8
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T8_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //далее конфигурация зависит от аргументов:  
  
  //если адрес буфера задан 
  if(AinBufferAddr){
    //инициализируем стрим для сигналов платы DExS.M4CPU
    DMA_InitStructure.DMA_Channel = DMA_Channel_2;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC3->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = AinBufferAddr;  
    DMA_InitStructure.DMA_BufferSize = 9;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    //разрешаем прерывание по завершению приёма пачки данных
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC , ENABLE);
    //разрешаем работу стрима
    DMA_Cmd(DMA2_Stream0, ENABLE);
    
    //завершаем конфигурацию АЦП
    ADC_InitStructure.ADC_NbrOfConversion = 9;  
    ADC_Init(ADC3, &ADC_InitStructure);
    ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
    ADC_DMACmd(ADC3, ENABLE);
    //Конфигурация каналов ацп
    ADC_RegularChannelConfig(ADC3, ADC_Channel_6,  1, ADC_SampleTime_28Cycles);//Ustat  
    ADC_RegularChannelConfig(ADC3, ADC_Channel_4,  2, ADC_SampleTime_28Cycles);//Istat
    ADC_RegularChannelConfig(ADC3, ADC_Channel_5,  3, ADC_SampleTime_28Cycles);//Istatmax
    ADC_RegularChannelConfig(ADC3, ADC_Channel_7,  4, ADC_SampleTime_28Cycles);//Uload 
    ADC_RegularChannelConfig(ADC3, ADC_Channel_8,  5, ADC_SampleTime_28Cycles);//Iload1.1 
    ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 6, ADC_SampleTime_28Cycles);//Iload1.2 
    ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 7, ADC_SampleTime_28Cycles);//In1 
    ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 8, ADC_SampleTime_28Cycles);//In2 
    ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 9, ADC_SampleTime_28Cycles);//In3 
    //включаем АЦП в работу
    ADC_Cmd(ADC3, ENABLE);
  }
  
  //если адрес буфера задан - инициализируем стрим для сигналов опц. платы 
  if(AinOptBufferAddr){
    //инициализируем нужные параметры
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = AinOptBufferAddr;  
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_Init(DMA2_Stream4, &DMA_InitStructure);
    
    if(AinBufferAddr==0){//если сигналы платы DExS.M4CPU не используются
      //нужны только сигналы с опциональной платы
      //разрешаем прерывание по завершению приёма пачки данных
      DMA_ITConfig(DMA2_Stream4, DMA_IT_TC , ENABLE);
    }    
    //разрешаем работу стрима
    DMA_Cmd(DMA2_Stream4, ENABLE);   
    
    //завершаем конфигурацию АЦП
    ADC_InitStructure.ADC_NbrOfConversion = 8;   
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);   
    ADC_DMACmd(ADC1, ENABLE);  
    //Конфигурация каналов ацп
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_28Cycles);//AINboard_1(A1)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6,  2, ADC_SampleTime_28Cycles);//AINboard_2(A2)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime_28Cycles);//AINboard_3(A3)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7,  4, ADC_SampleTime_28Cycles);//AINboard_4(A4)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  5, ADC_SampleTime_28Cycles);//AINboard_5(DAC2)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8,  6, ADC_SampleTime_28Cycles);//AINboard_6(A5)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9,  7, ADC_SampleTime_28Cycles);//AINboard_7(A6)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  8, ADC_SampleTime_28Cycles);//AINboard_8(DAC1)
    //включаем АЦП в работу   
    ADC_Cmd(ADC1, ENABLE);    
  } 
  
  //инициализируем таймер триггера преобразований АЦП
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);//подаем тактирование
  //считаем период таймера при прискейлере 0
  //если частота=0, то задается по умолчанию частота 10 кГц
  tU16 Period=(SamplingFrequency==0) ? PERIOD_10KHZ : ((tU16)(ADC_TIM_CLK/SamplingFrequency));
  //базовая конфигурация таймера
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;    
  TIM_TimeBaseStructure.TIM_Prescaler = 0;//частота = TIM8CLK(168МГц)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = Period-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);  
  
  //конфигурация импульса, т.е. канала сравнения
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;       
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = Period/2;//ширина импульса = половина периода
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);
  //выбираем настроеный канал сравнения, как триггерный выход таймера
  TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
  //включаем таймер
  TIM_Cmd(TIM8, ENABLE);    
}


/***Конфигурация EXTI для синхронизации с сетью********************************/
void SyncEXTIconfiguration(void)
{
  //По фронту/спаду по всем ногам
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource7);//SYNC1 линия 7
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource8);//SYNC2 линия 8
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource9);//SYNC3 линия 9 
  EXTI->RTSR |= EXTI_Line7 | EXTI_Line8 | EXTI_Line9;
  EXTI->FTSR |= EXTI_Line7 | EXTI_Line8 | EXTI_Line9;
  EXTI->PR |= EXTI_Line7 | EXTI_Line8 | EXTI_Line9; 
  EXTI->IMR |= EXTI_Line7 | EXTI_Line8 | EXTI_Line9;  
}

/***Таймер общего назначения с прерыванием раз в 0,001 сек TIM6****************/
void CommonTimerConfiguration(void){
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
  TIM6->SR = 0;
    
}
/*******************************************************************************
* Function Name  : Watchdog_configuration
* Description    : Настройка сторожевого таймера
* Input          : u16 reset_time_ms - время до перезагрузки в мс (от 1 до 511)
* Output         : None
* Return         : None
********************************************************************************/
#define IWDG_1ms_TICKS 1
#define IWDG_KEY_RELOAD 0xAAAA
#define IWDG_KEY_START  0xCCCC
#define IWDG_PRESC_32   ((uint8_t)0x03)
void Watchdog_configuration(tU16 reset_time_ms){
  RCC_LSICmd(ENABLE);
  while (!(RCC->CSR & RCC_CSR_LSIRDY)){};
  IWDG->KR = IWDG_WriteAccess_Enable;
  IWDG->RLR = reset_time_ms * IWDG_1ms_TICKS;
  IWDG->KR = IWDG_KEY_RELOAD;
  IWDG->KR = IWDG_KEY_START;
}


/***Начальная инициализация, достаточная для проверки секторов и работы связи**/
void StartInit (void){   
  
  //разрешаем работу системного таймера, для отладки
  //число для перезагрузки 0xffff для удобства расчета (16 бит)
  SysTick->LOAD  = 0xffff;      // set reload register  
  SysTick->VAL   = 0;           // Load the SysTick Counter Value
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;   
  
  GPIO_Configuration();//конфигурируем порты  
  //вырубим все светики на борде
  LED1_OFF; 
  LED2_OFF;
  LED3_OFF;
  LED4_OFF;  
  
  CheckFRAMkey();//проверяем ключ параметров  
  CheckFlashData((tU32*)&FLASH_DATA, (tU32*)&BKFLASH_DATA);//проверяем сектора уставок
  
  if(FlashStatus.Bits.FLASH_DATA_ERR==0){
    //если сектора уставок в порядке
    if(FRAMkeyStatus.Bits.FRAM_ERROR){//смотрим на состояние ключа параметров
      //пробуем восстановить ключ параметрв 
      FRAMwriteMassive((tU8*)&FLASH_DATA, FramSectorSize, 0, FRAM_SECTOR);
      CheckFRAMkey();//проверяем ключ параметров еще раз, может быть его вообще нет 0_о
    }
    else{
      //и если основной сектора ключа параметров впорядке - сравним контрольные суммы
      tU16* FLASHcrc = (tU16*)((tU32)&FLASH_DATA + (tU32)(FlashTmpBufferSize-2));//
      tU16 FRAMcrc=0;
      FRAMreadMassive((u8*)&FRAMcrc, 2, FramSectorSize-2, FRAM_SECTOR);//
      if(FRAMcrc != *FLASHcrc){//если контрольные суммы не равны - переписать из ключа параметров уставки        
        FRAMreadMassive((u8*)FlashTmpBuffer, FlashTmpBufferSize, 0, FRAM_SECTOR);//подготовили буфер к записи 
        FlashDataProtectedWrite((tU32*)&FLASH_DATA , (tU32*)&BKFLASH_DATA );//Запись с проверкой.
      }
    }
  }
  else {//если сектора уставок не в порядке
    if((FRAMkeyStatus.Bits.FRAM_ERROR)==0){
      //но ключ параметров в порядке - попробуем восстановить.
      FlashStatus.Bits.FLASH_DATA_ERR=0;//сбрасываем статус бит, иначе не запишется.
      FRAMreadMassive((u8*)FlashTmpBuffer, FlashTmpBufferSize, 0, FRAM_SECTOR);//подготовили буфер к записи 
      FlashDataProtectedWrite((tU32*)&FLASH_DATA , (tU32*)&BKFLASH_DATA );//Запись с проверкой.    
    }
  }  
  //подаем тактирование на оба контроллера DMA
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
  //подаем тактирование регистров SYSCFG (для EXTI)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  ModbusClientInit();//инициализируем работу интерфейсов модбас-клиента  
  NVIC_Configuration();//конфигурируем прерывания
  
  //выставляем флаги ошибок
//  if(FlashStatus.Bits.FLASH_DATA_ERR)   RAM_DATA.Flags.Bits.FLASH_ERR=1;
//  if(FlashStatus.Bits.BACKUP_DATA_ERR)  RAM_DATA.Flags.Bits.FLASH_BKP_ERR=1;
//  if(FRAMkeyStatus.Bits.FRAM_ERROR)     RAM_DATA.Flags.Bits.FRAM_KEY_ERR=1;
//  if(FRAMkeyStatus.Bits.FRAM_BKP_ERROR) RAM_DATA.Flags.Bits.FRAM_KEY_BKP_ERR=1;
  
  
}






