#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"

#include "init.h"

#include "DExS.M4CPU_BoardLEDs.h"

#include "STM32F4xx_Intmash_BKP.h"
#include "ramdata.h"
#include "CalibrationData.h"
#include "id.h"
                         
const char * IDtext = "Application v1.0.0 07.01.2022 www.intmash.ru";

int main(void) {
  
  IDinit(CD_DATA.serial_number, (char *) IDtext);

  Init();
  LED2_ON;  
  LED1_ON;
  LED3_OFF;
  LED4_OFF;

  BkpSram_Configuration();
  while (1)
  { 
    ////////////////////////////////////////////////////////////////////////////    
    if (RAM_DATA.FLAGS0.bits.T1000Hz) {
      RAM_DATA.FLAGS0.bits.T1000Hz = 0;
      static tU16 blink = 1000;
      (blink)
        ? (blink--)
        : ((LED2_ST) ? (LED2_ON) : (LED2_OFF), blink = 1000); 
    }
  }  
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1) {
  }
}
#endif