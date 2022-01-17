#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include "Intmash_bastypes.h"

//������ ������ ������� ModBus
#define MODBUS_BUFFER_SIZE 16384+8 //16� + 8 ���� ��� ��������� ���� 256

/***������� �������� ��������� � ������ �������********************************/
/*  ����� ���� ������� ��� ������(����� 0x11) */
#define MB_DEVICE_ADDRESS              0       //����� ���������� , ��� ���� ������
#define MB_COMMAND_CODE                1       //��� ������� , ��� ���� ������
#define MB_START_ADDR_HI               2       //����� ������� ,��� ���� ����� (0�11) !������� ����
#define MB_START_ADDR_LO               3       //����� ������� ,��� ���� ����� (0�11) !������� ���� 

/* GetDeviceID  0x11*/
//�����
#define MB_DATA_BYTE_CNT_CMD_11        2      //���������� ���� ������ � ������ 
#define MB_DATA_SECTION_CMD_11         3      //������ ������ ������ � ������   

/* ModbusMemRead Bits*/
//������
#define MB_BITS_NUM_HI_CMD_01          4      //���������� ��������� ��� ������� !������� ���� 
#define MB_BITS_NUM_LO_CMD_01          5      //���������� ��������� ��� ������� !������� ����
//�����
#define MB_DATA_BYTE_NUM_CMD_READ01    2     //���������� ���� ������ � ������ �� ������ �03 
#define MB_DATA_SECTION_CMD_READ01     3     //������ ������ ������ � ������ �� ������ 0x03 

/* ModbusMemWriteBit 0x05*/
//������
#define MB_BIT_STATE                   4      //� ������� ����� �������� ��������� ���� FF ��� 00

/* ModbusMemRead 0x03*/
//������
#define MB_REG_NUM_HI_CMD_03           4      //���������� ��������� ��� ������� !������� ���� 
#define MB_REG_NUM_LO_CMD_03           5      //���������� ��������� ��� ������� !������� ����
//�����
#define MB_DATA_BYTE_NUM_CMD_03        2     //���������� ���� ������ � ������ �� ������ �03 
#define MB_DATA_SECTION_CMD_03         3     //������ ������ ������ � ������ �� ������ 0x03 

/* ModbusMemWrite 0x10*/
//������ � �����
#define MB_REG_NUM_HI_CMD_10           4     //���������� ��������� ��� ������� � ������ !������� ����
#define MB_REG_NUM_LO_CMD_10           5     //���������� ��������� ��� ������� � ������ !������� ���� 
#define MB_DATA_BYTE_NUM_CMD_10        6     //���������� ���� ������ ��� �������
#define MB_DATA_SECTION_CMD_10         7     //������ ������ ������ ��� �������

/* ModbusMemWriteSingle 0x06 */
//������ � �����
#define MB_DATA_SECTION_CMD_06         4     //������ ������ ������ ��� �������

/* ModbusMemWriteMask 0x16*/
//������ � �����
#define MB_REG_AND_MASK_HI             4     //����� � !������� ���� 
#define MB_REG_AND_MASK_LO             5     //����� � !�������� ����
#define MB_REG_OR_MASK_HI              6     //����� ��� !������� ����
#define MB_REG_OR_MASK_LO              7     //����� ��� !�������� ����
#define MASK_ANSWER_SIZE               10    //������ ������ �� ������ ����� 

/* ModbusMemWriteCoil 0x05*/
#define MB_DATA_SECTION_CMD_05         4     //������ ������ ������ ��� �������

/* ModbusMemWriteCoils 0x0F*/
//������ � �����
#define MB_BITS_NUM_HI_CMD_0F          4     //���������� ��� �� ������ ������� ����
#define MB_BITS_NUM_LO_CMD_0F          5     //���������� ��� �� ������ ������� ����
#define MB_DATA_BYTE_NUM_CMD_0F        6     //���������� ���� ������ ��� ������
#define MB_DATA_SECTION_CMD_0F         7     //������ ������ ������ ��� ��������

/* ��������� �� ������ */
#define MB_ERROR_SECTION               2      //����� ���� ������ 

