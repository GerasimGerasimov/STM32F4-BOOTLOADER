import fs = require ("fs");
//import path = require('path');

//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F4.hex').toString().split("\n");
console.log(fileContent);
console.log('Stop Hex Reader');

function getMemoryAreas(content:Array<string>) {
  content.forEach(element => {
    console.debug(element);
  });
}

getMemoryAreas(fileContent);