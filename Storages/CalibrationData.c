#include "CalibrationData.h"//
#include "bastypes.h"//содержит описания структур данных
#include "flashdata.h"//содержит описания структур данных
#include "ramdata.h"

//#include "bastypes.h"//содержит описания структур данных
#pragma location = ".cdsection"
const TCD_DATA CD_DATA =
{
    8,  // serial_number;
    10, //tdir_Modbus_RS485;
    10, //tdir_Modbus_option;    
    10, //tdir_Modbus_USB;
    0x0401, //Modbus_Local;
    5000,   //lnk_mngr_t_TO;
    1000,   //lnk_mngr_t_ChSl;
    10,     //lnk_mngr_t_DIR;
    //регулятор
    0x03E9,//tS16 pssK1F;//9
    0x0FA1,//tS16 pssK0F;//10
    0x0001,//tS16 pssK1IF;//11    
    //данные с платы DExS.M4CPU, смещение нуля
    0,  // Oadc_Uload;
    0,  // Oadc_Iload;
    0,  // Oadc_In1;
    0,  // Oadc_In2;
    0,  // Oadc_In3;
    //данные с платы DExS.CPU.VTEG, смещение нуля 
    20,  //UsgMIN// Oadc_Upwr_ab;
    1,   //IsgMin// Oadc_Upwr_bc;
    0,  // Oadc_Upwr_ca;
    0,  // Oadc_Ipwr;
    0,  // Oadc_Usg_ab;
    0,  // Oadc_Usg_bc;
    0,  // Oadc_Usg_ca;
    //0,  // Align2
    //PSS 
    0x07D0, //u16 T0F;// = 2
    0x0014, //u16 Ta0F;// = 0.02
    0x0032, //u16 T1F;// = 0.05
    0x0096, //u16 Ta1F;// = 0.15
    0x0014, //u16 T1IF;// = 0.02
    0x0096, //u16 Ta1IF;// = 0.15     
    //данные с платы DExS.M4CPU, коэффициент преобразования
    1.0, //  Kadc_Uload;
    1.0, //  Kadc_Iload;
    1.0, //  Kadc_In1;
    1.0, //  Kadc_In2;
    1.0, //  Kadc_In3;
    //данные с платы DExS.CPU.VTEG, коэффициент преобразования
    1.0, //  Kadc_Upwr_ab;
    1.0, //  Kadc_Upwr_bc;
    1.0, //  Kadc_Upwr_ca;
    1.0, //  Kadc_Ipwr;
    1.0, //  Kadc_Usg_ab;
    1.0, //  Kadc_Usg_bc;
    1.0, //  Kadc_Usg_ca;
    //коэффициенты для платы DExS.PWR
    95.69378,    //S_DExS_PWR_R;//шкала для сопротивления изоляции
    0.361689815, //S_DExS_PWR_F;//шкала для частоты 0,361689815
    0.09191,     //S_DExS_PWR_U;//шкала для напряжения
    //Фильтрация аналоговых сигналов
    0x00C8, //fUstat;
    0x00C8, //fIstat;
    2048, //TfUload;
    1024, //TfIload;
    200, //TfItt;//датчиков тока на входе выпрямителя
    200, //TUfpwr;
    200, //TfIpwr;
    200, //TfUsg;
    ////////////////////////////////
    0, //CFI_OFFSET;//смещение угла Фи
    0x00C8, //fFi постояная времени фильтра угла Фи
    0x00C8, //fFreq постояная времени фильтра частота напряжения статора
    ////////////////////////////////
    0x0401, //Modbus_option; 
    0x0401, //Modbus_USB;   
    0x0401, //Modbus_RS485;
    0x0001, //IsensorLocation
    //продолжение для платы DExS.PWR
    0.17,   //float S_DExS_PWR_UTSNG;//шкала для напряжения трансформатора собственных нужд генератора
    0.05,    //float S_DExS_PWR_UACC; //шкала для напряжения АКБ
    //защита от неисправности вращающегося диода
    10000.0f, //частота дискретизации в Гц;
    400.0f,   //частота искомой гармоники в Гц;
    80.0f,    //полоса фильтра для поиска гармоники в Гц;   
    2.5f,     //коэффициент гармоники, при превышении которого фиксируется предупреждение, в %;
    3.0f,     //задержка срабатывания предупреждения, в секундах;
    20.0f,    //коэффициент гармоники, при превышении которого фиксируется авария, в %;
    0.5f,     //задержка срабатывания аварии, в секундах.
    //
    0
};

//копия уставок в оперативной памяти
struct TCD_DATA CD_DATA_RC;
////////////////////////////////////////////////////////////////////////////////
static bool bCDataLoaded = FALSE;//
////////////////////////////////////////////////////////////////////////////////
void CDataMkRamCpy (void);//создание копии уставок в ОЗУ
bool ChDataChk      (void);//сравнение уставок с их копией. TRUE если есть отличия
void CDataSave     (void);//сохранение копии уставок в CD
bool CDataLoaded   (void);//проверка на то что уставки выгружены в ОЗУ
////////////////////////////////////////////////////////////////////////////////
//создание копии уставок в ОЗУ
void CDataMkRamCpy (void){
  u8_mem_cpy( (u8*)&CD_DATA, (u8*)&CD_DATA_RC, sizeof (TCD_DATA));
  bCDataLoaded = TRUE;//сигналю что выгрузил уставки в ОЗУ
}

//сравнение уставок с их копией. TRUE если есть отличия
bool CDataChk (void){
  return ChkBuffersByWord((u32)&CD_DATA,
                             (u32)&CD_DATA_RC,
                                sizeof (TCD_DATA) / 2);
  //TRUE  - блоки различаются
  //FALSE - блоки идентичны
}

//сохранение копии калибровок во flash
void CDataSave     (void){
  if (CDataChk()){//если данные отличаются 
    SaveCD();   //то сохранить их
  }
}

//проверка на то что уставки выгружены в ОЗУ
bool CDataLoaded   (void){
  return bCDataLoaded;
}