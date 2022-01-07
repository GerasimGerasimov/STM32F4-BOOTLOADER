/**
  ******************************************************************************
  * @version V1.0.0
  * @date    17-08-2016
  * @brief   ���� �������� ������� ��� ���������� ������������ �� ����� 
  * DExS.M4CPU
  ******************************************************************************
  * @��� � ���� ��������:
  * - ���� ������ ���������������� ��������������� � init.c.
  * - ������� ������������ ��� ���������, ���������� � ����������� ���������.
  ******************************************************************************
  * @�������:
  * ���������� ����������� �� ����� ������� �� ������� ������������ �����
  ******************************************************************************
  */
#ifndef DEXS_M4CPU_BOARD_LEDS_H
#define DEXS_M4CPU_BOARD_LEDS_H  
  
#include "stm32f4xx.h"//���������� StdPeriph_Driver

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



