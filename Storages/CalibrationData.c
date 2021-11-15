#include "CalibrationData.h"//
#include "bastypes.h"//�������� �������� �������� ������
#include "flashdata.h"//�������� �������� �������� ������
#include "ramdata.h"

//#include "bastypes.h"//�������� �������� �������� ������
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
    //���������
    0x03E9,//tS16 pssK1F;//9
    0x0FA1,//tS16 pssK0F;//10
    0x0001,//tS16 pssK1IF;//11    
    //������ � ����� DExS.M4CPU, �������� ����
    0,  // Oadc_Uload;
    0,  // Oadc_Iload;
    0,  // Oadc_In1;
    0,  // Oadc_In2;
    0,  // Oadc_In3;
    //������ � ����� DExS.CPU.VTEG, �������� ���� 
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
    //������ � ����� DExS.M4CPU, ����������� ��������������
    1.0, //  Kadc_Uload;
    1.0, //  Kadc_Iload;
    1.0, //  Kadc_In1;
    1.0, //  Kadc_In2;
    1.0, //  Kadc_In3;
    //������ � ����� DExS.CPU.VTEG, ����������� ��������������
    1.0, //  Kadc_Upwr_ab;
    1.0, //  Kadc_Upwr_bc;
    1.0, //  Kadc_Upwr_ca;
    1.0, //  Kadc_Ipwr;
    1.0, //  Kadc_Usg_ab;
    1.0, //  Kadc_Usg_bc;
    1.0, //  Kadc_Usg_ca;
    //������������ ��� ����� DExS.PWR
    95.69378,    //S_DExS_PWR_R;//����� ��� ������������� ��������
    0.361689815, //S_DExS_PWR_F;//����� ��� ������� 0,361689815
    0.09191,     //S_DExS_PWR_U;//����� ��� ����������
    //���������� ���������� ��������
    0x00C8, //fUstat;
    0x00C8, //fIstat;
    2048, //TfUload;
    1024, //TfIload;
    200, //TfItt;//�������� ���� �� ����� �����������
    200, //TUfpwr;
    200, //TfIpwr;
    200, //TfUsg;
    ////////////////////////////////
    0, //CFI_OFFSET;//�������� ���� ��
    0x00C8, //fFi ��������� ������� ������� ���� ��
    0x00C8, //fFreq ��������� ������� ������� ������� ���������� �������
    ////////////////////////////////
    0x0401, //Modbus_option; 
    0x0401, //Modbus_USB;   
    0x0401, //Modbus_RS485;
    0x0001, //IsensorLocation
    //����������� ��� ����� DExS.PWR
    0.17,   //float S_DExS_PWR_UTSNG;//����� ��� ���������� �������������� ����������� ���� ����������
    0.05,    //float S_DExS_PWR_UACC; //����� ��� ���������� ���
    //������ �� ������������� ������������ �����
    10000.0f, //������� ������������� � ��;
    400.0f,   //������� ������� ��������� � ��;
    80.0f,    //������ ������� ��� ������ ��������� � ��;   
    2.5f,     //����������� ���������, ��� ���������� �������� ����������� ��������������, � %;
    3.0f,     //�������� ������������ ��������������, � ��������;
    20.0f,    //����������� ���������, ��� ���������� �������� ����������� ������, � %;
    0.5f,     //�������� ������������ ������, � ��������.
    //
    0
};

//����� ������� � ����������� ������
struct TCD_DATA CD_DATA_RC;
////////////////////////////////////////////////////////////////////////////////
static bool bCDataLoaded = FALSE;//
////////////////////////////////////////////////////////////////////////////////
void CDataMkRamCpy (void);//�������� ����� ������� � ���
bool ChDataChk      (void);//��������� ������� � �� ������. TRUE ���� ���� �������
void CDataSave     (void);//���������� ����� ������� � CD
bool CDataLoaded   (void);//�������� �� �� ��� ������� ��������� � ���
////////////////////////////////////////////////////////////////////////////////
//�������� ����� ������� � ���
void CDataMkRamCpy (void){
  u8_mem_cpy( (u8*)&CD_DATA, (u8*)&CD_DATA_RC, sizeof (TCD_DATA));
  bCDataLoaded = TRUE;//������� ��� �������� ������� � ���
}

//��������� ������� � �� ������. TRUE ���� ���� �������
bool CDataChk (void){
  return ChkBuffersByWord((u32)&CD_DATA,
                             (u32)&CD_DATA_RC,
                                sizeof (TCD_DATA) / 2);
  //TRUE  - ����� �����������
  //FALSE - ����� ���������
}

//���������� ����� ���������� �� flash
void CDataSave     (void){
  if (CDataChk()){//���� ������ ���������� 
    SaveCD();   //�� ��������� ��
  }
}

//�������� �� �� ��� ������� ��������� � ���
bool CDataLoaded   (void){
  return bCDataLoaded;
}