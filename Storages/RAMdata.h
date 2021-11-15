#ifndef RAMDATA_H
#define RAMDATA_H

#include "bastypes.h"
//����� ���������� � ���������;
typedef 
  union   {
    u32	   i;
    __packed struct {//����� ������
      //r0000.L
      unsigned DExS_PWR_LNK     : 1;//������� ����� � ������ DExS.PWR2 (��� �������� ����������� � ������������� ���������)
      unsigned DExS_PWR_Ready   : 1;//���������� ����� DExS.PWR (� �.�. ���������� � �������)
      unsigned b2               : 1;//
      unsigned ExtRegTimeOut    : 1;//1-������������ �������� ��������� (��������)
      unsigned R_INSL_LOW       : 1;//����������������� ����� ��������
      unsigned PWR1             : 1;//��������/��������� �������� ���-� ������������
      unsigned Blank            : 1;//������� ���� ���������������
      unsigned CLM              : 1;//Current Limit Mode - ����� ����������� ����
      //r0000.H
      unsigned imp_block        : 1;//���������� ��������� ��� �������� ������������ �����    
      unsigned IExcRef_min      : 1;//������� ���� ����������� ���������� �������� ������������ �������              
      unsigned IExcRef_max      : 1;//������� ���� ����������� ���������� �������� ������������� �������
      unsigned b11              : 1;//
      unsigned b12              : 1;//
      unsigned b13              : 1;//
      unsigned UstLow           : 1;//�������� ���������� ����
      unsigned UstFail          : 1;//���������� ���������� ����
      //r0001.L
      unsigned ForceByUst       : 1;//���������� ��� �������� ���������� �������
      unsigned cfi_enable       : 1;//���� ����������� �������� ���� ������� (������)
      unsigned cfu_enable       : 1;//���� ����������� �������� ���������� ������� (������ + ������� ������ ������� "����������")
      unsigned i2tR             : 1;// 1-i2t ������ � �������� ������
      unsigned TestMode         : 1;// ����� �����������
      unsigned Tougle           : 1;//
      unsigned Up               : 1;//
      unsigned UACC_OK          : 1;//������� ���������� �� ��� ���������� ����������� - ���������� ���	  
      //r0001.H
      unsigned UEWG_OK          : 1;//������� ���������� �� ������� ����������� ���������� - ���������� ���
      unsigned UTSNG_OK         : 1;//������� ���������� �� �������������� ����������� ���� ���������� - ���������� ���
      unsigned BackPower        : 1;//��������� ��������
      unsigned QminLimIrUp      : 1;//���������� ����������� ���������� ��������, ��������� �������� ��� �����������
      unsigned QmaxLimIrDn      : 1;//���������� ������������ ���������� ��������, ��������� ������� ��� �����������
      unsigned T1000Hz          : 1;//������������ ��� ��������� 0.01 ��� �������� ������� ��������� ���������� ��������� ���������
      unsigned FS_OK            : 1;//��������� �������� �������
      unsigned FLASH_WRITE      : 1;// 1-���� ������������� (������������ ����� ���������� ������� ������ �����)      
    } bits ;
} baFLAGS0;//����� ���������� � ���������;

