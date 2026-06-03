#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"
#include "init.h"
#include "id.h"
#include "BoardLEDs.h"
#include "ramdata.h"
#include "bootloader.h"
                         
const char * IDtext = "Bootloader ALS v1.3.0 02.06.2026 www.intmash.ru";

int main(void){

  GPIO_INIT_Configuration();
  INIT_ON;
  if((INIT_IST != 0) && (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == Bit_RESET)){
    if(isBootLoaderMustBeStart() != true){
      if(isApplicationToBe()){
        if(isApplicationReadyToStart()){
          jumpToApplication();
        }
      }
    }
  }

  IDinit(1, (char *) IDtext);
  Init();
  LED_1_ON;
  LED_2_OFF;
  LED_3_OFF;

  __enable_irq(); 

  while (1)
  {
    ////////////////////////////////////////////////////////////////////////////    
    if (RAM_DATA.FLAGS0.bits.T1000Hz) {
      RAM_DATA.FLAGS0.bits.T1000Hz = 0;
      static tU16 blink = 1000;
      (blink)
        ? (blink--)
        : ((!LED_2_ST) ? (LED_2_ON) : (LED_2_OFF), blink = 1000);
    }
  }  
}
