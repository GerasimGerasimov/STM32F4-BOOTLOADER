#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"

#include "init.h"
#include "id.h"

#include "STM32F4xx_Intmash_Flash.h"
#include "STM32F4xx_Intmash_RTC.h"
#include "DExS.M4CPU_BoardLEDs.h"

#include "STM32F4xx_Intmash_BKP.h"
//////////////////////////
#include "ramdata.h"
#include "flashdata.h"
#include "CalibrationData.h"   

uint32_t LastTimeRegisterValue=0;
char * IDtext = "DExS.M4CPU.ARVSG v1.16.8.7 14.11.2021 www.intmash.ru";
void SettingsFlashSave(void);//Cохранение параметров FLASH
void SettingsCDSave(void);//Cохранение параметров CD
void logService(void);//работа с SD-картой

int main(void) {  
  TDateTimeIntRTC * ActualTimeDate;//элемент структуры для получения времени в MDLL
  ActualTimeDate=(TDateTimeIntRTC *)RAM_DATA.TimeBuf;
  
  RAM_DATA.DO = 0;
  CDataMkRamCpy ();//создание копии калибровочных данных
  IDinit(CD_DATA_RC.serial_number, IDtext);
  Init();
  LED2_OFF;
  LED1_OFF;
  //RAM_DATA.FLAGS0.bits.AsyncRun;
  //если есть ошибка памяти - работа в безопасном режиме (только светики и связь)
  /*while (FlashStatus.Bits.FLASH_DATA_ERR){    
    if(RAM_DATA.FLAGS.BA.flash_error==0){
      FlashStatus.Bits.FLASH_DATA_ERR=0;
      FlashStatus.Bits.BACKUP_DATA_ERR=0;
      RAM_DATA.FLAGS.BA.backup_error=0;
    }
  }*/FlashStatus.Bits.FLASH_DATA_ERR =0;//временно сбрасываем вручную
  
  FlashDataMkRamCpy();//создание копии параметров
  LED2_ON;  
  LED1_ON;
  LED3_OFF;
  LED4_OFF;
  RAM_DATA.Qpwm=0.0;

  BkpSram_Configuration();
  
  RAM_DATA.RCMD_Rst_Timer = 1000;//1 сек.время таймаута с Арбитром//ModbusInit();
  RAM_DATA.FLAGS0.bits.FLASH_WRITE = 1;//флэш был модифицирован чтобы вызвать чтение
  //------------------------------------------------------------------------------
  
  //часы
  IntmashRTC_Configuration(); //инициализация 
  while (1)
  { 
    ////////////////////////////////////////////////////////////////////////////    
    if (RAM_DATA.FLAGS0.bits.T1000Hz) {
      //////////////////////////////////////////////////////////////////////////
      SettingsFlashSave();//Cохранение параметров FLASH 
      SettingsCDSave();//Сохраннеие калибровочных параметров
      RAM_DATA.FLAGS0.bits.T1000Hz = 0;
    }
    
    /***работа интерфейсов связи***************************************************/  
    //часы работают постоянно
    if(ActualTimeDate->Options){//если регистр опций не равен нулю - время остановлено
     /* if(ActualTimeDate->Options==SET_TIME_CMD){//если введена команда записи       
        IntmashRTC_SetDate(ActualTimeDate);//задать новое время
        IntmashRTC_SetTime(ActualTimeDate);//и дату
        ActualTimeDate->Options=0;//и запустить часы
      }*/
    }
    else if(LastTimeRegisterValue != (RTC->TR)){//время изменилось
      LastTimeRegisterValue = RTC->TR;//обновить регистр сравнения
      IntmashRTC_GetDateTime(ActualTimeDate);
    }
  }  
}

//проверка условий сохранения уставок из ОЗУ во флэш
void SettingsFlashSave(void){
  static u16 delay;// = 1000;
  //параметры сохраняются если:
  if (!RAM_DATA.FLAGS0.bits.PWR1) { //нет импульсов управления
          //проверить есть ли отличия в блока памяти
          if (FlashDataChk()) {//да, отличия есть
            //если есть авария, то данные в копии уставок восстанавливаются
            //из FLASH памяти
            /*
            if (RAM_DATA.FLAGS1.bits.FAULT){
              //восстановить даные
              FlashDataMkRamCpy();
            }
            else*/
              //сохранить из копии во FLASH
              if (delay) {
                delay--;
              }
              else {  
                FlashDataSave();
                delay = 1000;
              }   
          }
        }
  else {
    delay = 1000;
  }
}

//проверка условий сохранения уставок из ОЗУ в CD
void SettingsCDSave(void){
  static u16 delay;// = 1000;
  //параметры сохраняются если:
  if (!RAM_DATA.FLAGS0.bits.PWR1)
  { //нет импульсов управления
    //проверить есть ли отличия в блока памяти
    if (CDataChk()) {//да, отличия есть
      //если есть авария, то данные в копии уставок восстанавливаются
      //из FLASH памяти
      /*
      if (RAM_DATA.FLAGS1.bits.FAULT){
        //восстановить даные
        CDataMkRamCpy();
      }
      else*/
        //сохранить из копии во FLASH
        if (delay) {
          delay--;
        }
        else {  
          CDataSave();
          delay = 1000;
        }   
    }
  }
  else {
    delay = 1000;
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
