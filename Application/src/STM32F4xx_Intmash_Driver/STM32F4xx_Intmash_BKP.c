#include "STM32F4xx_Intmash_BKP.h"


/*
  как с этим работать:

если решили использовать BkpSram, необходимо его инициализировать, для этого 
вместе со всей инициализацией вызываем:
  BkpSram_Configuration();
  //оно проинициализируется только раз, и дальше, если будут ребуты - заново не будет переинициализироваться

 если хотите записать чтото в BkpSram, надо вызвать
u8 BkpSramWrite( u8 *Src, u16 BytesNum, u16 BkpSramAddr )
где  u8 *Src - указатель на начало данных, например на массив
     u16 BytesNum - количество записываемых байт
     u16 BkpSramAddr - сдвиг в памяти от начала сектора BkpSram,
     если записываемые байты выйдут за размер сектора BkpSram, функция вернет 1
     если запись прошла успешно и за размеры сектора не вылезли - вернет 0

  если хотите прочитать чтото из BkpSram, надо вызвать
u8 BkpSramRead( u8 *Dst, u16 BytesNum, u16 BkpSramAddr )
где  u8 *Dst - указатель на начало данных, например на массив
     u16 BytesNum - количество записываемых байт
     u16 BkpSramAddr - сдвиг в памяти от начала сектора BkpSram,
     если читаемое количетсво байт выйдет за размер сектора BkpSram, функция вернет 1
     если чтение прошло успешно и за размеры сектора не вылезли - вернет 0
         
  эти две функции сами включат разрешат доступ к BkpSram при записи
  и сами включат тактирование. Дополнительно ничего не надо
*/


//#define BKPSRAM_BASE  0x40024000 //начало срам, уже где-то там определено
#define BKPSRAM_END  0x40024FFF //конец срам
//#define UNLOCK_RTC_2 0x53 //второе значение для записи для разблокировки
//#define LOCK_RTC 0xFF //любая чушь, отличная от предыдущих


/*функция Инициализациb BkpSram*/
/**
  * @brief  Инициализация BkpSram
  * @param  none
  * @retval none.
  */
void BkpSram_Configuration(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  
  //разрешили доступ к памяти для записи
  PWR->CR |= PWR_CR_DBP; //
  //Enable backup SRAM Clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  
  //Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode
  PWR->CSR |= 0x0200;   //включили для сохранения дзначений в выключенном режиме
  
  PWR_BackupAccessCmd(ENABLE);                          // set PWR->CR.dbp = 1;
  while (!(PWR->CSR & PWR_CSR_BRR))  { } //ждем, пока не разрешится
  
  
}

u8 BkpSramWrite( u8 *Src, u16 BytesNum, u16 BkpSramAddr ) {
  const u16 backup_size = 0x1000;
  u8* base_addr = (u8 *) BKPSRAM_BASE;
  u16 i;
  if( BytesNum + BkpSramAddr >= backup_size ) {
    /* ERROR : the last byte is outside the backup SRAM region */
    return 1;
  }
  //включаем тактирование для записи - если это есть в инициализации - не используем
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);   // set RCC->APB1ENR.pwren
    /* disable backup domain write protection */
//  PWR_BackupAccessCmd(ENABLE);                          // set PWR->CR.dbp = 1;
  /** enable the backup regulator (used to maintain the backup SRAM content in
    * standby and Vbat modes).  NOTE : this bit is not reset when the device
    * wakes up from standby, system reset or power reset. You can check that
    * the backup regulator is ready on PWR->CSR.brr, see rm p144 */
//  PWR_BackupRegulatorCmd(ENABLE);     // set PWR->CSR.bre = 1;
  for( i = 0; i < BytesNum; i++ ) {
    *(base_addr + BkpSramAddr + i) = *(Src + i);
  }
// PWR_BackupAccessCmd(DISABLE);                     // reset PWR->CR.dbp = 0;
  return 0;
}

u8 BkpSramRead( u8 *Dst, u16 BytesNum, u16 BkpSramAddr ) {
  const u16 backup_size = 0x1000;
  u8* base_addr = (u8 *) BKPSRAM_BASE;
  u16 i;
  if( BytesNum + BkpSramAddr >= backup_size ) {
    /* ERROR : the last byte is outside the backup SRAM region */
    return 1;
  }
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  for( i = 0; i < BytesNum; i++ ) {
    *(Dst + i) = *(base_addr + BkpSramAddr + i);
  }
  return 0;
}

