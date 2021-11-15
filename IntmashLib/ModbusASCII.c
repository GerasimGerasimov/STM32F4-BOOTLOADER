
#include "ModbusASCII.h"
#include "crc16.h"

/*MBA = Modbus ASCII ----------------------- Defines ------------------------------------------*/
#define MBA_END_CR     '\r'     // CR символ для Modbus ASCII
#define MBA_END_LF     '\n'     //  LF символ для Modbus ASCII
#define MBA_BEGIN      ':'      //  : начальный символ для Modbus ASCII
#define MBA_LRC_SIZE    1       //размер LRC 



/*
Функция перевода кода ascii  в символ 
tU8 DataChar - код ascii, который нужно перевести в символ 
ret: символ
иначе возвращает 0
*/
tU8 mbaAsciiToData(tU8 DataChar)
{
  //проверка символа на соответствие формату, если символ от 0 до 9
  if((DataChar >= '0') && (DataChar <= '9'))return (tU8)(DataChar - '0');
  //или от A до F - возвращаем символ
  else if((DataChar >= 'A' ) && (DataChar <= 'F'))return (tU8)(DataChar - 'A' + 0x0A);
  //иначе прислали неправомерный символ
  else return 0xFF;
}

/*
Функция перевода символа в код ascii
tU8 DataByte - символ, который нужно перевести в формат ascii
ret: аски-код принимаемого символа, если он соответствует условиям формата mba
иначе возвращает 0xff
*/
tU8 mbaDataToAscii(tU8 DataByte)
{
  //проверка числа на соответствие формату, если число от 0 до 9
  if(DataByte <= 0x09) return (tU8)('0' + DataByte);
  //или от A до F - возвращаем код
  else if((DataByte >= 0x0A) && (DataByte <= 0x0F)) return (tU8)(DataByte - 0x0A + 'A');
  //иначе прислали неправомерный символ
  else return '0';
}

/*
Функция рассчета LRC для mba
tU8 *pucFrame - указатель на начало буфера данных
ret: LRC8

note: если затем сложить все байты пакета данных и байт LRC мы получим в результате 0. 
Это cамая быстрая проверка корректности пакета данных.
*/
tU8 mbaLRC(tU8 *pucFrame, tU8 MesageSize)
{
  tU8 LRC8 = 0; 
  //Сложить вместе все байты в сообщении Modbus ASCII, до того, как они сконвертированы в в символы ASCII. 
  //Не включаются в вычисления стартовый символ двоеточия и завершающие символы CR/LF.
  //Обнулить все биты больше 8 (т.е. оставить младший байт)
  while(MesageSize--){ LRC8 += *pucFrame++;}
  //Сделать результирующий байт отрицательным чтобы получить LRC байт
  LRC8 = (tU8)(-((tS8)LRC8));
  return LRC8;
}


