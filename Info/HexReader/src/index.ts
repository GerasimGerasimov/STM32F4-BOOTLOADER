import fs = require ("fs");
import { getUsageMemoryAddresAndSize, TCodeSegment } from "./hex";
import { getFlashPages } from "./mcu";
//import path = require('path');
//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F405.hex').toString().split("\n");

const Areas: Array<TCodeSegment> = getUsageMemoryAddresAndSize(fileContent);

Areas.forEach((item)=>{console.log(JSON.stringify(item))});
//console.log(JSON.stringify(Areas, null, '\t'));
getFlashPages().forEach((item)=>{
  console.log(JSON.stringify(item));
});
console.log('Hex Reader has Done');
