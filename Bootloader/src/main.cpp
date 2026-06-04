#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"
#include "init.h"
#include "id.h"
#include "DExS.M4CPU_BoardLEDs.h"
#include "ramdata.h"
#include "bootloader.h"
                         
const char * IDtext = "Bootloader OP.OLED.CM4 v1.3.1 04.06.2026 www.intmash.ru";

int main(void) {
  
  GPIO_INIT_Configuration();
  INIT_ON;
  if(INIT_IST != 0){
    LED5_OFF;
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
  LED5_ON;
  LED_WRN_OFF; 
  LED_RUN_OFF;
  LED_LINK_OFF;
  LED_ALARM_OFF;
  
  __enable_irq(); 

  while (1)
  { 
    (INIT_IST)? LED5_OFF : LED5_ON;
    ////////////////////////////////////////////////////////////////////////////    
    if (RAM_DATA.FLAGS0.bits.T1000Hz) {
      RAM_DATA.FLAGS0.bits.T1000Hz = 0;
      static tU16 blink = 1000;
      (blink)
        ? (blink--)
        : ((LED_ALARM_ST) ? (LED_ALARM_ON) : (LED_ALARM_OFF), blink = 1000);
    }
  }  
}
