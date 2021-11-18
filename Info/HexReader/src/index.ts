import fs = require ("fs");
//import path = require('path');

//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F4-short.hex').toString().split("\n");

type TArea = {
  from: number;
  size: number
}

function getMemoryAreas(content:Array<string>) {
  var Area: TArea = {from: 0, size: 0};
  const Areas: Array<TArea> = [];
  var segmentAddr: number = 0;
  var PrevOffset: number = 0;
  var segSize: number = 0;
  for (const idx in content) {
    const hexstr: string = content[idx];
    let HexSrtLen: number = getHexSrtLenght(hexstr);
    if (HexSrtLen) {
      const {isAddition, Addr, hexAddr} = isAdditionSegmentAddress(hexstr);
      if (isAddition) {
        segmentAddr = Addr;
      } else {
        const {Addr, size} = getStartAddrAndSizeOfCodeStr(HexSrtLen, hexstr, segmentAddr);
        
        if ((Addr - segSize) > PrevOffset){
          console.log(`0x${(PrevOffset).toString(16)} : `, segSize);
          PrevOffset = Addr;
          segSize = size;
          continue;
        } else {
          segSize += size;
        }
      }
    } else {
      if (isEndOfHex(hexstr)) {
        console.log(`0x${(PrevOffset).toString(16)} : `, segSize);
        console.log('End Of Hex', hexstr);
        break;
      }
    }
  };
}

function getStrFirstAddr(str: string, Addition: number): number {
  const hsaddr: string =  `0x${str.slice(3,7)}`;
  const addr: number = Addition + parseInt(hsaddr);
  return addr;
}

//:10 0070 00 E90B0108ED0B0108F10B0108F50B0108 74
function getStartAddrAndSizeOfCodeStr(size: number, str: string, Addition: number): {Addr: number, size: number} {
  const FirstAddr: number = getStrFirstAddr(str, Addition);
  return {Addr: FirstAddr, size};
}

//:020000040800F2
function isAdditionSegmentAddress(str: string): {isAddition: boolean, Addr: number, hexAddr:string}{
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

//:00000001FF
function isEndOfHex(str: string): boolean {
  const code: string = str.slice(7,9);
  return (code == '01');
}

getMemoryAreas(fileContent);

console.log('Hex Reader has Done');