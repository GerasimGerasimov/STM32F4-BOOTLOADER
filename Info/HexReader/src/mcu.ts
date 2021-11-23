import { TFlashSegmen } from "./hextypes";

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
export function getErasedPages( domain: Array<TFlashSegmen>): Array<string> {
  const res:Array<string> = [];
  const pages: Array<TFlashSegmen> = getFlashPages();
  domain.forEach((value)=>{
    if (value.size !== 'main') {
      const domain_set = {start: parseInt(value.start), end: parseInt(value.start) + Number(value.size)};
      for (const idx in pages) {
        const page:TFlashSegmen = pages[idx];
        const page_set = {start: parseInt(page.start), end: parseInt(page.start) + Number(page.size)};
        if (intersection(domain_set, page_set)) {
          res.push(page.start);
          console.log(page);
        }
      }
    }
  })
  return res;
}

function intersection(A: {start:number, end: number}, B:{start: number, end: number}): boolean {
  let [a,b] = (A.start <= B.start) //сортировка по младшим значениям начал
              ? [A, B]
              : [B, A];
  if (a.end < b.start) return false;
  if (b.end < a.start) return false;
  return true;
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