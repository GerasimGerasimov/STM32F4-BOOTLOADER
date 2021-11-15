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
    tU16Union Modbus_Local_CRB;//5    
    u16 lnk_mngr_t_TO;//6
    u16 lnk_mngr_t_ChSl;//7
    u16 lnk_mngr_t_DIR;//8
    //PSS https://pandia.ru/text/78/407/72268.php
    tS16 pssK1F;//9
    tS16 pssK0F;//10
    tS16 pssK1IF;//11
    //������ � ����� DExS.M4CPU, �������� ����
    u16 Oadc_Ur;//12
    u16 Oadc_Ir;//13
    u16 Oadc_In1;//14
    u16 Oadc_In2;//15
    u16 Oadc_In3;//16
    //������ � ����� DExS.CPU.VTEG, �������� ���� 
    u16 UsgMIN;//����������� ���������� ��� ����������� �������//tU16 __Oadc_Upwr_ab;//17
    u16 IsgMIN;//__Oadc_Upwr_bc;//18
    //�������� �����
    u16 Oadc_UpwrAB_UpwrBC;//L - UpwrAB, H - UpwrBC
    u16 Oadc_UpwrCA_UsgAB;//L - UpwrCA, H-UsgAB
    u16 Oadc_UsgBC_UsgCA;//L - UpwrBC, H-UsgCA
    u16 Oadc_Ipwr_UabError;//L-Ipwr H-UabError
    tU16 ___free_rC017;//23
    //u16 Align2;//24
     //������ � ����� DExS.M4CPU, ����������� ��������������
    u16 T0F;// = 2
    u16 Ta0F;// = 0.02
    u16 T1F;// = 0.05
    u16 Ta1F;// = 0.15
    u16 T1IF;// = 0.02
    u16 Ta1IF;// = 0.15   
    float Kadc_Ur;
    float Kadc_Ir;
    float Kadc_In1;
    float Kadc_In2;
    float Kadc_In3;
    //������ � ����� DExS.CPU.VTEG, ����������� ��������������
    float Kadc_Upwr_ab;
    float Kadc_Upwr_bc;
    float Kadc_Upwr_ca;
    float KadcIsg;
    float KadcUsgAB;
    float KadcUsgBC;
    float KadcUsgCA;
    //������������ ��� ����� DExS.PWR
    float S_DExS_PWR_R;//����� ��� ������������� ��������
    float S_DExS_PWR_F;//����� ��� �������
    float S_DExS_PWR_UEWG;//����� ��� ���������� ������� ����������� ����������
    //���������� ���������� ��������
    u16 fUstat;
    u16 fIstat;
    tU16 TfUr;
    u16 TfIr;
    u16 TfItt;//�������� ���� �� ����� �����������
    u16 TfUpwr;
    u16 TfIpwr;
    u16 TfUsg;
    /////////////////////////////////////////////
    s16 PhiZeroShift;//�������� ���� ��
    u16 fPhi;//��������� ������� ������� ���� ��
    u16 fFreq;//��������� ������� ������� ������� ���������� �������
    /////////////////////////////////////////////
    tU16Union Modbus_RS485;//   
    tU16Union Modbus_option;//1 
    tU16Union Modbus_USB;//2
    //p503=IsensorLocation/��� ����� ������ ����/TPrmList/xF002/r2001.L/ //x01#PhaseA/x02#PhaseB/x03#PhaseC/x00#Test/x00/
    u16 IsensorLocation;
    u16 __gap__0;
    //����������� ��� ����� DExS.PWR
    float S_DExS_PWR_UTSNG;//����� ��� ���������� �������������� ����������� ���� ����������
    float S_DExS_PWR_UACC; //����� ��� ���������� ���
    //�����/�� ������������ �����
    float Fdisc; //ROT_DIODE_F_DISC_DEFAULT  10000.0f //������� ������������� � ��;
    float Fbase; //ROT_DIODE_F_BASE_DEFAULT  400.0f   //������� ������� ��������� � ��;
    float Fband; //ROT_DIODE_F_BAND_DEFAULT  80.0f    //������ ������� ��� ������ ��������� � ��;  
    float KwrnL; //ROT_DIODE_K_WRN_DEFAULT   2.5f     //����������� ���������, ��� ���������� �������� ����������� ��������������, � %;
    float tWrnL; //ROT_DIODE_T_WRN_DEFAULT   3.0f     //�������� ������������ ��������������, � ��������;
    float KalmL; //ROT_DIODE_K_ALM_DEFAULT   20.0f    //����������� ���������, ��� ���������� �������� ����������� ������, � %;
    float tAlmL; //ROT_DIODE_T_ALM_DEFAULT   0.5f     //�������� ������������ ������, � ��������
    //
    u16 __gap__1;
 } TCD_DATA;
///////////////////////////////////////////////////////////////////////////////

//��������� ������������  � CD
extern const TCD_DATA CD_DATA;

//����� ������� � ����������� ������
extern struct TCD_DATA CD_DATA_RC;

#define CD_DATA_PREFIX 0xC
#define CD_DATA_SIZE sizeof(TCD_DATA)

//�������
  #ifdef __cplusplus
    extern "C" void CDataMkRamCpy (void);//�������� ����� ������� � ���
    extern "C" bool CDataChk      (void);//��������� ������� � �� ������. TRUE ���� ���� �������
    extern "C" void CDataSave     (void);//���������� ����� ������� �� CD
    extern "C" bool CDataLoaded   (void);//�������� �� �� ��� ������� ��������� � ���
  #else
    extern void CDataMkRamCpy (void);//�������� ����� ������� � ���
    extern bool CDataChk      (void);//��������� ������� � �� ������. TRUE ���� ���� �������
    extern void CDataSave     (void);//���������� ����� ������� �� CD
    extern bool CDataLoaded   (void);//�������� �� �� ��� ������� ��������� � ���
  #endif


#endif 