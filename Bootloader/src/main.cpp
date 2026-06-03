#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"
#include "init.h"
#include "id.h"
#include "DExS.M4CPU_BoardLEDs.h"
#include "ramdata.h"
#include "bootloader.h"
                         
const char * IDtext = "Bootloader iCM.4X.RS485 v1.4.0 16.10.2025 www.intmash.ru";

int main(void){

  GPIO_INIT_Configuration();
  INIT_ON;
  if (INIT_IST == 0) {
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
  LED_WRN_OFF; 
  LED_RUN_OFF;
  LED_LINK_OFF;
  LED_ALARM_OFF;

  __enable_irq(); 

  while (1)
  {
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
