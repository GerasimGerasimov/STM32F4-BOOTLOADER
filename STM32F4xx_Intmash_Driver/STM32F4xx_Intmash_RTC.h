/*
  как с этим работать:

если решили использовать RTC, необходимо его инициализировать, для этого 
вместе со всей инициализацией вызываем:
  RTC_Configuration();

затем необходимо создать элемент структуры TDateTime_stm32f407, например, как это
создано для работы с MDLL:
  TDateTimeIntRTC datetime_stm32f407;//элемент структуры для получения времени в MDLL

необходимо натроить необходимую дату, записав требуемые значения в созданный элемент структуры (datetime_stm32f407)
и вызвать функцию, указав аргументом элемент структуры, куда записали данные 
  IntmashRTC_SetDate(&datetime_stm32f407);

необходимо натроить необходимое время, записав требуемые значения в созданный элемент структуры (datetime_stm32f407)
и вызвать функцию, указав аргументом элемент структуры, куда записали данные 
  IntmashRTC_SetTime(&datetime_stm32f407);

чтобы получить данные о дате/времени, надо  вызвать функцию, указав агрументом элемент структуры, 
куда хотим получить данные
  IntmashRTC_GetDateTime(&datetime_stm32f407);


пример указания данных для даты/времени:

datetime_stm32f407.Year = 13 //от 0 до 99, считаем начало каждого столетия = 0
datetime_stm32f407.Month =  8 // от 1 до 12   
datetime_stm32f407.Day =  5 //от 1 до 31. автоматически считает высокосные года (не проверяла, доверимся даташиту)            
datetime_stm32f407.DayOfWeek = 1 // от 1 до 7
datetime_stm32f407.Hours = 14 //  от 0 до 24   
datetime_stm32f407.Minutes = 49 // от 0 до 59
datetime_stm32f407.Seconds = 30 // от 0 до 59

*/

#ifndef _STM32F4XX_INTMASH_RTC_H_
#define _STM32F4XX_INTMASH_RTC_H_

#include "stm32f4xx.h"



    typedef struct
    {
        uint8_t Year;      // Год
        uint8_t Month;     // Месяц
        uint8_t Day;       // День месяца        
        uint8_t DayOfWeek; // День недели
        
        uint8_t Hours;     // Часы
        uint8_t Minutes;   // Минуты
        uint8_t Seconds;   // Секунды
        
        uint8_t Options;   // на всякий случай
    } TDateTimeIntRTC;
     
  #ifdef __cplusplus
    extern "C" void IntmashRTC_Configuration(void);// Инициализация модуля
    extern "C" void IntmashRTC_Reset(void); // Сброс состояния часов
    extern "C" void  IntmashRTC_GetDateTime(TDateTimeIntRTC * DateTime);// Получить текущее время
    extern "C" void IntmashRTC_SetDate(TDateTimeIntRTC * DateTime);// Установка нужной даты 
    extern "C" void IntmashRTC_SetTime(TDateTimeIntRTC * DateTime);
  #else
    extern void IntmashRTC_Configuration(void);
    extern void IntmashRTC_Reset(void); // Сброс состояния часов
    extern void  IntmashRTC_GetDateTime(TDateTimeIntRTC * DateTime);
    extern void IntmashRTC_SetDate(TDateTimeIntRTC * DateTime);
    extern void IntmashRTC_SetTime(TDateTimeIntRTC * DateTime);
  #endif
#endif
