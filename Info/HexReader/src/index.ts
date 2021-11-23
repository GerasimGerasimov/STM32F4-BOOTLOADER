import fs = require ("fs");
import { getUsageMemoryAddresAndSize} from "./hex";
import { TFlashSegmen } from "./hextypes";
import { getErasedPages} from "./mcu";
//import path = require('path');
//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F405.hex').toString().split("\n");

const Areas: Array<TFlashSegmen> = getUsageMemoryAddresAndSize(fileContent);

Areas.forEach((item)=>{console.log(JSON.stringify(item))});

console.log(getErasedPages(Areas));

console.log('Hex Reader has Done');
