
#include "ModbusASCII.h"
#include "crc16.h"

/*MBA = Modbus ASCII ----------------------- Defines ------------------------------------------*/
#define MBA_END_CR     '\r'     // CR ������ ��� Modbus ASCII
#define MBA_END_LF     '\n'     //  LF ������ ��� Modbus ASCII
#define MBA_BEGIN      ':'      //  : ��������� ������ ��� Modbus ASCII
#define MBA_LRC_SIZE    1       //������ LRC 



/*
������� �������� ���� ascii  � ������ 
tU8 DataChar - ��� ascii, ������� ����� ��������� � ������ 
ret: ������
����� ���������� 0
*/
tU8 mbaAsciiToData(tU8 DataChar)
{
  //�������� ������� �� ������������ �������, ���� ������ �� 0 �� 9
  if((DataChar >= '0') && (DataChar <= '9'))return (tU8)(DataChar - '0');
  //��� �� A �� F - ���������� ������
  else if((DataChar >= 'A' ) && (DataChar <= 'F'))return (tU8)(DataChar - 'A' + 0x0A);
  //����� �������� ������������� ������
  else return 0xFF;
}

/*
������� �������� ������� � ��� ascii
tU8 DataByte - ������, ������� ����� ��������� � ������ ascii
ret: ����-��� ������������ �������, ���� �� ������������� �������� ������� mba
����� ���������� 0xff
*/
tU8 mbaDataToAscii(tU8 DataByte)
{
  //�������� ����� �� ������������ �������, ���� ����� �� 0 �� 9
  if(DataByte <= 0x09) return (tU8)('0' + DataByte);
  //��� �� A �� F - ���������� ���
  else if((DataByte >= 0x0A) && (DataByte <= 0x0F)) return (tU8)(DataByte - 0x0A + 'A');
  //����� �������� ������������� ������
  else return '0';
}

/*
������� �������� LRC ��� mba
tU8 *pucFrame - ��������� �� ������ ������ ������
ret: LRC8

note: ���� ����� ������� ��� ����� ������ ������ � ���� LRC �� ������� � ���������� 0. 
��� c���� ������� �������� ������������ ������ ������.
*/
tU8 mbaLRC(tU8 *pucFrame, tU8 MesageSize)
{
  tU8 LRC8 = 0; 
  //������� ������ ��� ����� � ��������� Modbus ASCII, �� ����, ��� ��� ��������������� � � ������� ASCII. 
  //�� ���������� � ���������� ��������� ������ ��������� � ����������� ������� CR/LF.
  //�������� ��� ���� ������ 8 (�.�. �������� ������� ����)
  while(MesageSize--){ LRC8 += *pucFrame++;}
  //������� �������������� ���� ������������� ����� �������� LRC ����
  LRC8 = (tU8)(-((tS8)LRC8));
  return LRC8;
}


