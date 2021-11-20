type TFlashSegmen = {
  start: string;
  size: number;
}

export function getFlashPages(): Array<TFlashSegmen> {
  const FlashPagesSTM32F407IGT: Array<TFlashSegmen> = [
    {start: '0x08000000', size: 0x03FFF},//16K
    {start: '0x08004000', size: 0x03FFF},//16K
    {start: '0x08008000', size: 0x03FFF},//16K
    {start: '0x0800C000', size: 0x03FFF},//16K

    {start: '0x08010000', size: 0x0FFFF},//64K

    {start: '0x08020000', size: 0x1FFFF},//128K
    {start: '0x08040000', size: 0x1FFFF},//128K
    {start: '0x08060000', size: 0x1FFFF},//128K
    {start: '0x08080000', size: 0x1FFFF},//128K
    {start: '0x080A0000', size: 0x1FFFF},//128K
    {start: '0x080C0000', size: 0x1FFFF},//128K
    {start: '0x080E0000', size: 0x1FFFF}//128K
  ];
  return FlashPagesSTM32F407IGT;
}