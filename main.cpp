#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"

#include "init.h"
#include "id.h"

#include "STM32F4xx_Intmash_Flash.h"
#include "DExS.M4CPU_BoardLEDs.h"

#include "STM32F4xx_Intmash_BKP.h"
//////////////////////////
#include "ramdata.h"
#include "CalibrationData.h"   
                         
const char * IDtext = "-=OTLOAD=- v1.0.0 07.12.2021 www.intmash.de";

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
  while (1)
  {
  }
}
#endif


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