/*
������� ��������� ���������� ��������� ���������, � ���� ��� ��������� - ��������� ����� 

*/
tU8 MBAsciiRx(tU8* InBuffer, tU8 MesageSize, ModbusAsciiType* MBAclient)
{
  tU8* BufAsciiToChar =&MBAclient->BufferAscii[0];
  tU8* DataIn=&MBAclient->DataAsciiCnt; //��������� ��������� ����� ���������� �������� ��� ��������� ����������
   
  MBAclient->StopTimer(); 
  if (InBuffer[0] == MBA_BEGIN) //���� ������ ������ ���������, ���������� �� ������� ������
  {
    if ((InBuffer[MesageSize-1] == MBA_END_LF)&(InBuffer[MesageSize-2] == MBA_END_CR))//� ��������� ������ ����������
    {
      //��������� ���������� ������ - ����� ������ CR_LF     
      *DataIn =0; //��������
      MBAclient->Status = STATE_RX_RCV;
      //������������ � ������������� ����� � ���������������
      tU8 ByteCnt=0; //������� ���� ��������� ���������
      for(tU8 i=1; i<(MesageSize-2);){
        BufAsciiToChar[ByteCnt]= (mbaAsciiToData(InBuffer[i])<<4) + mbaAsciiToData(InBuffer[i+1]);
        i+=2;
        ByteCnt++;
      }
      for(tU8 i=0; i<(ByteCnt); i++){InBuffer[i] = BufAsciiToChar[i] ;} //��������          
      //��������� ����������� �����
      if(mbaLRC(&InBuffer[0], ByteCnt) == 0){//����� �������
        FrameEndCrc16(&InBuffer[0], (ByteCnt+1)); //�������� LRC �� CRC, ����� ������ ������ � ����
        return (ByteCnt+1);     //���������� ������ ������� � ������ � crc              
      }      
    }
    else 
    {
      MBAclient->Status = STATE_RX_WAIT_EOF; //��������� ��������� ��� - �������� ������ � ���� ������  
      MBAclient->SetTimer(ASKII_TIME_PAUSE); //������ ������� �� ������� ������
      //�������� ������ � ������ �� ����������
      for(tU8 i=0; i<MesageSize;i++ ){BufAsciiToChar[i] = InBuffer[i];} //��������� ������
      *DataIn = MesageSize; //��������� ����������
    }
  }
  else if (MBAclient->Status == STATE_RX_WAIT_EOF) //���� ��� ������ �� ������, �� ��������� ������ - ���� �� �� ��������� ������� ���������, ���� ����
  {
    if ((*DataIn + MesageSize)>=MODBUS_BUFFER_SIZE){//���� � ��� ������ ������ ������, ��� ������ ���� � �������
      *DataIn =0;
      MBAclient->Status = STATE_RX_RCV; 
    }
    else{ //������ �� ����� �� ������������ ������ ������
      //�������� ������ � ������ �� ����������
      for(tU8 i=0; i<(*DataIn +MesageSize);i++ ){BufAsciiToChar[*DataIn+i] = InBuffer[i];} //��������� ������
      *DataIn += MesageSize; //��������� ����������
      if ((BufAsciiToChar[*DataIn-1] == MBA_END_LF)&(BufAsciiToChar[*DataIn-2] == MBA_END_CR))//� ��������� ������ ����������
      {
        MBAclient->Status = STATE_RX_RCV; //������� ������
        //������������ ������� � ���������������
        tU8 ByteCnt=0; //������� ���� ��������� ���������
        for(tU8 i=1; i<(*DataIn-2);){
          InBuffer[ByteCnt]= (mbaAsciiToData(BufAsciiToChar[i])<<4) + mbaAsciiToData(BufAsciiToChar[i+1]);
          i+=2;
          ByteCnt++;
        }        
        *DataIn =0; //��������
        //��������� ����������� �����
        if(mbaLRC(&InBuffer[0], ByteCnt) == 0){//����� �������
          FrameEndCrc16(&InBuffer[0], (ByteCnt+1)); //�������� LRC �� CRC, ����� ������ ������ � ����
          return (ByteCnt+1);     //���������� ������ ������� � ������ � crc              
        }         
      }
      else MBAclient->SetTimer(ASKII_TIME_PAUSE); //������ ������� �� ������� ������     
    }  
  }
  return 0; //���� ������ ��� ������ ����� ��� �����
  //���� ������ - �� ���� �������� - ��� ������ ������
  //���� ����� ��� ����� - ���� �����
}


