import { TFlashSegmen } from "./hextypes";
import fs = require ("fs");
import { U16ToU8Array, U32ToU8Array } from "./mcu";
/*TODO
Result is structure from StartAddres:
  #Header
  u16 Size of Resources record
  u16 Number of Items in Resources
  #Resource Table
    [
      [ u32 Addr of Item_0
        u32 Size of Item_0
        [16] Name of Item_0, null-terminated string, not more than 15 simbols.
      ],
      [ u32 Addr of Item_1
        u32 Size of Item_1
        [16] Name of Item_1, null-terminated string, not more than 15 simbols.
      ]
    ]
  #Control
  u16 CRC16 of Resources record
  #Binary Data of Resources:
      '12345678',
      'DExS.SMFCB v1.10.3.0 29.11.2021 MVK www.intmash.ru',
      content of file,
      ...
*/

class TResourceProps {
  Addr: number = 0;
  SizeOfData: number = 0;
  Name: string = '';
}

class TResourcePropsAndData extends TResourceProps {
  Data: Array<number> = [];
}

class TResourcesData {
  Size: number = 0;
  NumberOfItems: number = 0;
  Table: Array<TResourceProps> = [];
  BinaryData: Array<number> = [];
  CRC16: number;
}

const SizeOfFieldSizeofResourcesRecord: number = 2;
const SizeOfFieldNumberOfItems: number = 2;

export function getResourses(resources: any):Array<TFlashSegmen> {
  const result: Array<TFlashSegmen> = [];
  const Resources: TResourcesData =  new TResourcesData();
  //return result;
  if (!isResourcesValid(resources)) return result;
  const {items, start} = resources;
  const src: Array<TResourcePropsAndData> = getResoursesData(items);

  const AddrOfResourceTable: number = parseInt(start) +
                                        SizeOfFieldSizeofResourcesRecord +
                                          SizeOfFieldNumberOfItems;
  const StartAddrOfBinaryData: number = AddrOfResourceTable + 
                                          getSizeOfResourcesTable(src);

  calculateAddressOfData(src, StartAddrOfBinaryData);
  Resources.Size = getResoursesSize(src);
  Resources.NumberOfItems = src.length;
  Resources.Table = getResoursesTable(src); 
  Resources.BinaryData = getBinaryData(src);
  return result;
}

/*
  const cmdSource = new Uint8Array([FieldBusAddr, 0xB0, 0x04,
    ...U16ToU8Array(buff.length),
    ...U32ToU8Array(U32Addr),
    ...buff]);
*/
function convertResourcesToBinary(Resources: TResourcesData):Uint8Array {
  const res: Uint8Array = new Uint8Array([
    ...U16ToU8Array(Resources.Size),
    ...U16ToU8Array(Resources.NumberOfItems),
    ...resourceTableToBinary(Resources.Table)
  ]);//+2 for CRC16

  return res;
}

function resourceTableToBinary(Table: Array<TResourceProps>): Uint8Array {
  const AddrSize: number = 4;
  const SizeOfDataSize: number = 4;
  const SizeOfName: number = 16;
  const itemOfTableSize = AddrSize + SizeOfDataSize + SizeOfName;
  const res: Uint8Array = new Uint8Array(Table.length * itemOfTableSize);
  Table.forEach((item)=> {
    let itemBinary: Uint8Array = new Uint8Array([
      ...U32ToU8Array(item.Addr),
      ...U32ToU8Array(item.SizeOfData),
      ...NullTermStrToU8Array(item.Name, 16)
    ])
  });
  return res;
}

function NullTermStrToU8Array(text: string, arrSize: number): Uint8Array {
  const res: Uint8Array = new Uint8Array(arrSize);
  const str = Uint8Array.from(text, c => c.charCodeAt(0))
  /*TODO export text and set zero after the text*/
  res = [...str];
  return res;
}

function getBinaryData(src: Array<TResourcePropsAndData>):Array<number> {
  const res: Array<number> = src.reduce((acc, item)=>{
    acc = [...acc, ...item.Data];
    return acc;
  },[])
  return res;
}

function getResoursesTable(src: Array<TResourcePropsAndData>): Array<TResourceProps> {
  const res: Array<TResourceProps> = src.map((item)=>{
    let {Addr, SizeOfData, Name} = {...item};
    return {Addr, SizeOfData, Name};
  })
  return res;
}

function getResoursesSize(src: Array<TResourcePropsAndData>): number {
  var size: number = 0;
  src.forEach((item)=>{
    size += item.SizeOfData;
  });
  size += getSizeOfResourcesTable(src);
  size += SizeOfFieldSizeofResourcesRecord;
  size +=  SizeOfFieldNumberOfItems;
  return size;
}

function getSizeOfResourcesTable(src: Array<TResourcePropsAndData>): number {
  const SizeOfAddr: number = 4;
  const SizeOfSize: number = 4;
  const SizeOfNameString: number = 16;
  const OneRecordSize: number = SizeOfAddr + SizeOfSize + SizeOfNameString ;
  return src.length * OneRecordSize;
}

function calculateAddressOfData(src: Array<TResourcePropsAndData>, StartAddr: number) {
  var startAddr: number = StartAddr;
  src.forEach((item)=>{
    item.Addr = startAddr;
    startAddr += item.SizeOfData;
  });
}

function getResoursesData(items: any):Array<TResourcePropsAndData> {
  const res: Array<TResourcePropsAndData> = [];
  for (const name in items) {
    console.log(name);
    const resource: TResourcePropsAndData = getResourceProperties(name, items);
    res.push(resource);
  }
  return res;
}

function getResourceProperties(name: string, items: any): TResourcePropsAndData {
  const res: TResourcePropsAndData = new TResourcePropsAndData();
  const item: any = items[name];
  const data: Array<number> = getDataByType(item);
  res.Name = name;
  res.SizeOfData = data.length;
  res.Data = [...data];
  return res;
}

function getDataByType(item: any): Array<number> {
  const res: Array<number> = [];
  
  if (typeof item === 'string') {
    let a = Buffer.from(item, 'utf8');
    return [...a];
  }

  if (typeof item === 'number') {
    let s = item.toString();
    let a = Buffer.from(s, 'utf8');
    return [...a];
  }

  if (typeof item === 'object') {
    if ('file' in item) {
      let filename: string = item.file;
      let a = fs.readFileSync(filename);
      return [...a];
    }
  }
  return res;
}

function isResourcesValid(resources: any): boolean {
  if (resources === undefined) return false;
  if (!('start' in resources)) return false;
  if (!('items' in resources)) return false;
  return true;
}