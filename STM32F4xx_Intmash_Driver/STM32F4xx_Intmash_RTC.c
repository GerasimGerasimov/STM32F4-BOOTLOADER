#include "STM32F4xx_Intmash_RTC.h"


/*
как с этим работать:

если решили использовать RTC, необходимо его инициализировать, для этого 
вместе со всей инициализацией вызываем:
IntmashRTC_Configuration();

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


#define UNLOCK_RTC_1 0xCA //первое значение для записи для разблокировки
#define UNLOCK_RTC_2 0x53 //второе значение для записи для разблокировки
#define LOCK_RTC 0xFF //любая чушь, отличная от предыдущих
u8 RTC_init=0; //флаг, что часы конфигуриуемы
/*функция Выключения защиты от записи*/
/**
* @brief  Выключить защиту от записи
* @param  none
* @retval none.
*/
void IntmashRTC_Unlock(void)
{
  if((RTC_init==1)||(RTC->ISR & RTC_ISR_INITS))
  {//обязательно разрешить доступ!
    PWR_BackupAccessCmd(ENABLE);
    // Запишем эти значения по очереди
    RTC->WPR = UNLOCK_RTC_1;
    RTC->WPR = UNLOCK_RTC_2;    
    // Войдём в режим инициализации:
    RTC->ISR |= RTC_ISR_INIT;       
    // Ждём, когда это произойдёт
    while(!(RTC->ISR & RTC_ISR_INITF)) {}            
    // теперь Здесь можем менять регистры RTC
  }
  else {return;}  //если часы не инициализированы - даже не пытаемся с ними чтото сделать
}

/*функция Включения защиты от записи*/
/**
* @brief  Включить защиту от записи
* @param  none
* @retval none.
*/
void IntmashRTC_Lock(void)
{
  // Инициализация закончилась
  RTC->ISR &= ~RTC_ISR_INIT;
  RTC_WaitForSynchro();
  // Запишем какую-нибудь фигню, главное, чтоб не правильную
  RTC->WPR = LOCK_RTC;
}

/*функция установки даты*/
/**
* @brief  установить дату
* @param  DateTime - элемент структуры TDateTime_stm32f407 для получения времени в MDLL
* @retval none.
*/
void IntmashRTC_SetDate(TDateTimeIntRTC * DateTime) //static
{
  IntmashRTC_Unlock();
  
  uint32_t Tens, Units;
  uint32_t TempReg = 0;   
  // Очистим поле даты
  TempReg = 0;    
  // Запишем год
  Tens  = (DateTime->Year / 10) & 0x0f;          // Десятки лет
  Units = (DateTime->Year - (Tens * 10)) & 0x0f; // Единицы лет       
  TempReg |= (Tens  << 20); // YT, 20
  TempReg |= (Units << 16); // YU, 16
  // Запишем месяц
  Tens  = (DateTime->Month / 10) & 0x01;          // Десятки месяцев
  Units = (DateTime->Month - (Tens * 10)) & 0x0f; // Единицы месяцев
  TempReg |= (Tens  << 12); // MT, 12
  TempReg |= (Units << 8);  // MU, 8
  // Запишем день
  Tens  = (DateTime->Day / 10) & 0x03;          // Десятки дней
  Units = (DateTime->Day - (Tens * 10)) & 0x0f; // Единицы дней        
  TempReg |= (Tens  << 4); // DT, 4
  TempReg |= (Units << 0);  // DU, 0
  // День недели:
  TempReg |= ((DateTime->DayOfWeek & 0x07) << 13); // WDU, 13   
  // Записывать надо всё сразу
  
  RTC->DR = TempReg;
  IntmashRTC_Lock();
}

/*функция установки времени*/
/**
* @brief  установить время
* @param  DateTime - элемент структуры TDateTime_stm32f407 для получения времени в MDLL
* @retval none.
*/
void IntmashRTC_SetTime(TDateTimeIntRTC * DateTime) //static
{
  IntmashRTC_Unlock();
  uint32_t Tens, Units;
  uint32_t TempReg = 0;   
  // Очистим поле даты
  TempReg = 0;    
  // Запишем часы
  Tens  = (DateTime->Hours / 10) & 0x03;          // Десятки часов
  Units = (DateTime->Hours - (Tens * 10)) & 0x0f; // Единицы часов        
  TempReg |= (Tens  << 20); // HT, 20
  TempReg |= (Units << 16); // HU, 16
  // Запишем минуты
  Tens  = (DateTime->Minutes / 10) & 0x07;          // Десятки минут
  Units = (DateTime->Minutes - (Tens * 10)) & 0x0f; // Единицы минут        
  TempReg |= (Tens  << 12); // MNT, 12
  TempReg |= (Units << 8);  // MNU, 8
  // Запишем секунды
  Tens  = (DateTime->Seconds / 10) & 0x07;          // Десятки секунд
  Units = (DateTime->Seconds - (Tens * 10)) & 0x0f; // Единицы секунд      
  TempReg |= (Tens  << 4); // ST, 4
  TempReg |= (Units << 0);  // SU, 0   
  // Записывать надо всё сразу    
  RTC->TR = TempReg;
  IntmashRTC_Lock();
}