///////////////////////////////////////////////////////////////////////////////
typedef 
  union   {
    u32	   i;
    __packed struct {//����� ������
      //r0002.L
      unsigned Auto    : 1;//�������� ��������� ���� ��� ������ �����
      unsigned RotatingDiodeNotConducted : 1;//������������� ������������ ����� - �� ������������
      unsigned RotatingDiodeShortCircuit : 1;//������������� ������������ ����� - ����������������
      unsigned b3      : 1;//
      unsigned b4      : 1;//
      unsigned b5      : 1;//
      unsigned b6      : 1;// 
      unsigned b7      : 1;// 
      //r0002.H
      unsigned b8      : 1;//
      unsigned b9      : 1;// 
      unsigned b10     : 1;// 
      unsigned b11     : 1;//
      unsigned b12     : 1;//
      unsigned LOG_STAT       : 1;//��������� ����������� 
      unsigned LOG_UNMOUNT    : 1;//���� ��� ��������������� �������
      unsigned LOG_ERROR      : 1;//������ ���� - ����������� �������� �����/ ��� ����� �����? �� ���� ����� �� ������ ���� SD_ERROR ����� ��������� ������
      //r0003.L
      unsigned FAULT        : 1;//����� ������� ������ = (OVC | OVL) ��������� �������� ������� ������ Start, PWR � �.�.          
      unsigned RotatingDiodeFlt : 1;//������������� ������������ �����
      unsigned GlobalError  : 1;//������ ����������� ������� �����������
      unsigned FieldFail    : 1;//���������� �����������
      unsigned UstMaxFlt    : 1;//���������� ������������ ���������� ���������� �������       
      unsigned IttMaxFlt    : 1;//������ �� ���� ����������� (�� ��)      
      unsigned IexcMaxFlt   : 1;//������ ���������� ���� ��������
      unsigned FltMPS       : 1;// ������ ���������� �������� ����������� �� ����� ������ (���� ���������)
      //r0003.H
      unsigned PowerBlockFlt   : 1;//������������� ������� �����       
      unsigned FLongForce   : 1;//������ "���������� ������� ����������"      
      unsigned FltCCSB      : 1;//������ �� ������� ���������� ���������� ������� ����������� ����� �� ���
      unsigned FreqMinFlt   : 1;//������ �� ����������� ������� ���������� ���������� �� �������� ����
      unsigned QminAsyncRun : 1;//����������� ��� �� ����������� ���������� ��������       
      unsigned NotUpVoltage : 1;//�� ���������� ������� ���������� ��� ����� ����������      
      unsigned R_INSL_FLT   : 1;//��������� ����� ��������
      unsigned IstOV        : 1;//�������� ������� 
    } bits ;
} baFLAGS1;//����� ���������� � ���������;
///////////////////////////////////////////////////////////////////////////////

typedef 
  union bits8{
   u8	   i;
   __packed struct {//����� 
      unsigned flash_error : 1;	  //
      unsigned backup_error : 1;  //
      unsigned fram_error : 1;	  //
      unsigned fram_bkp_error : 1;//
      unsigned sram_error : 1;	  //
      //unsigned SD_init_error : 1; //��������� ������������� SD-�����
      unsigned ext_master_lnk : 1;//����� � ������� �������� �� ������� FA (0-����, 1-���)
    } BA;
} bits8;//����� ���������� � ���������;

/////////////////////////////////////////////////////////////////////////////// 

