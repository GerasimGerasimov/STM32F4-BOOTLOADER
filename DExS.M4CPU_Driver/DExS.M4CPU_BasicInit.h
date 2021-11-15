#ifndef BASIC_INIT_H
#define BASIC_INIT_H
#include "Intmash_bastypes.h" 

void GPIO_Configuration(void);
void NVIC_Configuration(void);
void BasicADCconfiguration(tU32 AinBufferAddr, tU32 AinOptBufferAddr, tU32 SamplingFrequency);
void SyncEXTIconfiguration(void);
void CommonTimerConfiguration(void);
void Watchdog_configuration(tU16 reset_time_ms);
void StartInit(void);

#endif