/*** ModbusErrorCodes *********************************************************/
#define ERROR_ANSWER_SIZE              5      //������ ������ ��� ������
// �������� �����
#define MB_ERR_CODE_PREFIX             0x80   //������� � ���������� ������� 
#define MB_ERR_CODE_ILLEGAL_FUNC       1      //�������� ��� ������� �� ����� ���� ��������� 
#define MB_ERR_CODE_ILLEGAL_ADDR       2      //����� ������, ��������� � �������, �� �������� 
#define MB_ERR_CODE_ILLEGAL_DATA       3      //��������, ���������� � ���� ������ ������� �������� �� ����������


/*** ������ ������� ***********************************************************/
#define BROADCAST_MSG_ADDR             0x00
#define MB_IS_BROADCAST_MSG            (Slave->Buffer[MB_DEVICE_ADDRESS] == BROADCAST_MSG_ADDR) 

#define WR_ANSWER_SIZE 8 //������ ������ �� ������ ������ 
#define CRC_SIZE 2 //������ crc-2 �����

/*** ����� �������� ������, ��������� �� Modbus *******************************/
#define MB_DATA_TYPE_COILS             0x10
#define MB_DATA_TYPE_DISCRETE_INPUTS   0x20
#define MB_DATA_TYPE_HOLDING_REGISTERS 0x30
#define MB_DATA_TYPE_INPUT_REGISTERS   0x40
typedef struct ModbusMapType
{
  tU8  MemSpaceCode;//Type + Prefix
  tU16 Size;
  tU8  ModifyFlag;
  tU8* DataPointer;
}ModbusMapType;

/*** ��� ������������ �������� � ���� Modbus*** *******************************/
typedef struct ModbusSlaveType
{
  tU8 SlaveAddress;     //����� �������� ����������
  tU8Union SlaveStatus; //������ �������� ����������
  tU8 Buffer[MODBUS_BUFFER_SIZE]; //����� ������/��������    
  ModbusMapType* ModbusMap; //��������� �� ����� ������
} ModbusSlaveType;



/*** ��� ���������, ����������� ���������� ������� ****************************/
typedef struct ModbusCommandHandlerType
{
  tU16 (*Handler)(ModbusSlaveType* Slave); //��������� �� ����������
  tU8 CommandCode; // ��������������� ����������� �������.
} ModbusCommandHandlerType;




/*** ������� ������ ***********************************************************/
/*Name: ModbusCommandDecode
	������������� ��������� ������� � �� ����������, ���� ��� ������� ��������
Arg:
	ModbusSlaveType* Slave - ��������� �� ��������� ��������
        MesageSize - ������ �������� �������
        ModbusCommandHandlerType* HandlerTable - ��������� �� ��������� ������������ ������
Ret:
  ������ ������������ ������� � ������
Comment:
*/
tU16 ModbusCommandDecode(ModbusSlaveType* Slave, tU16 MesageSize, ModbusCommandHandlerType* HandlerTable);
//������� ����������� ������ � ������������ ������ �� ��������� Modbus
void ModbusSwapCopy(tU8* Src, tU16* Dst, tU16 RegNum);
//��������� ������ �� modbus
tU8 ModbusError(ModbusSlaveType* Slave,tU8 ModbusErrorType);
//������� ������� ������ Modbus
tU8 ModbusReadCoils(ModbusSlaveType* Slave);
tU8 ModbusReadDiscreteInputs(ModbusSlaveType* Slave);
tU8 ModbusReadHoldingRegisters(ModbusSlaveType* Slave);
tU8 ModbusReadInputRegisters(ModbusSlaveType* Slave);
tU8 ModbusWriteSingleCoil(ModbusSlaveType* Slave);
tU8 ModbusWriteSingleRegister(ModbusSlaveType* Slave);
tU8 ModbusWriteMultipleCoils(ModbusSlaveType* Slave);
tU8 ModbusWriteMultipleRegisters(ModbusSlaveType* Slave);
tU8 ModbusReportSlaveID(ModbusSlaveType* Slave);
//��������� ������ �������� �������� �� ����� AND,OR - 0x16
tU8 ModbusMaskWriteRegister(ModbusSlaveType* Slave);

#endif




