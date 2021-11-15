#include "CalibrationData.h"//
#include "bastypes.h"//содержит описания структур данных
#include "ramdata.h"
#include "STM32F4xx_Intmash_Flash.h"


//#include "bastypes.h"//содержит описания структур данных
#pragma location = ".cdsection"
const TCD_DATA CD_DATA =
{
    8,  // serial_number;
    10, //tdir_Modbus_RS485;
    10, //tdir_Modbus_option;    
    10, //tdir_Modbus_USB;
    0x0401, //Modbus_option; 
    0x0401, //Modbus_USB;   
    0x0401, //Modbus_RS485;

};

//копия уставок в оперативной памяти
struct TCD_DATA CD_DATA_RC;

//запись во флэш память копии калибровок из ОЗУ
void __SaveCD(void){
  //записать данные из временного буфера
  __disable_irq(); // handles nested interrupt
  FlashSectorWrite((tU32*)&CD_DATA, (tU32*) &CD_DATA_RC);  
  __enable_irq(); // handles nested interrupt
}
