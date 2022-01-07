#ifndef CalibrationData_H
#define CalibrationData_H

#include "stm32f4xx.h"
#include "IntmashLib/Intmash_bastypes.h"

///////////////////////////////////////////////////////////////////////////////
typedef struct TCD_DATA{
    u32 serial_number;//0,1
    u16 tdir_Modbus_RS485;//2
    u16 tdir_Modbus_option;//3    
    u16 tdir_Modbus_USB;//4
    tU16Union Modbus_RS485;//   
    tU16Union Modbus_option;//1 
    tU16Union Modbus_USB;//2
 } TCD_DATA;
///////////////////////////////////////////////////////////////////////////////

//параметры расположеные  в CD
extern const TCD_DATA CD_DATA;
extern const char SID_bkfdsection[];

#define CD_DATA_PREFIX 0xC
#define CD_DATA_SIZE sizeof(TCD_DATA)


#endif 