/*
Функция проверяет валидность принятого сообщения, и если все нормально - формирует буфер 

*/
tU8 MBAsciiRx(tU8* InBuffer, tU8 MesageSize, ModbusAsciiType* MBAclient)
{
  tU8* BufAsciiToChar =&MBAclient->BufferAscii[0];
  tU8* DataIn=&MBAclient->DataAsciiCnt; //присвоили указателю адрес переменной счетчика для улучшения читаемости
   
  MBAclient->StopTimer(); 
  if (InBuffer[0] == MBA_BEGIN) //если пришло начало сообщения, независимо от статуса работы
  {
    if ((InBuffer[MesageSize-1] == MBA_END_LF)&(InBuffer[MesageSize-2] == MBA_END_CR))//и сообщение пришло оконченным
    {
      //проверили валидность пакета - конец верный CR_LF     
      *DataIn =0; //сбросили
      MBAclient->Status = STATE_RX_RCV;
      //переписываем в промежуточный буфер с преобразованием
      tU8 ByteCnt=0; //счетчик байт итогового сообщения
      for(tU8 i=1; i<(MesageSize-2);){
        BufAsciiToChar[ByteCnt]= (mbaAsciiToData(InBuffer[i])<<4) + mbaAsciiToData(InBuffer[i+1]);
        i+=2;
        ByteCnt++;
      }
      for(tU8 i=0; i<(ByteCnt); i++){InBuffer[i] = BufAsciiToChar[i] ;} //копируем          
      //проверяем контрольную сумму
      if(mbaLRC(&InBuffer[0], ByteCnt) == 0){//сумма сошлась
        FrameEndCrc16(&InBuffer[0], (ByteCnt+1)); //заменили LRC на CRC, чтобы меньше менять в коде
        return (ByteCnt+1);     //возвращаем размер посылки в байтах с crc              
      }      
    }
    else 
    {
      MBAclient->Status = STATE_RX_WAIT_EOF; //окончания сообщения нет - изменили статус и ждем дальше  
      MBAclient->SetTimer(ASKII_TIME_PAUSE); //запуск таймера на таймаут приема
      //копируем данные и храним их количество
      for(tU8 i=0; i<MesageSize;i++ ){BufAsciiToChar[i] = InBuffer[i];} //сохранили данные
      *DataIn = MesageSize; //сохранили количество
    }
  }
  else if (MBAclient->Status == STATE_RX_WAIT_EOF) //если нам пришло не начало, то проверяем статус - ждем ли мы окончания старого сообщения, если ждем
  {
    if ((*DataIn + MesageSize)>=MODBUS_BUFFER_SIZE){//если у нас пришло больше данных, чем должно быть в посылке
      *DataIn =0;
      MBAclient->Status = STATE_RX_RCV; 
    }
    else{ //данные не вышли за максимальный размер пакета
      //копируем данные и храним их количество
      for(tU8 i=0; i<(*DataIn +MesageSize);i++ ){BufAsciiToChar[*DataIn+i] = InBuffer[i];} //сохранили данные
      *DataIn += MesageSize; //сохранили количество
      if ((BufAsciiToChar[*DataIn-1] == MBA_END_LF)&(BufAsciiToChar[*DataIn-2] == MBA_END_CR))//и сообщение пришло оконченным
      {
        MBAclient->Status = STATE_RX_RCV; //сменили статус
        //переписываем обратно с преобразованием
        tU8 ByteCnt=0; //счетчик байт итогового сообщения
        for(tU8 i=1; i<(*DataIn-2);){
          InBuffer[ByteCnt]= (mbaAsciiToData(BufAsciiToChar[i])<<4) + mbaAsciiToData(BufAsciiToChar[i+1]);
          i+=2;
          ByteCnt++;
        }        
        *DataIn =0; //сбросили
        //проверяем контрольную сумму
        if(mbaLRC(&InBuffer[0], ByteCnt) == 0){//сумма сошлась
          FrameEndCrc16(&InBuffer[0], (ByteCnt+1)); //заменили LRC на CRC, чтобы меньше менять в коде
          return (ByteCnt+1);     //возвращаем размер посылки в байтах с crc              
        }         
      }
      else MBAclient->SetTimer(ASKII_TIME_PAUSE); //запуск таймера на таймаут приема     
    }  
  }
  return 0; //если ошибка или пришел пакет без конца
  //если ошибка - то надо ответить - что пришла ошибка
  //если пакет без конца - ждем конца
}


