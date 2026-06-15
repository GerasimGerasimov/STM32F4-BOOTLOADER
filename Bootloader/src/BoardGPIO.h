#ifndef DEXS_M4CPU_BOARD_LEDS_H
#define DEXS_M4CPU_BOARD_LEDS_H  

#include "stm32f4xx.h"//библиотека StdPeriph_Driver

#define INIT_ON   GPIOB->BSRRL = GPIO_Pin_2
#define INIT_OFF  GPIOB->BSRRH = GPIO_Pin_2
#define INIT_OST   (GPIOB->ODR & GPIO_Pin_2)
#define INIT_IST   (GPIOB->IDR & GPIO_Pin_2) //вход на ноге если его перед стартом
                                              //замкнуть на GND то Bootloader не будет стартовать приложение

#endif