//��������� ������������ � RAM
typedef struct TRAM_DATA {
    ////////////////////////////////////////////////////////////////////////////
    baFLAGS0 FLAGS0;//0-1 32bit ����� ���������� � ���������; 
    baFLAGS1 FLAGS1;//2-3 32bit ����� ���������� � ���������; 
    //����� �����/������
    u16 DI;   //04 
    u16 DO;   //05
    //////////////
    //������ � ����� DExS.M4CPU, �������������
    u16 uUstat; //���������� �������    � ��������� 0.1
    u16 uIstat; //��� �������           � ��������� 0.1
    u16 __uIsmax;  //��� ������� �����
    s16 sUexc;  //���������� ������     � ��������� 0.1
    u16 uIexc;  //��� ������            � ��������� 0.01
    u16 IttA;   //38
    u16 IttB;   //3A
    u16 IttC;   //3C
    //������ � ����� DExS.CPU.VTEG, �������������, ������� ��������
    u16 uUpwr_ab;//����. ���� (� ��������� 0.1)
    u16 uUpwr_bc;//����. ���� (� ��������� 0.1)
    u16 uUpwr_ca;//����. ���� (� ��������� 0.1)
    u16 uIpwr;   //��� ���� (������)  (� ��������� 0.1)
    u16 uUsg_ab; //����. ���������� (� ��������� 0.1)
    u16 uUsg_bc; //����. ���������� (� ��������� 0.1)
    u16 uUsg_ca; //����. ���������� (� ��������� 0.1)
    ///////////////////////////////////////////////////////////////////////////
    u16  Freq;   //������� ���� (� ��������� �� 0,01)
    f32  Phi;     //������� �������� ���� ������ ���� ����� ����������� � ����� ���������� ��s� (� ����)
    bauint  SYSTEM_STATE;//��������� ����������
                         //DEX_STATE
                         //REG_MODE ����� ������������� 1-��� 3-�� 2-��
    u16  UstStC; //����������� �������� ��� �������� (��� ��������)
    f32  QoeByP; //���������� �������� � ������������� �������� ������������ ������� �������� ��������
    f32  __dQref;  
    //���������������� �����
    f32  EReg;    //���������� ���������� ���� ����������� ����������
    f32  UsgRef;    //�������� �������� ���������� ����������
    f32  StRef;     //�������� �������
    f32  ARVRef;//
    f32  Ilim_max;//����������� ���� ����������� �� ���������
    f32  Ilim_min;//����������� ���� ����������� �� ��������� 
    u16  RegCMD;  //������� ����������
    u16 roIexcHistory;//��������� ���� 1 ��� ����� ��� �������� � ������ ����� ��� ������������� ��������������� ������
    ///////////////////////////////////////////////////////////////////////////
    f32  Qpwm;//������ �������� ����������
    //������ � ����� DExS.PWR2
    u16 ___ReqMode__;//
    u16 RINSL;//������������� ��������
    u16 __T_sync;//������ ����
    u16 UEWG;//���������� ������� ����������� ����������
    u16 UTSNG;//���������� ������� ������� ����� �� �������������� ����������� ���� ������� ����������
    u16 UACC;//���������� ��� =24� ���������� �����������
    //��������� �������� ������ � �������� ����������
    f32 Ssg;     //������ ��������
    f32 Psg;     //�������� ��������
    f32 Qsg;     //���������� �������� ���������� 
    ////////////////////////////////////////////////////////////////////////////
    u16  __IzBias; //�������� ���������� ���� � �������������� ����������� �������� 0.001 (��� ��� DEX_STATE)
    s16 QLmin;//������������ ����������� ���������� ��������
    s16 QLmax;//������������ ������������ ���������� ��������
    u16 __UstStC; //����������� �������� ��� �������� (��� ��������)
    ////////////////////////////////////////////////////////////////////////////
    f32 QoeByPnom;
    //������ � ����� DExS.CPU.VTEG, �������������, ������� ��������
    f32 Osc[4];
    f32 Usg;// ������� ����������� ����������
    f32 __Phi;// ����� ����
    f32 CosPhi;// ������� ������ ����
    f32 Isg;   //��� ���� (������)
    f32 UabSg; //����. ����������
    f32 UbcSg; //����. ����������
    f32 UcaSg; //����. ����������
    //f32 UabPwr;//����. ����
    //f32 UbcPwr;//����. ����
    //f32 UcaPwr;//����. ����
    
    f32 i2tRSum;//r0014 ����������� ���-�� ����� ������
    //������������ �� ���������� �������� (���������� ��� ������ �������� ��������)
    u16 ___Free_r77; //����������� �������� ��� �������� (��� ��������)
    f32 ___Free;//���������� � ���������� �������� � ���������� �� ��������
    ///////////////////////////////////////////////////////////////////////////
    //u16 ext_ref; //������� ���������� ����� DExS.CPU.VTEG      
    u16 tmp1;//E
    u16 tmp2;//F
    ////////////////////////// 
    u32 Ei2tS;   //���������� "�����" � ������ �������
    //////////////////////////
    u16 ExtRegTimeOutTimer;//������� ���������� �� ������ ��������������� ����������
    /////////////////////////////////////////////////////// 
    f32 Iexc;//��� ������
    bits8 FLAGS;  //r0031
    u16 Last_lnk_error;//08 
    volatile u8  TimeBuf[8];//04-07
    u16 RCMD_Rst_Timer;//������� ������� �������� � ��������� "�������" ��� 
                       //���������� ����� � ��������
    u16 ___gap___;
} TRAM_DATA;

#define DEX_STATE       RAM_DATA.SYSTEM_STATE.b[0]
#define REG_MODE        RAM_DATA.SYSTEM_STATE.b[1]
//��������� ������������ RAM
extern struct TRAM_DATA RAM_DATA;

extern u16 FLASH_change;

//����������� ��������� ��������� ���������
#define RAM_DATA_PREFIX 0x0
#define RAM_DATA_SIZE 0xf0

#endif