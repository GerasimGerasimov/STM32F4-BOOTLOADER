import { addCRC16ToCodeArea, TFirmwareCheckInfo } from "./FirmwareCheckInfo";
import { TFlashSegmen } from "./hextypes";

class TAreaProps {

  private Code: Array<number>=[];
  PrevOffset: number = 0;
  segSize: number = 0;

  public isNewArea(Addr: number): boolean {
    const addr: number = Addr;
    const res: boolean =  (((addr - this.segSize) > this.PrevOffset) && (this.segSize !== 0));
    return res;
  }

  public getAreaData(): TFlashSegmen {
    return {
      start: `0x${(this.PrevOffset - this.segSize).toString(16)}`,
      size: this.segSize,
      code: [...this.Code]
    };
  }

  public setNewCodeString(addr: number, size: number, code:Array<number>) {
    this.Code.push(...code);
    this.segSize += size;
    this.PrevOffset = addr + size;
  }

}

export function getUsageMemoryAddresAndSize(content:Array<string>, FirmwareCheckInfo : TFirmwareCheckInfo): Array<TFlashSegmen> {
  var Area: TAreaProps = new TAreaProps;
  const res:  Array<TFlashSegmen> = [];
  var SegAddr: number = 0;
  for (const idx in content) {
    const hexstr: string = content[idx];
    const cmd: string = getCommand(hexstr);
    switch (cmd) {
      case '00'://данные
        const {Addr, size, code} = getStartAddrSizeAndDataOfCodeStr(hexstr);
        if (Area.isNewArea(Addr+SegAddr)) {
          res.push(Area.getAreaData());
          Area = new TAreaProps;
        } 
        Area.setNewCodeString(Addr+SegAddr, size, code);
        break;
      case '01'://конец файла
        break;
      case '05'://адрес начала приложения ARM
        res.push(Area.getAreaData());
        res.push({start:getMainAddr(hexstr), size:'main', code: []});
        break;
      case '04'://расширение адреса сегмента до 32 бит, старшие 16 бит
        SegAddr = getAdditionSegmentAddress(hexstr);
        break;
      default:
        break;
    }
  }
  addCRC16ToCodeArea(res, FirmwareCheckInfo);
  return res;
}

export function getMainAddr(str: string): string {
  return `0x${str.slice(9,17)}`
}

export function getStrFirstAddr(str: string): number {
  const hsaddr: string =  `0x${str.slice(3,7)}`;
  const addr: number = parseInt(hsaddr);
  return addr;
}

// SZ ADDR CM |             DATA             |
//:10 0000 00 1848002081DD000855B9000857B90008 DC
//:10 0070 00 E90B0108ED0B0108F10B0108F50B0108 74
//012 3456 78 9
//            0 1 2 3 4 5 6 7 8 9 A B C D E F
export function getStartAddrSizeAndDataOfCodeStr(str: string): {Addr: number, size: number, code:Array<number>} {
  const size: number = getHexSrtLenght(str);
  const FirstAddr: number = getStrFirstAddr(str);
  const code: Array<number> = getCode(str);
  return {Addr: FirstAddr, size, code};
}

function getNumbersArrayFromHexStr(str: string):Array<number> {
  let count = 0;
  let hex: string = '0x';
  const res:Array<number> = Array.from(str).reduce((acc, item)=>{
    hex += item;
    if (count++ === 1) {
      acc.push(parseInt(hex));
      hex = '0x';
      count = 0;
    };
    return acc;
  },[]);
  return res;
}

function getCode(str: string): Array<number> {
  const codestr = str.slice(9, -3);
  const res: Array<number> = getNumbersArrayFromHexStr(codestr);
  return res;
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
