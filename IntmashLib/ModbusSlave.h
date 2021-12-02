#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include "Intmash_bastypes.h"

//размер буфера посылки ModBus
#define MODBUS_BUFFER_SIZE 16384+8 //16К + 8 байт под заголовки было 256

/***Индексы разметки протокола в буфере посылки********************************/
/*  общий блок запроса для команд(кроме 0x11) */
#define MB_DEVICE_ADDRESS              0       //адрес устройства , для всех команд
#define MB_COMMAND_CODE                1       //код команды , для всех команд
#define MB_START_ADDR_HI               2       //адрес первого ,для всех кроме (0х11) !старший байт
#define MB_START_ADDR_LO               3       //адрес первого ,для всех кроме (0х11) !младший байт 

/* GetDeviceID  0x11*/
//ответ
#define MB_DATA_BYTE_CNT_CMD_11        2      //количество байт данных в ответе 
#define MB_DATA_SECTION_CMD_11         3      //начало секции данных в ответе   

/* ModbusMemRead Bits*/
//запрос
#define MB_BITS_NUM_HI_CMD_01          4      //количество регистров для запроса !старший байт 
#define MB_BITS_NUM_LO_CMD_01          5      //количества регистров для запроса !младший байт
//ответ
#define MB_DATA_BYTE_NUM_CMD_READ01    2     //количество байт данных в ответе на запрос х03 
#define MB_DATA_SECTION_CMD_READ01     3     //начало секции данных в ответе на запрос 0x03 

/* ModbusMemWriteBit 0x05*/
//запрос
#define MB_BIT_STATE                   4      //в старшем байте хранится состояние бита FF или 00

/* ModbusMemRead 0x03*/
//запрос
#define MB_REG_NUM_HI_CMD_03           4      //количество регистров для запроса !старший байт 
#define MB_REG_NUM_LO_CMD_03           5      //количества регистров для запроса !младший байт
//ответ
#define MB_DATA_BYTE_NUM_CMD_03        2     //количество байт данных в ответе на запрос х03 
#define MB_DATA_SECTION_CMD_03         3     //начало секции данных в ответе на запрос 0x03 

/* ModbusMemWrite 0x10*/
//запрос и ответ
#define MB_REG_NUM_HI_CMD_10           4     //количество регистров для запроса и ответа !старший байт
#define MB_REG_NUM_LO_CMD_10           5     //количество регистров для запроса и ответа !младший байт 
#define MB_DATA_BYTE_NUM_CMD_10        6     //количество байт данных для запроса
#define MB_DATA_SECTION_CMD_10         7     //начало секции данных для запроса

/* ModbusMemWriteSingle 0x06 */
//запрос и ответ
#define MB_DATA_SECTION_CMD_06         4     //начало секции данных для запроса

/* ModbusMemWriteMask 0x16*/
//запрос и ответ
#define MB_REG_AND_MASK_HI             4     //маска И !старший байт 
#define MB_REG_AND_MASK_LO             5     //маска И !младшиий байт
#define MB_REG_OR_MASK_HI              6     //маска ИЛИ !старший байт
#define MB_REG_OR_MASK_LO              7     //маска ИЛИ !младшиий байт
#define MASK_ANSWER_SIZE               10    //размер ответа на запрос маски 

/* ModbusMemWriteCoil 0x05*/
#define MB_DATA_SECTION_CMD_05         4     //начало секции данных для запроса

/* ModbusMemWriteCoils 0x0F*/
//запрос и ответ
#define MB_BITS_NUM_HI_CMD_0F          4     //количество бит на запись старший байт
#define MB_BITS_NUM_LO_CMD_0F          5     //количество бит на запись младший байт
#define MB_DATA_BYTE_NUM_CMD_0F        6     //количество байт данных для записи
#define MB_DATA_SECTION_CMD_0F         7     //начало секции данных для запросаа

/* Сообщение об ошибке */
#define MB_ERROR_SECTION               2      //адрес кода ошибки 

/*** ModbusErrorCodes *********************************************************/
#define ERROR_ANSWER_SIZE              5      //размер ответа при ошибке
// значения кодов
#define MB_ERR_CODE_PREFIX             0x80   //добавка к ошибочному запросу 
#define MB_ERR_CODE_ILLEGAL_FUNC       1      //Принятый код функции не может быть обработан 
#define MB_ERR_CODE_ILLEGAL_ADDR       2      //Адрес данных, указанный в запросе, не доступен 
#define MB_ERR_CODE_ILLEGAL_DATA       3      //Величина, помещенная в поле данных запроса является не допустимой


/*** Прочие дефайны ***********************************************************/
#define BROADCAST_MSG_ADDR             0x00
#define MB_IS_BROADCAST_MSG            (Slave->Buffer[MB_DEVICE_ADDRESS] == BROADCAST_MSG_ADDR) 

#define WR_ANSWER_SIZE 8 //размер ответа на запрос записи 
#define CRC_SIZE 2 //размер crc-2 байта

/*** Карта областей памяти, доступной по Modbus *******************************/
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

/*** Тип конфигурации ведомого в сети Modbus*** *******************************/
typedef struct ModbusSlaveType
{
  tU8 SlaveAddress;     //адрес ведомого устройства
  tU8Union SlaveStatus; //статус ведомого устройства
  tU8 Buffer[MODBUS_BUFFER_SIZE]; //буфер приема/передачи    
  ModbusMapType* ModbusMap; //указатель на карту памяти
} ModbusSlaveType;



/*** Тип структуры, описывающий обработчик команды ****************************/
typedef struct ModbusCommandHandlerType
{
  tU16 (*Handler)(ModbusSlaveType* Slave); //указатель на обработчик
  tU8 CommandCode; // соответствующая обработчику команда.
} ModbusCommandHandlerType;




/*** Функции модуля ***********************************************************/
/*Name: ModbusCommandDecode
	декодирование пришедшей команды и ее выполнение, если эта команда известна
Arg:
	ModbusSlaveType* Slave - указатель на структуру ведомого
        MesageSize - размер принятой посылки
        ModbusCommandHandlerType* HandlerTable - указатель на структуру обработчиков команд
Ret:
  размер отправляемой посылки в байтах
Comment:
*/
tU16 ModbusCommandDecode(ModbusSlaveType* Slave, tU8 MesageSize, ModbusCommandHandlerType* HandlerTable);
//функции копирования данных с расстановкой данных по протоколу Modbus
void ModbusSwapCopy(tU8* Src, tU16* Dst, tU16 RegNum);
//обработка ошибок по modbus
tU8 ModbusError(ModbusSlaveType* Slave,tU8 ModbusErrorType);
//функции базовых команд Modbus
tU8 ModbusReadCoils(ModbusSlaveType* Slave);
tU8 ModbusReadDiscreteInputs(ModbusSlaveType* Slave);
tU8 ModbusReadHoldingRegisters(ModbusSlaveType* Slave);
tU8 ModbusReadInputRegisters(ModbusSlaveType* Slave);
tU8 ModbusWriteSingleCoil(ModbusSlaveType* Slave);
tU8 ModbusWriteSingleRegister(ModbusSlaveType* Slave);
tU8 ModbusWriteMultipleCoils(ModbusSlaveType* Slave);
tU8 ModbusWriteMultipleRegisters(ModbusSlaveType* Slave);
tU8 ModbusReportSlaveID(ModbusSlaveType* Slave);
//изменение одного регистра хранения по маске AND,OR - 0x16
tU8 ModbusMaskWriteRegister(ModbusSlaveType* Slave);

#endif




