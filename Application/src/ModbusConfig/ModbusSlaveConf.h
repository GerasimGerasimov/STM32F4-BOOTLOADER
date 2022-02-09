#ifndef MODBUSSLAVECONF_H
#define MODBUSSLAVECONF_H
   
  #ifdef __cplusplus
    extern "C" void ModbusClientInit(void);
    extern "C" void ModbusSlaveProc(void);
    extern "C" void RCMD_Rst_Timer_run(void);
    extern "C" void BootLoadCmdFillZero(void);
  #else
    extern void ModbusClientInit(void);
    extern void ModbusSlaveProc(void);
    extern void RCMD_Rst_Timer_run(void);
    extern void BootLoadCmdFillZero(void);
  #endif
#endif
