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

  public getNewArea(addr: number, size: number): TArea | undefined {
    var newArea: TArea = undefined;
    if ((addr - this.segSize) > this.PrevOffset){
      if (this.segSize !== 0) {
        newArea = {
          from: `0x${(this.PrevOffset).toString(16)}`,
          size: this.segSize
        };
        console.log(`${newArea.from} : ${newArea.size}`);
      }
      this.PrevOffset = addr;
      this.segSize    = size;
      return newArea;
    } else {
      this.segSize += size;
      return undefined;
    }
  }
}

function getMemoryAreas(content:Array<string>): Array<TArea> {
  var segmentAddr = 0;
  const Area: TAreaProps = new TAreaProps();
  const res:  Array<TArea> = [];
  for (const idx in content) {
    const hexstr: string = content[idx];
    const HexSrtLen: number = getHexSrtLenght(hexstr);
    switch (getCommand(hexstr)) {
      case '04':
        segmentAddr = getAdditionSegmentAddress(hexstr);
        break;
      case '00':
        const {Addr, size} = getStartAddrAndSizeOfCodeStr(HexSrtLen, hexstr, segmentAddr);
        const NewArea: TArea = Area.getNewArea(Addr, size);
        if (NewArea) res.push(NewArea);
        break;
      case '01':
        res.push(Area.getLastPosition());
        console.log('End Of Hex');
        break;
    }
  }
  return res;
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
function getAdditionSegmentAddress(str: string): number {
  return parseInt(`0x${str.slice(9,13)}0000`)
}

function getHexSrtLenght(str: string): number {
  const hexStr: string = `0x${str.slice(1,3)}`;
  const len: number = parseInt(hexStr);
  return len;
}

function getCommand(str: string): string{
  return str.slice(7,9);
}

const Areas: Array<TArea> = getMemoryAreas(fileContent);
console.log(Areas);

console.log('Hex Reader has Done');