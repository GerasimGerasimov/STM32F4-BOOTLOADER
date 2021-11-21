
type TFlashSegmen = {
  start: string;
  size: number | string;
}

/*
passed:
{"start":"0x8000000","info":61368} EFB8 = 3FFF + 3FFF + 3FFF + 2FBB
{"start":"0x80a0000","info":104}
{"start":"0x80c0000","info":104}
{"start":"0x0800DD81","info":"main"}
{"start":"0x80e0000","info":4}

result:
    {start: '0x08000000', size: 0x03FFF},//16K 3FFF
    {start: '0x08004000', size: 0x03FFF},//16K 3FFF
    {start: '0x08008000', size: 0x03FFF},//16K 3FFF
    {start: '0x0800C000', size: 0x03FFF},//16K 2FBB

    {start: '0x080A0000', size: 0x1FFFF},//128K
    {start: '0x080C0000', size: 0x1FFFF},//128K
    {start: '0x080E0000', size: 0x1FFFF} //128K

*/
export function getErasedPages( PropramAreAS: Array<TFlashSegmen>): Array<string> {
  const res:Array<string> = [
    '0x08000000',//16K 3FFF
    '0x08004000',//16K 3FFF
    '0x08008000',//16K 3FFF
    '0x0800C000',//16K 2FBB
    '0x080A0000',//128K
    '0x080C0000',//128K
    '0x080E0000' //128K
  ];
  return res;
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
    {start: '0x080E0000', size: 0x1FFFF} //128K
  ];
  return FlashPagesSTM32F407IGT;
}