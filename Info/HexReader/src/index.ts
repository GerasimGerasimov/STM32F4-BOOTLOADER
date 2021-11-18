import fs = require ("fs");
//import path = require('path');

//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F4.hex').toString().split("\n");

function getMemoryAreas(content:Array<string>) {
  var segmentAddr: number = 0;
  for (const idx in content) {
    const hexstr: string = content[idx];
    if (getHexSrtLenght(hexstr)) {
      const {isAddition, Addr, hexAddr} = isAdditionSegmentAddress(hexstr);
      if (isAddition) {
        segmentAddr = Addr;
        console.log(hexAddr);
      } else {

      }
    } else {
      if (isEndOfHex(hexstr)) {
        console.log('End Of Hex', hexstr);
        break;
      }
    }
  };
}

function isAdditionSegmentAddress(str: string): {isAddition: boolean, Addr: number, hexAddr:string}{
  //:020000040800F2
  const code: string = str.slice(7,9);
  return (code == '04')
         ? {isAddition: true, Addr:parseInt(`0x${str.slice(9,13)}0000`), hexAddr:`0x${str.slice(9,13)}0000`}
         : {isAddition: false, Addr:0, hexAddr:'0x00000000'};
}

function getHexSrtLenght(str: string): number {
  const hexStr: string = `0x${str.slice(1,3)}`;
  const len: number = parseInt(hexStr);
  return len;
}

function isEndOfHex(str: string): boolean {
  const code: string = str.slice(7,9);
  return (code == '01');
}

getMemoryAreas(fileContent);

console.log('Hex Reader has Done');