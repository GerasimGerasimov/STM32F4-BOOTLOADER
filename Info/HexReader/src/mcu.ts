import { TFlashSegmen } from "./hextypes";

export function getErasedPages( domain: Array<TFlashSegmen>): Array<string> {
  const res:Set<string> = new Set();
  const pages: Array<TFlashSegmen> = getFlashPages();
  domain.forEach((value)=>{
    if (value.size !== 'main') {
      const domain_set = {start: parseInt(value.start), end: parseInt(value.start) + Number(value.size)};
      for (const idx in pages) {
        const page:TFlashSegmen = pages[idx];
        const page_set = {start: parseInt(page.start), end: parseInt(page.start) + Number(page.size)};
        if (intersection(domain_set, page_set)) {
          res.add(page.start);
          console.log(page);
        }
      }
    }
  })
  return [...res];
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

  return FlashPagesSTM32F407IGT;
}

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

const FlashPagesLPC2378: Array<TFlashSegmen> = [
  {start: '0x00000000', size: 0x0FFF},
  {start: '0x00001000', size: 0x0FFF},
  {start: '0x00002000', size: 0x0FFF},
  {start: '0x00003000', size: 0x0FFF},
  {start: '0x00004000', size: 0x0FFF},
  {start: '0x00005000', size: 0x0FFF},
  {start: '0x00006000', size: 0x0FFF},
  {start: '0x00007000', size: 0x0FFF},
  {start: '0x00008000', size: 0x7FFF},
  {start: '0x00010000', size: 0x7FFF},
  {start: '0x00018000', size: 0x7FFF},
  {start: '0x00020000', size: 0x7FFF},
  {start: '0x00028000', size: 0x7FFF},
  {start: '0x00030000', size: 0x7FFF},
  {start: '0x00038000', size: 0x7FFF},
  {start: '0x00040000', size: 0x7FFF},
  {start: '0x00048000', size: 0x7FFF},
  {start: '0x00050000', size: 0x7FFF},
  {start: '0x00058000', size: 0x7FFF},
  {start: '0x00060000', size: 0x7FFF},
  {start: '0x00068000', size: 0x7FFF},
  {start: '0x00070000', size: 0x7FFF},
  {start: '0x00078000', size: 0x0FFF},
  {start: '0x00079000', size: 0x0FFF},
  {start: '0x0007A000', size: 0x0FFF},
  {start: '0x0007B000', size: 0x0FFF},
  {start: '0x0007C000', size: 0x0FFF},
  {start: '0x0007D000', size: 0x0FFF}
];