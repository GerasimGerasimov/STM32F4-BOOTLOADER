import fs = require ("fs");
import { getUsageMemoryAddresAndSize} from "./hex";
import { TFlashSegmen } from "./hextypes";
import { getErasedPages} from "./mcu";

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/hex-samples/stm32F405.hex').toString().split("\n");

const Areas: Array<TFlashSegmen> = getUsageMemoryAddresAndSize(fileContent);

Areas.forEach((item)=>{console.log(JSON.stringify(item))});

console.log(getErasedPages(Areas));

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
