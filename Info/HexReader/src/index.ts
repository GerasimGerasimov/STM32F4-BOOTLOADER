import fs = require ("fs");
import { getUsageMemoryAddresAndSize} from "./hex";
import { TFlashSegmen } from "./hextypes";
import { getErasedPages, getFlashPages } from "./mcu";
//import path = require('path');
//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F405.hex').toString().split("\n");

const Areas: Array<TFlashSegmen> = getUsageMemoryAddresAndSize(fileContent);

Areas.forEach((item)=>{console.log(JSON.stringify(item))});
//console.log(JSON.stringify(Areas, null, '\t'));
getFlashPages().forEach((item)=>{
  console.log(JSON.stringify(item));
});

console.log(getErasedPages(
  [
    {start:"0x08000000", size: 61368},
    {start:"0x080a0000", size: 104},
    {start:"0x080c0000", size: 104},
    {start:"0x0800DD81", size: "main"},
    {start:"0x080e0000", size: 4}
  ]));

console.log('Hex Reader has Done');
