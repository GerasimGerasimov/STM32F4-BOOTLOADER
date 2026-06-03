#ifndef DEXS_M4CPU_BOARD_LEDS_H
#define DEXS_M4CPU_BOARD_LEDS_H  

#include "stm32f4xx.h"//библиотека StdPeriph_Driver

#define LED_RUN_ON   GPIOC->BSRRH = GPIO_Pin_7
#define LED_RUN_OFF  GPIOC->BSRRL = GPIO_Pin_7
#define LED_RUN_ST   GPIOC->ODR & GPIO_Pin_7

#define LED_WRN_ON   GPIOC->BSRRH = GPIO_Pin_9
#define LED_WRN_OFF  GPIOC->BSRRL = GPIO_Pin_9
#define LED_WRN_ST   GPIOC->ODR & GPIO_Pin_9

#define LED_LINK_ON   GPIOC->BSRRH = GPIO_Pin_6
#define LED_LINK_OFF  GPIOC->BSRRL = GPIO_Pin_6
#define LED_LINK_ST   GPIOC->ODR & GPIO_Pin_6

#define LED_ALARM_ON   GPIOC->BSRRH = GPIO_Pin_8
#define LED_ALARM_OFF  GPIOC->BSRRL = GPIO_Pin_8
#define LED_ALARM_ST   GPIOC->ODR & GPIO_Pin_8

#define INIT_ON   GPIOA->BSRRL = GPIO_Pin_0
#define INIT_OFF  GPIOA->BSRRH = GPIO_Pin_0
#define INIT_OST   (GPIOA->ODR & GPIO_Pin_0)
#define INIT_IST   (GPIOA->IDR & GPIO_Pin_0) //вход на ноге USB_P PA12 если его перед стартом
                                              //замкнуть на GND то Bootloader не будет стартовать приложение

#endif



