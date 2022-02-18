import { getCRC16 } from "./crc/crc16";
import { TFlashSegmen } from "./hextypes";
import { intersection, U16ToU8Array, U16ToU8ArrayLE, U32ToU8ArrayLE } from "./mcu";

export class TFirmwareCheckInfo {
	info: string = ''; //0x08008200;
	CheckFrom: string = ''; //0x08008208;
	CheckTo: string = ''; //0x0800C4FF;
}

class TAppCheckInfo {
  AppSize: number;
  AppCRC16: number;
  AppInfoCrc: number;
}
const SIZE_OF_APP_CHECK_INFO: number = 8;

export function addCRC16ToCodeArea(Areas: Array<TFlashSegmen>, FirmwareCheckInfo : TFirmwareCheckInfo) {
  if (!validateFirmwareCheckInfo(FirmwareCheckInfo)) return;
  /*Find out, the Area is there an intersection with checked region*/
  const Area: TFlashSegmen = getAreaIncludedFirmwareCheckInfo(Areas, FirmwareCheckInfo);

  if (Area === undefined) {
    console.warn('Area is out of range');
    return;
  }
  /*get the code from specified Area and to calculate the CRC of it,
         BUT! the start addres for crc should be FirmwareCheckInfo.CheckFrom*/
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

function validateFirmwareCheckInfo(FirmwareCheckInfo : TFirmwareCheckInfo): boolean {
  if (FirmwareCheckInfo === undefined) {
    console.warn('FirmwareCheckInfo is out of settings');
    return false;
  }
  for (const key in new TFirmwareCheckInfo) {
    if (!(key in FirmwareCheckInfo)) {
      console.warn(`${key} is out of FirmwareCheckInfo`);
      return false;
    }
  }
  return true;
}

function getAreaIncludedFirmwareCheckInfo(Areas: Array<TFlashSegmen>, FirmwareCheckInfo : TFirmwareCheckInfo):TFlashSegmen | undefined {
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
  return Area;
}