/*функция сброса состояния RTC*/
/**
* @brief  Сброс состояния часов
* @param  none
* @retval none.
*/
void IntmashRTC_Reset(void)
{
  // Включим тактирование PWR
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; 
  // Разрешим доступ к управляющим регистрам энергонезависимого домена
  PWR->CR |= PWR_CR_DBP;   
  // Выберем его как источник тактирования RTC:
  RCC->BDCR |=  RCC_BDCR_BDRST;
  RCC->BDCR &= ~RCC_BDCR_BDRST;
  PWR->CR &= ~PWR_CR_DBP;  
  u32 cnt_return = 0;
  while(!(RCC->BDCR & RCC_BDCR_BDRST)) {
    if(cnt_return == 60000) return; //кусок, спасающий от зависания
    else cnt_return++; //если в течение определенного времени нет инициализации - не инициализируем 
  } 
}

/*функция получинея даты/времени*/
/**
* @brief  Получить текущее время
* @param  DateTime - элемент структуры TDateTime_stm32f407 для получения времени в MDLL
* @retval none.
*/
void IntmashRTC_GetDateTime(TDateTimeIntRTC * DateTime)
{
  uint32_t Date = RTC->DR;
  uint32_t Time = RTC->TR;   
  // Год
  DateTime->Year      = ((Date >> 20) & 0x0f) * 10 + ((Date >> 16) & 0x0f);
  // Месяц
  DateTime->Month     = ((Date >> 12) & 0x01) * 10 + ((Date >>  8) & 0x0f);
  // День
  DateTime->Day       = ((Date >>  4) & 0x03) * 10 + ((Date >>  0) & 0x0f);
  // День недели
  DateTime->DayOfWeek = ((Date >> 13) & 0x07);   
  // Час
  DateTime->Hours     = ((Time >> 20) & 0x03) * 10 + ((Time >> 16) & 0x0f);
  // Минуты
  DateTime->Minutes   = ((Time >> 12) & 0x07) * 10 + ((Time >> 8) & 0x0f);
  // Секунды
  DateTime->Seconds   = ((Time >> 4) & 0x07) * 10 + ((Time >> 0) & 0x0f);
}

/*функция Инициализациb RTC*/
/**
* @brief  Инициализация RTC
* @param  none
* @retval none.
*/
void IntmashRTC_Configuration(void)
{
  u32 cnt_return = 0;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  
  //Enable backup SRAM Clock
 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  //PWR->CR |= PWR_CR_DBP;// PWR_BackupAccessCmd(ENABLE); 
    
  // Если часы запущены, делать тут нечего.  
  if(RTC->ISR & RTC_ISR_INITS)  { return;}  
  RTC_DeInit(); //встроенная функция 

  // Enable the LSE OSC
  RCC_LSEConfig(RCC_LSE_ON);
  // Wait till LSE is ready
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)  
  {
    if(cnt_return == 600000) return; //кусок, спасающий от зависания
    else cnt_return++; //если в течение определенного времени нет инициализации - не инициализируем 
  }
  
  RCC->BDCR &= ~RCC_BDCR_RTCSEL; // сбросим источник тактирования полностью
  // Select the RTC Clock Source
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  // Enable the RTC Clock /
  RCC_RTCCLKCmd(ENABLE);  
  // Wait for RTC APB registers synchronisation (needed after start-up from Reset)
  RTC_WaitForSynchro();
  
  //отключили защиту регистров и вкл инициализацию
  RTC_init =1;// выствили флаг, что можон инициализировать дальше
  IntmashRTC_Unlock();
  // Часы остановлены. Режим инициализации
  // Настроим предделитель для получения частоты 1 Гц.       
  // LSI: 
  // LSE: нужно разделить на 0x7fff (кварцы так точно рассчитаны на это)
  {          
        /* Clear RTC CR FMT Bit */
    RTC->CR &= ((uint32_t)~(RTC_CR_FMT));
    /* Set RTC_CR register */
    RTC->CR |=  (uint32_t)RTC_HourFormat_24;
  
    /* Configure the RTC PRER */
    RTC->PRER = (uint32_t)(0xff); //RTC_SynchPrediv
    RTC->PRER |= (uint32_t)(0x7f << 16); //RTC_AsynchPrediv
    
  }  
  // Инициализация закончилась
  IntmashRTC_Lock();
  
  RTC_BypassShadowCmd(ENABLE);  //выставили, теперь при чтении данные всегда читаются напрямую , а не из теневых регистров
  
}

