#pragma once

#include "Intmash_bastypes.h"
#include "STM32F4xx_Intmash_USART.h"

//структуры драйверов UART и слейвов
extern Intmash_Usart UARTtoUSB; // связь по USB
extern Intmash_Usart UARTtoRS485_1; // связь по RS485_1

void initUsart1();
void initUsart3();
