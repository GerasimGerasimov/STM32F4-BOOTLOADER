import fs = require ("fs");
import ComPort from "./comport/comport";
import { appendCRC16toArray, getCRC16 } from "./crc/crc16";
import { getUsageMemoryAddresAndSize} from "./hex";
import { TFlashSegmen } from "./hextypes";
import { ErasedPagesToU8Array, getErasedPages, getFlashPages, U16ToU8Array, U32ToU8Array} from "./mcu";
import { settings } from "./settings";
import { delay } from "./utils/delay";

const COMx: ComPort = new ComPort(settings.COM);

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/hex-samples/stm32F405.hex').toString().split("\n");
const Areas: Array<TFlashSegmen> = getUsageMemoryAddresAndSize(fileContent);
//Areas.forEach((item)=>{console.log(JSON.stringify(item))});
console.log(getErasedPages(Areas, getFlashPages()));
console.log('!');

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

async function arePagesWereErased(ErasedPages:Array<string>): Promise<any> | never {
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

function validateAnswer(answer: any): Array<number> | never {
  if (!('msg' in answer)) throw new Error(`The Answer has no 'msg' field`);
  if (getCRC16(answer.msg) != 0) throw new Error(`The Answer CRC doesn't match`);
  return answer.msg;
}

(async () => { 
  while (true) {
    try {
      let s: string;
      const ID: string = await getID();
      const AvailiblePages: Array<TFlashSegmen> = await getAvailablePagesList();
      const ErasedPages:Array<string> = getErasedPages(Areas, AvailiblePages);
      console.log(ErasedPages);
      const mem_after: any = await readMem(0x080C0000, 0x0100);
      console.log(Buffer.from(mem_after).toString('ascii'));
      await arePagesWereErased(['0x08060000','0x08080000','0x080A0000', '0x080C0000']);//ErasedPages
      const mem: any = await readMem(0x080C0000, 0x0100);//0x4000);
      console.log(Buffer.from(mem).toString('ascii'));
    } catch (e) {0x080C0000
      await delay(1000);
      console.log('главЛовушка',e);
    }

  }
})();


/**TODO Establish connect to Device:
        Wait For Flash Pages List*/
/**TODO Send list of erased Pages*/
/**TODO Wait an Answer, the Pages has been Erased*/
/**TODO Loop of Write Areas
 * 1) Get the Area (for instatce{"start":"0x8000000","size":61368}, {"start":"0x80a0000","size":104} ...)
 * 2) Send to Device StartAddres and Size of Area ("start":"0x8000000","size":61368 for instatce)
 * 3) Wait for Aswer: Availible spase in BIG RAM Buffer
    * 4) Send content of Area to BIG RAM bufer of Device, chunk by chunk
    *   * size of chunk is "Availible spase" if possible
          IF (rest of memory spase below than next chunk size) OR (last chunk of Area) {
            a) Send "FlushBuffer" command to write accumulated data to Flash
            b) Wait for answer, Flash has been writed
                (The Device flush the RAM buffer, and store last offset in Flash memory for new chunks)
          }
          IF (last chunk of Area) return to point (1)
*/

console.log('Hex Reader has Done');

/*
(async () => { 
  while (true) {
    try {
      await Tagger.open();//открыть соединение и получить ClientID
      await devicesInfoStore.getDevicesInfo();//получить инфу об устройствах
      devicesValueStore.createTasks(Triggers.getRequests());//разбить на задачи для чтения
      while (true) {
        //TODO надо чтобы на переконнект с WS вываливался только если Fetch Error
        for await (let i of devicesValueStore.asyncGenerator()) {
          const written: boolean = await DBWritter.write(doTriggers(Triggers));
          if (written) {
            WSS.sendNotificationAfter(3000);
          }
        }
      }

    } catch (e) {
      console.log('главЛовушка',e);
      await delay(5000);//подождал 3 а потом закрыл соединение
      await Tagger.close();//закрыть соединение
      //devicesValueStore.clearTasks()
    }

  }
})();
*/