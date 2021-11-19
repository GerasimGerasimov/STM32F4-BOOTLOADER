import fs = require ("fs");
import { getUsageMemoryAddresAndSize, THEXArea } from "./hex";
//import path = require('path');
//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F4.hex').toString().split("\n");

const Areas: Array<THEXArea> = getUsageMemoryAddresAndSize(fileContent);

Areas.forEach((item)=>{console.log(JSON.stringify(item))});
//console.log(JSON.stringify(Areas, null, '\t'));

console.log('Hex Reader has Done');