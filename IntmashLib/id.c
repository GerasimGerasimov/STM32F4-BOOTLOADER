#include "Intmash_bastypes.h"

const char DigitsChars[10]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};//массив символов цифр
char DeviceID[256];//массив дл€ формировани€ ID-строки

/**
  * @brief  формирование ID-строки в формате xxxxxxxx IDtext, 
  * где xxxxxxxx - серийный номер устросйтва. ≈сли номер не 8-значное число,
  * перед номером будут идти нули, чтобы суммарно было 8 символов. 
  * например 00000365 DExS.M4CPU v0.03 03.06.2013 www.intmash.ru
  * сформированна€ строка хранитс€ в символьном массиве DeviceID.
  * @param  u32 SN - серийный номер устросйтва, число от 1 до 99999999
  * @retval none.
  */
#define SN_SIZE 8 //размер серийного номера
#define START_DIVIDER 10000000 //начальный делитель, 8-й разр€д
#define TEXT_INDEX_SHIFT 9 //смещени€ дл€ текста
void IDinit(unsigned int SN, char* ID_text)
{ 
  tU8  Index=0;//индекс дл€ перемещение по массиву ID
  
  /*—начала разместим серийный номер*/
  tU32 Divider=START_DIVIDER;//делитель 
  tU8  Digit;//число в текущем разр€де  
  while(Index<SN_SIZE){//заполн€ем 8-значный серийный номер
	Digit=SN/Divider;//число в текущем разр€де  
	SN=SN%Divider;//теперь серийный номер уменьшили на одну цифру 
	DeviceID[Index]=DigitsChars[Digit];//заполнили р€зр€д символом
	Divider=Divider/10;//делитель уменьшили на 10, переходим в ругой разр€д
	Index++;//переходим к следующему симфолу
  } 
  
  /*ƒалее ставим пробел*/
  DeviceID[Index]=' ';
  Index++;
  
  /*“еперь размещаем текстовую строку*/
  while(ID_text[Index-TEXT_INDEX_SHIFT] != 0) {//пока строка не кончилась
	DeviceID[Index]=ID_text[Index-TEXT_INDEX_SHIFT];//копируем посимвольно
	Index++;//переходим к следующему символу
  }
  
  /*завершаем строку ID*/
  DeviceID[Index]=0;  
}