import fs = require ("fs");
//import path = require('path');

//const dir: string = path.resolve(__dirname,'');

console.log('Start Hex Reader');
const fileContent: Array<string> = fs.readFileSync('./src/stm32F4-short.hex').toString().split("\n");

type TArea = {
  from: string;
  size: number
}

class TAreaProps {
  PrevOffset: number = 0;
  segSize: number = 0;

  public getLastPosition(): TArea {
    console.log(`0x${(this.PrevOffset).toString(16)} : ${this.segSize}`);
    return {
      from: `0x${(this.PrevOffset).toString(16)}`,
      size: this.segSize
    }
  }
}

const Areas: Array<TArea> = [];

function getMemoryAreas(content:Array<string>) {
  const segmentAddr = {addr: 0};
  const Area: TAreaProps = new TAreaProps();
  for (const idx in content) {
    const hexstr: string = content[idx];
    let HexSrtLen: number = getHexSrtLenght(hexstr);
    if (HexSrtLen) {
      if (isAdditionSegmentAddress(segmentAddr, hexstr)) continue;
      const {Addr, size} = getStartAddrAndSizeOfCodeStr(HexSrtLen, hexstr, segmentAddr.addr);
      const NewArea: TArea = isNewArea(Area, Addr, size);
      if (NewArea) Areas.push(NewArea);
    } else {
      if (isEndOfHex(hexstr)) {
        Areas.push(Area.getLastPosition());
        console.log('End Of Hex');
        break;
      }
    }
  };
}

function isNewArea(Area:{ PrevOffset: number,  segSize: number}, addr: number, size: number): TArea | undefined {
  var newArea: TArea = undefined;
  if ((addr - Area.segSize) > Area.PrevOffset){
    if (Area.segSize !== 0) {
      newArea = {
        from: `0x${(Area.PrevOffset).toString(16)}`,
        size: Area.segSize
      };
      console.log(`${newArea.from} : ${newArea.size}`);
    }
    Area.PrevOffset = addr;
    Area.segSize    = size;
    return newArea;
  } else {
    Area.segSize += size;
    return undefined;
  }
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
function isAdditionSegmentAddress(seg: {addr: number}, str: string): boolean {
  const code: string = str.slice(7,9);
  return (code == '04')
         ? (seg.addr = parseInt(`0x${str.slice(9,13)}0000`), true)
         : false;
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