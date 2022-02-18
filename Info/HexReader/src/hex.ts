import { getCRC16 } from "./crc/crc16";
import { TFlashSegmen } from "./hextypes";
import { intersection, U16ToU8Array, U16ToU8ArrayLE, U32ToU8ArrayLE } from "./mcu";

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

class TFirmwareCheckInfo {
	info: string; //0x08008200;
	CheckFrom: string; //0x08008208;
	CheckTo: string; //0x0800C4FF;
}

class TAppCheckInfo {
  AppSize: number;
  AppCRC16: number;
  AppInfoCrc: number;
}
const SIZE_OF_APP_CHECK_INFO: number = 8;

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

function addCRC16ToCodeArea(Areas: Array<TFlashSegmen>, FirmwareCheckInfo : TFirmwareCheckInfo) {
  if (FirmwareCheckInfo === undefined) {
    console.warn('FirmwareCheckInfo is out of settings');
    return;
  }
  /*Find out, the Area is there an intersection with checked region*/
  let Area: TFlashSegmen = undefined;
  let A: {start:number, end: number} = {start:parseInt(FirmwareCheckInfo.info),
                                            end:parseInt(FirmwareCheckInfo.CheckTo)};
  
  for (const area of Areas) {
      const AreaStartAddr: number = parseInt(area.start);
      const AreaEnd: number = AreaStartAddr + (Number(area.size) || 0);
      const B: {start:number, end: number} = {start:AreaStartAddr, end:AreaEnd};
      if (intersection(A, B)) {
        Area = area;
        break;
      }
  }

  if (Area === undefined) {
    console.warn('Area is out of range');
    return;
  }
  /*get the code from specified Area and to calculate the CRC of it,
         BAUT! the start addres for crc should be FirmwareCheckInfo.CheckFrom*/
  const CheckStartPosition: number = parseInt(FirmwareCheckInfo.CheckFrom) - parseInt(Area.start);
  const CheckEndPosition: number = parseInt(FirmwareCheckInfo.CheckTo) - parseInt(Area.start);
  const CheckedCode:Array<number> =  Area.code.slice(CheckStartPosition, CheckEndPosition);
  const AppCRC16: number = getCRC16(new Uint8Array(CheckedCode));
  /* Write a class of TAppCheckInfo (take it from Application src) and fill it of data:
         - size of Application
         - Application's CRC*/
  const AppCheckInfo: TAppCheckInfo = {
    AppSize: CheckedCode.length,
    AppCRC16,
    AppInfoCrc:0
  }
  /*Calculate CRC of the TAppCheckInfo*/
  /*Convert the TAppCheckInfo to  Byte Array*/
  const AppCheckInfoDataBin: Uint8Array = new Uint8Array([
    ...U32ToU8ArrayLE(AppCheckInfo.AppSize),
    ...U16ToU8ArrayLE(AppCheckInfo.AppCRC16)
  ]);
  const AppCheckInfoSummaryBin: Uint8Array = new Uint8Array([
    ...AppCheckInfoDataBin,
    ...U16ToU8Array(getCRC16(AppCheckInfoDataBin))
  ]);

  /*write the prepared Array of TAppCheckInfo to Area */
  const AppinfoStartPosition: number = parseInt(FirmwareCheckInfo.info) - parseInt(Area.start);
  
  Area.code.splice(AppinfoStartPosition, SIZE_OF_APP_CHECK_INFO, ...AppCheckInfoSummaryBin);
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
