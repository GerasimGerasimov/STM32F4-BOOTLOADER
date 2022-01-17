import fs = require ("fs");
import ComPort from "./comport/comport";
import { appendCRC16toArray, getCRC16 } from "./crc/crc16";
import { getUsageMemoryAddresAndSize} from "./hex";
import { TFlashSegmen } from "./hextypes";
import { ErasedPagesToU8Array, getErasedPages, U16ToU8Array, U32ToU8Array} from "./mcu";
import { settings } from "./settings";
import { delay } from "./utils/delay";

const setings = {
  "App": "D:/Project/Projects/!SMFCB/DExSys/FW/CM4/DExS-SMFCB/EWARMv6/STM32F407IGT6/Exe/DExS.M4CPU.SMFCB.hex",
  "SkipSectors":['0x080A0000', '0x080C0000', '0x080E0000']
}

console.log('Start Hex Reader');
//'./src/hex-samples/STM32-APP.hex'
const src: string = setings.App;
const fileContent: Array<string> = fs.readFileSync(src).toString().split("\n");
const Areas: Array<TFlashSegmen> = getUsageMemoryAddresAndSize(fileContent);

const COMx: ComPort = new ComPort(settings.COM);

(async () => { 
  while (true) {
    try {
      await COMx.waitForOpen();
      const ID: string = await getID();
      if (isItApplication(ID)) {
        console.log('Swap to Bootloader');
        await startBootloader();
      }
      const AvailiblePages: Array<TFlashSegmen> = await getAvailablePagesList();
      const ErasedPages:Array<string> = getErasedPages(Areas, AvailiblePages, setings.SkipSectors);
      await eraseSpecifiedPages(ErasedPages);
      await downloadCodeToMCU(setings.SkipSectors);
      await startApplication();
      console.log('Hex Reader has Done');
      process.exit(0);
    } catch (e) {
      await delay(1000);
      console.log('главЛовушка',e);
    }
  }
})();

async function getID(): Promise<string> {
  const FieldBusAddr: number = 0x01;
  const cmdSource = new Uint8Array([FieldBusAddr, 0x11]);
  const cmd: Array<number> = Array.from(appendCRC16toArray(cmdSource))
  const answer: any = await COMx.getCOMAnswer({cmd});
  const msg: Array<number> = validateAnswer(answer);
  return Buffer.from(msg.slice(3,-2)).toString('ascii');
}

async function getAvailablePagesList(): Promise< Array<TFlashSegmen>> {
  const FieldBusAddr: number = 0x01;
  const cmdSource = new Uint8Array([FieldBusAddr, 0xB0, 0x00]);
  const cmd: Array<number> = Array.from(appendCRC16toArray(cmdSource));
  const answer: any = await  COMx.getCOMAnswer({cmd});
  const msg: Array<number> = validateAnswer(answer);
  const buff: Array<number> = msg.slice(3,-2);
  const str = Buffer.from(buff).toString('ascii');
  const res: Array<TFlashSegmen> = JSON.parse(str);
  return res;
}

const BOOT_CMD_POSITION = 2;

async function eraseSpecifiedPages(ErasedPages:Array<string>): Promise<any> | never {
  console.log(ErasedPages);
  const FieldBusAddr: number = 0x01;
  const ErasedPagesArray: Uint8Array = ErasedPagesToU8Array(ErasedPages);
  const cmdSource = new Uint8Array([FieldBusAddr, 0xB0, 0x01, ...ErasedPagesArray]);
  const cmd: Array<number> = Array.from(appendCRC16toArray(cmdSource));
  const answer: any = await COMx.getCOMAnswer({cmd, 
                                               ChunksEndTime:100,
                                               timeOut:10000});
  /*TODO timeOut depends of: number of pages, size of page, time to erase*/
  const msg: Array<number> = validateAnswer(answer);
  //if (msg[BOOT_CMD_POSITION] != 1) throw new Error ('Wrong Answer');
  return msg;
}

async function readMem(U32Addr: number, U16Size: number): Promise<any> {
  const FieldBusAddr: number = 0x01;
  const cmdSource = new Uint8Array([FieldBusAddr, 0xB0, 0x03,
                                    ...U32ToU8Array(U32Addr),
                                    ...U16ToU8Array(U16Size)]);
  const cmd: Array<number> = Array.from(appendCRC16toArray(cmdSource))
  /*TODO It is necessary to calculate the chunkend time and timeout
         depending on the required data size and boudrate.*/
  const answer: any = await COMx.getCOMAnswer({cmd, 
                                               ChunksEndTime:100,
                                               timeOut:3800});
  const msg: Array<number> = validateAnswer(answer);
  return msg;
}

async function writeMem(U32Addr: number, buff: Uint8Array): Promise<any> {
  const FieldBusAddr: number = 0x01;
  const cmdSource = new Uint8Array([FieldBusAddr, 0xB0, 0x04,
                                    ...U16ToU8Array(buff.length),
                                    ...U32ToU8Array(U32Addr),
                                    ...buff]);
  const cmd: Array<number> = Array.from(appendCRC16toArray(cmdSource))
  /*TODO It is necessary to calculate the chunkend time and timeout
         depending on the required data size and boudrate.*/
  const answer: any = await COMx.getCOMAnswer({cmd, 
                                               ChunksEndTime:1,
                                               timeOut:3000});
  const msg: Array<number> = validateAnswer(answer);
  return msg;
}

async function startApplication(): Promise<any> {
  const FieldBusAddr: number = 0x01;
  const cmdSource = new Uint8Array([FieldBusAddr, 0xB0, 0x02]);
  const cmd: Array<number> = Array.from(appendCRC16toArray(cmdSource))
  await COMx.getCOMAnswer({cmd, ChunksEndTime:20,
                                timeOut:500,
                                NotRespond: true });
  return;
}

function validateAnswer(answer: any): Array<number> | never {
  if (!('msg' in answer)) throw new Error(`The Answer has no 'msg' field`);
  if (getCRC16(answer.msg) != 0) throw new Error(`The Answer CRC doesn't match`);
  return answer.msg;
}

function isItApplication(ID: string): boolean {
  let sa: Array<string> =  ID.split(' ');
  let name:string = sa[1] || '';
  return (name !== 'Bootloader');
}

async function downloadCodeToMCU(SkipSectors:Array<string>) {
  const skipArea: Array<number> = SkipSectors.map((value)=>parseInt(value));
  try {
    const chunkSize: number = 240;
    for (const area of Areas) {
      let StartAddr: number = parseInt(area.start);
      if (skipArea.includes(StartAddr)) continue;
      const CodeData:Array<number> = area.code;
      let count: number = Number((area.size !== 'main') ? area.size : 0);
      let idx: number = 0;
      while (count > 0) {
        let trcount: number = (chunkSize > count) ? count : chunkSize;
        count -= trcount;
        const code: Uint8Array = new Uint8Array(CodeData.slice(idx, idx+trcount));
        await writeMem(StartAddr, code);
        StartAddr +=trcount;
        idx += trcount;
      }
    }
    
  } catch (e) {
    console.log('write flash error', e);
  }
}

async function startBootloader(): Promise<any> {
  const FieldBusAddr: number = 0x01;
  const cmdSource = new Uint8Array([FieldBusAddr, 0xB0]);
  const cmd: Array<number> = Array.from(appendCRC16toArray(cmdSource))
  await COMx.getCOMAnswer({cmd, ChunksEndTime:20,
                                timeOut:500,
                                NotRespond: true });
  await delay(500);
  return;
}