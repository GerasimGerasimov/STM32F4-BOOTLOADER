#ifndef DEXS_M4CPU_BOARD_LEDS_H
#define DEXS_M4CPU_BOARD_LEDS_H  
  
#include "stm32f4xx.h"//библиотека StdPeriph_Driver

#define LED_RUN_ON   GPIOB->BSRRH = GPIO_Pin_11//13
#define LED_RUN_OFF  GPIOB->BSRRL = GPIO_Pin_11
#define LED_RUN_ST   GPIOB->ODR & GPIO_Pin_11

#define LED_WRN_ON   GPIOB->BSRRH = GPIO_Pin_1//10
#define LED_WRN_OFF  GPIOB->BSRRL = GPIO_Pin_1
#define LED_WRN_ST   GPIOB->ODR & GPIO_Pin_1

#define LED_LINK_ON   GPIOB->BSRRH = GPIO_Pin_10//12
#define LED_LINK_OFF  GPIOB->BSRRL = GPIO_Pin_10
#define LED_LINK_ST   GPIOB->ODR & GPIO_Pin_10

#define LED_ALARM_ON   GPIOB->BSRRH = GPIO_Pin_12//11
#define LED_ALARM_OFF  GPIOB->BSRRL = GPIO_Pin_12
#define LED_ALARM_ST   GPIOB->ODR & GPIO_Pin_12

#define INIT_ON   GPIOA->BSRRL = GPIO_Pin_12
#define INIT_OFF  GPIOA->BSRRH = GPIO_Pin_12
#define INIT_OST   (GPIOA->ODR & GPIO_Pin_12)
#define INIT_IST   (GPIOA->IDR & GPIO_Pin_12) //вход на ноге USB_P PA12 если его перед стартом
                                              //замкнуть на GND то Bootloader не будет стартовать приложение
#define LED5_ON   GPIOA->BSRRL = GPIO_Pin_0
#define LED5_OFF  GPIOA->BSRRH = GPIO_Pin_0
#define LED5_ST   (GPIOA->ODR & GPIO_Pin_0)

#endif