/*
Функция формирует буфер отправки в формате аски
*/
tU8 MBAsciiTx(tU8* OutBuffer, tU8 MesageSize)
{
  tU8 BufferCharToAscii[MODBUS_BUFFER_SIZE];
  tU8 CharCnt=1;
  
  //буфер к нам призодит с CRC, но нам надо убрать CRC и добавить LRC
  OutBuffer[MesageSize-2] = mbaLRC(&OutBuffer[0], (MesageSize-2));
  //копировали данные в промежуточный буфер
  for(tU8 i=0;i<(MesageSize-1);i++){BufferCharToAscii[i] = OutBuffer[i];}
  
  //собираем итоговый буфер уже из символов аски  
  OutBuffer[0] = MBA_BEGIN; //собрали буфер данных
  for(tU8 i=0; i<(MesageSize-1); i++){ //срс нам не надо
    OutBuffer[CharCnt] = mbaDataToAscii(BufferCharToAscii[i]>>4); CharCnt ++;
    OutBuffer[CharCnt] = mbaDataToAscii(BufferCharToAscii[i]&0x0f); CharCnt ++;    
  }
  OutBuffer[CharCnt] = MBA_END_CR;CharCnt ++;
  OutBuffer[CharCnt] = MBA_END_LF;CharCnt ++;
  
  return CharCnt;
}

/*
MBAsciiSlaveProc - функция для слейва
Функция декодирует из аски символов пришедшее сообщение, проверяет его валидность, если сообщение целое - вызывает функцию
обработки команд. При необходимости ответа, ответ кодируется в аски формат

  ModbusSlaveType* Slave - указатель на текущий слейв
  ModbusAsciiType* MBASlave - указатель на структуру данных модбас аски для этого слейва
  tU8 MesageSize - размер принятого по уарту сообщения
  ModbusCommandHandlerType* HandlerTable - указатель на массив команд

  ret: размер посылки для отправки в байтах
*/
tU8 MBAsciiSlaveProc(ModbusSlaveType* Slave, ModbusAsciiType* MBASlave, tU8 MesageSize, ModbusCommandHandlerType* HandlerTable)
{
  tU8 MsgSize = MBAsciiRx(&Slave->Buffer[0], MesageSize, MBASlave); //проверили и обработали пришедшие данные
  if(MsgSize){   //если нет никаких ошибок и пакет принят полностью - надо обрабатывать команду
    MsgSize = ModbusCommandDecode(Slave, MsgSize, HandlerTable); //вызываем функцию декодирования, определяем размер пакета для отправки 
    if(MsgSize){ //если требуется ответ - кодируем ответ в формат аски
      MsgSize = MBAsciiTx(&Slave->Buffer[0], MsgSize);   
      MBASlave->Status = STATE_RX_RCV; //после отправки статус - прием нового сообщения
      return MsgSize;
    }
  }
  return 0;
}

/*
MBAsciiMasterProc - функция для мастера
Функция декодирует из аски символов пришедшее сообщение, проверяет его валидность, если сообщение целое - вызывает функцию
обработки команд. При необходимости ответа, ответ кодируется в аски формат

  MBmasterType* Master - указатель на текущий слейв
  ModbusAsciiType* MBAclient - указатель на структуру данных модбас аски для этого мастера
  tU8 MesageSize - размер принятого по уарту сообщения

  ret: размер посылки для отправки в байтах
*/
tU8 MBAsciiMasterProc(MBmasterType* Master, tU8 MesageSize, ModbusAsciiType* MBAclient)
{ 
  tU8 MsgSize = MBAsciiRx(&Master->InBuf[0], MesageSize, MBAclient); //проверили и обработали пришедшие данные    
  if (MsgSize) MBAclient->Status = STATE_RX_RCV;
  else if (MBAclient->Status == STATE_RX_WAIT_EOF)
  {
    Master->Status =RECIEVE;
  }
  return MsgSize;
}

//функция проверки на таймаут
void TimeOutCheck(tU8* Status)
{
  //если мы пытали принять данные, но время вышло - переключаем на прием нового сообщения
  if (*Status == STATE_RX_WAIT_EOF) *Status = STATE_RX_RCV;
  
}

/*******************************************************************************
Функция копирования данных без перестановкой байт
*******************************************************************************/
void ModbusCopy(tU8* Src, tU8* Dst, tU16 ByteNum)
{
  while (ByteNum) {
    ByteNum--;
    *Dst++ = *Src++;
  }
}


