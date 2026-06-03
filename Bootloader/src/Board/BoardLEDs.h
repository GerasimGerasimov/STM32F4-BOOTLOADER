#ifndef DEXS_M4CPU_BOARD_LEDS_H
#define DEXS_M4CPU_BOARD_LEDS_H  

#include "stm32f4xx.h"//библиотека StdPeriph_Driver

#define LED_1_ON   (GPIOC->BSRRL = GPIO_Pin_3)
#define LED_1_OFF  (GPIOC->BSRRH = GPIO_Pin_3)
#define LED_1_ST   (GPIOC->ODR & GPIO_Pin_3)

#define LED_2_ON   (GPIOC->BSRRL = GPIO_Pin_2)
#define LED_2_OFF  (GPIOC->BSRRH = GPIO_Pin_2)
#define LED_2_ST   (GPIOC->ODR & GPIO_Pin_2)

#define INIT_ON    (GPIOB->BSRRL = GPIO_Pin_2)
#define INIT_OFF   (GPIOB->BSRRH = GPIO_Pin_2)
#define INIT_OST   (GPIOB->ODR & GPIO_Pin_2)
#define INIT_IST   (GPIOB->IDR & GPIO_Pin_2) //вход на ноге USB_P PA12 если его перед стартом
                                              //замкнуть на GND то Bootloader не будет стартовать приложение

#endif



