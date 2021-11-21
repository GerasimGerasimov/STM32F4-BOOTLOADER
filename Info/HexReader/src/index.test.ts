/*
import { getErasedPages } from "./mcu";

import { equal } from "assert";

describe('function test', function() {
  it ('should get erased pages', function () {
    equal(getErasedPages(
      [
        {start:"0x08000000", size: 61368},
        {start:"0x080a0000", size: 104},
        {start:"0x080c0000", size: 104},
        {start:"0x0800DD81", size: "main"},
        {start:"0x080e0000", size: 4}
      ])
      ,[
        {start: '0x08000000', size: 0x03FFF},//16K 3FFF
        {start: '0x08004000', size: 0x03FFF},//16K 3FFF
        {start: '0x08008000', size: 0x03FFF},//16K 3FFF
        {start: '0x0800C000', size: 0x03FFF},//16K 2FBB
        {start: '0x080A0000', size: 0x1FFFF},//128K
        {start: '0x080C0000', size: 0x1FFFF},//128K
        {start: '0x080E0000', size: 0x1FFFF} //128K
      ]);
  });

});
*/