/**
  ******************************************************************************
  * @version V1.0.0
  * @date    17-08-2016
  * @brief   Файл содержит макросы для управления светодиодами на плате 
  * DExS.M4CPU
  ******************************************************************************
  * @Как с этим работать:
  * - Пины портов микроконтроллера конфигурируются в init.c.
  * - Макросы используются для включения, выключения и определения состояния.
  ******************************************************************************
  * @Заметки:
  * Количество светодиодов на плате зависит от ревизии используемой платы
  ******************************************************************************
  */
#ifndef DEXS_M4CPU_BOARD_LEDS_H
#define DEXS_M4CPU_BOARD_LEDS_H  
  
#include "stm32f4xx.h"//библиотека StdPeriph_Driver

#define LED1_ON   GPIOF->BSRRH = GPIO_Pin_11
#define LED1_OFF  GPIOF->BSRRL = GPIO_Pin_11
#define LED1_ST   (GPIOF->ODR & GPIO_Pin_11)

#define LED2_ON   GPIOB->BSRRH = GPIO_Pin_12
#define LED2_OFF  GPIOB->BSRRL = GPIO_Pin_12
#define LED2_ST   (GPIOB->ODR & GPIO_Pin_12)

#define LED3_ON   GPIOH->BSRRH = GPIO_Pin_2
#define LED3_OFF  GPIOH->BSRRL = GPIO_Pin_2
#define LED3_ST   (GPIOH->ODR & GPIO_Pin_2)

#define LED4_ON   GPIOH->BSRRH = GPIO_Pin_3
#define LED4_OFF  GPIOH->BSRRL = GPIO_Pin_3
#define LED4_ST   (GPIOH->ODR & GPIO_Pin_3)

#endif



