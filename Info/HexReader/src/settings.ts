export const settings =  {
  App: "D:/Project/Projects/!SMFCB/DExSys/FW/CM4/DExS-SMFCB/EWARMv6/STM32F407IGT6/Exe/DExS.M4CPU.SMFCB.hex",
  SkipSectors:['0x080A0000', '0x080C0000', '0x080E0000'],
  COM:{
    port:"COM3",
  settings: {
        baudRate: 115200,
        dataBits: 8,
        parity: "none",
        stopBits: 1,
        flowControl: false
    }
  },
  resources: {
    start: "0x08080000",
    items: {
      SN : '12345678',
      ID : 'DExS.SMFCB v1.10.3.0 29.11.2021 MVK www.intmash.ru',
      INI : { file:'D:/Project/Projects/!SMFCB/DExSys/FW/CM4/DExS-SMFCB/Devices/DExS SMFCB 1_10_2.ini'},
    }
  }
}