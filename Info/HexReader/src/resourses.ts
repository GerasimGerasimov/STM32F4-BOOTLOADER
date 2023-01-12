import { TFlashSegmen } from "./hextypes";
import fs = require ("fs");
import { U16ToU8Array, U16ToU8ArrayLE, U32ToU8Array, U32ToU8ArrayLE } from "./mcu";
import { getCRC16 } from "./crc/crc16";
/*TODO
Result is structure from StartAddres:
  #Header
  u16 Size of Resources record
  u16 Number of Items in Resources
  #Resource Table
    [
      [ u32 Addr of Item_0
        u32 Size of Item_0
        [14] Name of Item_0, null-terminated string, not more than 15 simbols.
        u16 crc
      ],
      [ u32 Addr of Item_1
        u32 Size of Item_1
        [14] Name of Item_1, null-terminated string, not more than 15 simbols.
        u16 crc
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
  Addr: Uint8Array;
  SizeOfData: number = 0;
  Name: string = '';
  CRC: Uint8Array;
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

/*TODO size of resources may be over of 64K, must be use 32-bit value here*/
const SizeOfFieldSizeofResourcesRecord: number = 2;
const SIZE_OF_TOTAL_NUMBER_OF_ITEMS: number = 2;
const SIZE_OF_TOTAL_CRC: number = 2;

const SIZE_FIELD_OF_RESOURCE_ADDR: number = 4;
const SIZE_FILED_OF_RESOURCE_SIZE: number = 4;
const SIZE_FIELD_OF_RESOURCE_NAME: number = 14; 
const SIZE_FIELD_OF_RESOURCE_CRC: number = 2;
//the sum of the bytes is a multiple of 4 !!!
const SIZE_OF_RESOURCE_TABLE_ITEM = 
                          SIZE_FIELD_OF_RESOURCE_ADDR +
                            SIZE_FILED_OF_RESOURCE_SIZE +
                              SIZE_FIELD_OF_RESOURCE_NAME + 
                                SIZE_FIELD_OF_RESOURCE_CRC;

export function getResourses(resources: any):Array<TFlashSegmen> {
  const Resources: TResourcesData =  new TResourcesData();
  if (!isResourcesValid(resources)) return [];
  const {items, start} = resources;
  const src: Array<TResourcePropsAndData> = getResoursesData(items);

  /*TODO так как в таблице ресурсов теперь указывается смещение ресурсу относительно Root а не абсолютный адрес,
  то надо это изменение распространить на примеры App и Bootloader*/
  const AddrOfResourceTable: number = SizeOfFieldSizeofResourcesRecord + //убрал start чтобы данные адресовались от начала таблицы
                                          SIZE_OF_TOTAL_NUMBER_OF_ITEMS;
  const StartAddrOfBinaryData: number = AddrOfResourceTable + 
                                          getSizeOfResourcesTable(src);

  calculateAddressOfData(src, StartAddrOfBinaryData);
  Resources.Size = getResoursesSize(src);
  Resources.NumberOfItems = src.length;
  Resources.Table = getResoursesTable(src); 
  Resources.BinaryData = getBinaryData(src);
  const Binary: Uint8Array = convertResourcesToBinary(Resources);
  const result: Array<TFlashSegmen> = getFlashAreaFromBinaryResource(Binary, start, Resources);
  return result;
}

function getFlashAreaFromBinaryResource(bin: Uint8Array, startAddr: string, Resources: TResourcesData): Array<TFlashSegmen> {
  const FlashSegmen: TFlashSegmen = {
    start:startAddr,
    size: bin.length,
    code:[...bin]
  }
  return [FlashSegmen];
}

function convertResourcesToBinary(Resources: TResourcesData):Uint8Array {
  const src: Uint8Array = new Uint8Array([
    ...U16ToU8ArrayLE(Resources.Size),
    ...U16ToU8ArrayLE(Resources.NumberOfItems),
    ...resourceTableToBinary(Resources.Table),
    ...resourceDataToBinary(Resources.BinaryData)
  ]);
  const res: Uint8Array = new Uint8Array([
    ...src,
    ...U16ToU8Array(getCRC16(src))
  ])
  return res;
}

function resourceDataToBinary(BinaryData: Array<number>): Uint8Array {
  const res: Uint8Array = new Uint8Array(BinaryData);
  return res;
}

function resourceTableToBinary(Table: Array<TResourceProps>): Uint8Array {
  const res: Uint8Array = new Uint8Array(Table.length * SIZE_OF_RESOURCE_TABLE_ITEM);
  let offset: number = 0;
  Table.forEach((item)=> {
    let bin: Uint8Array = new Uint8Array([
      ...item.Addr,
      ...U32ToU8ArrayLE(item.SizeOfData),
      ...NullTermStrToU8Array(item.Name, SIZE_FIELD_OF_RESOURCE_NAME),
      ...item.CRC
    ]);
    res.set([...bin], offset);
    offset += SIZE_OF_RESOURCE_TABLE_ITEM;
  });
  return res;
}

function NullTermStrToU8Array(text: string, arrSize: number): Uint8Array {
  const res: Uint8Array = new Uint8Array(arrSize).fill(0);
  const str = Uint8Array.from(text, c => c.charCodeAt(0));
  if (str.length > (SIZE_FIELD_OF_RESOURCE_NAME-1)) throw new Error (`Name of Resource could not be above 15 symbols ${text}`)
  /*TODO export text and set zero after the text*/
  str.forEach((value, index)=>{
    res[index] = value;
  })
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
    let bin: Uint8Array = new Uint8Array([
      ...item.Addr,
      ...U32ToU8ArrayLE(item.SizeOfData),
      ...NullTermStrToU8Array(item.Name, SIZE_FIELD_OF_RESOURCE_NAME)
    ]);
    let CRC: Uint8Array = U16ToU8Array(getCRC16(bin));
    return {Addr, SizeOfData, Name, CRC};
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
  size += SIZE_OF_TOTAL_NUMBER_OF_ITEMS;
  size += SIZE_OF_TOTAL_CRC;
  return size;
}

function getSizeOfResourcesTable(src: Array<TResourcePropsAndData>): number {
  return src.length * SIZE_OF_RESOURCE_TABLE_ITEM;
}

function calculateAddressOfData(src: Array<TResourcePropsAndData>, StartAddr: number) {
  var startAddr: number = StartAddr;
  src.forEach((item)=>{
    item.Addr = U32ToU8ArrayLE(startAddr);
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