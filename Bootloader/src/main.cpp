#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"
#include "init.h"
#include "id.h"
#include "BoardGPIO.h"
#include "ramdata.h"
#include "bootloader.h"
                         
const char* IDtext = "Bootloader LESH v1.0.1 07.04.2026 www.intmash.ru";

int main(void){

  GPIO_INIT_Configuration();
  if (INIT_IST != 0){
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

  __enable_irq(); 

  while (1)
  {
    ////////////////////////////////////////////////////////////////////////////    
    if (RAM_DATA.FLAGS0.bits.T1000Hz) {
      RAM_DATA.FLAGS0.bits.T1000Hz = 0;
    }
  }  
}