/*
������� ��������� ����� �������� � ������� ����
*/
tU8 MBAsciiTx(tU8* OutBuffer, tU8 MesageSize)
{
  tU8 BufferCharToAscii[MODBUS_BUFFER_SIZE];
  tU8 CharCnt=1;
  
  //����� � ��� �������� � CRC, �� ��� ���� ������ CRC � �������� LRC
  OutBuffer[MesageSize-2] = mbaLRC(&OutBuffer[0], (MesageSize-2));
  //���������� ������ � ������������� �����
  for(tU8 i=0;i<(MesageSize-1);i++){BufferCharToAscii[i] = OutBuffer[i];}
  
  //�������� �������� ����� ��� �� �������� ����  
  OutBuffer[0] = MBA_BEGIN; //������� ����� ������
  for(tU8 i=0; i<(MesageSize-1); i++){ //��� ��� �� ����
    OutBuffer[CharCnt] = mbaDataToAscii(BufferCharToAscii[i]>>4); CharCnt ++;
    OutBuffer[CharCnt] = mbaDataToAscii(BufferCharToAscii[i]&0x0f); CharCnt ++;    
  }
  OutBuffer[CharCnt] = MBA_END_CR;CharCnt ++;
  OutBuffer[CharCnt] = MBA_END_LF;CharCnt ++;
  
  return CharCnt;
}

/*
MBAsciiSlaveProc - ������� ��� ������
������� ���������� �� ���� �������� ��������� ���������, ��������� ��� ����������, ���� ��������� ����� - �������� �������
��������� ������. ��� ������������� ������, ����� ���������� � ���� ������

  ModbusSlaveType* Slave - ��������� �� ������� �����
  ModbusAsciiType* MBASlave - ��������� �� ��������� ������ ������ ���� ��� ����� ������
  tU8 MesageSize - ������ ��������� �� ����� ���������
  ModbusCommandHandlerType* HandlerTable - ��������� �� ������ ������

  ret: ������ ������� ��� �������� � ������
*/
tU8 MBAsciiSlaveProc(ModbusSlaveType* Slave, ModbusAsciiType* MBASlave, tU8 MesageSize, ModbusCommandHandlerType* HandlerTable)
{
  tU8 MsgSize = MBAsciiRx(&Slave->Buffer[0], MesageSize, MBASlave); //��������� � ���������� ��������� ������
  if(MsgSize){   //���� ��� ������� ������ � ����� ������ ��������� - ���� ������������ �������
    MsgSize = ModbusCommandDecode(Slave, MsgSize, HandlerTable); //�������� ������� �������������, ���������� ������ ������ ��� �������� 
    if(MsgSize){ //���� ��������� ����� - �������� ����� � ������ ����
      MsgSize = MBAsciiTx(&Slave->Buffer[0], MsgSize);   
      MBASlave->Status = STATE_RX_RCV; //����� �������� ������ - ����� ������ ���������
      return MsgSize;
    }
  }
  return 0;
}

/*
MBAsciiMasterProc - ������� ��� �������
������� ���������� �� ���� �������� ��������� ���������, ��������� ��� ����������, ���� ��������� ����� - �������� �������
��������� ������. ��� ������������� ������, ����� ���������� � ���� ������

  MBmasterType* Master - ��������� �� ������� �����
  ModbusAsciiType* MBAclient - ��������� �� ��������� ������ ������ ���� ��� ����� �������
  tU8 MesageSize - ������ ��������� �� ����� ���������

  ret: ������ ������� ��� �������� � ������
*/
tU8 MBAsciiMasterProc(MBmasterType* Master, tU8 MesageSize, ModbusAsciiType* MBAclient)
{ 
  tU8 MsgSize = MBAsciiRx(&Master->InBuf[0], MesageSize, MBAclient); //��������� � ���������� ��������� ������    
  if (MsgSize) MBAclient->Status = STATE_RX_RCV;
  else if (MBAclient->Status == STATE_RX_WAIT_EOF)
  {
    Master->Status =RECIEVE;
  }
  return MsgSize;
}

//������� �������� �� �������
void TimeOutCheck(tU8* Status)
{
  //���� �� ������ ������� ������, �� ����� ����� - ����������� �� ����� ������ ���������
  if (*Status == STATE_RX_WAIT_EOF) *Status = STATE_RX_RCV;
  
}

/*******************************************************************************
������� ����������� ������ ��� ������������� ����
*******************************************************************************/
void ModbusCopy(tU8* Src, tU8* Dst, tU16 ByteNum)
{
  while (ByteNum) {
    ByteNum--;
    *Dst++ = *Src++;
  }
}


