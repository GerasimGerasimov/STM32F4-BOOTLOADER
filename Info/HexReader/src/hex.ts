import { TFlashSegmen } from "./hextypes";

class TAreaProps {
  PrevOffset: number = 0;
  segSize: number = 0;

  public getLastPosition(): TFlashSegmen {
    return {
      start: `0x${(this.PrevOffset).toString(16)}`,
      size: this.segSize
    }
  }

  public getFrom(): string {
    return `0x${(this.PrevOffset).toString(16)}`;
  }

  public getNewArea(addr: number, size: number): TFlashSegmen | undefined {
    var newArea: TFlashSegmen = undefined;
    if ((addr - this.segSize) > this.PrevOffset){
      if (this.segSize !== 0) {
        newArea = {
          start: `0x${(this.PrevOffset).toString(16)}`,
          size: this.segSize
        };
      }
      this.PrevOffset = addr;
      this.segSize    = size;
      return newArea;
    } else {
      this.segSize += size;
      return undefined;
    }
  }
}

export function getUsageMemoryAddresAndSize(content:Array<string>): Array<TFlashSegmen> {
  var segmentAddr = 0;
  const Area: TAreaProps = new TAreaProps();
  const res:  Array<TFlashSegmen> = [];
  for (const idx in content) {
    const hexstr: string = content[idx];
    switch (getCommand(hexstr)) {
      case '04'://расширение адреса сегмента до 32 бит, старшие 16 бит
        segmentAddr = getAdditionSegmentAddress(hexstr);
        break;
      case '00'://данные
        const {Addr, size} = getStartAddrAndSizeOfCodeStr(hexstr, segmentAddr);
        const NewArea: TFlashSegmen = Area.getNewArea(Addr, size);
        if (NewArea) res.push(NewArea);
        break;
      case '05'://адрес начала приложения ARM
        res.push({start:getMainAddr(hexstr), size:'main'});
        break;
      case '01'://конец файла
        res.push(Area.getLastPosition());
        break;
      default:
        break;
    }
  }
  return res;
}

export function getMainAddr(str: string): string {
  return `0x${str.slice(9,17)}`
}

export function getStrFirstAddr(str: string, Addition: number): number {
  const hsaddr: string =  `0x${str.slice(3,7)}`;
  const addr: number = Addition + parseInt(hsaddr);
  return addr;
}

//:10 0070 00 E90B0108ED0B0108F10B0108F50B0108 74
export function getStartAddrAndSizeOfCodeStr(str: string, Addition: number): {Addr: number, size: number} {
  const size: number = getHexSrtLenght(str);
  const FirstAddr: number = getStrFirstAddr(str, Addition);
  return {Addr: FirstAddr, size};
}

//:020000040800F2
export function getAdditionSegmentAddress(str: string): number {
  return parseInt(`0x${str.slice(9,13)}0000`)
}

export function getHexSrtLenght(str: string): number {
  const hexStr: string = `0x${str.slice(1,3)}`;
  const len: number = parseInt(hexStr);
  return len;
}

export function getCommand(str: string): string{
  return str.slice(7,9);
}