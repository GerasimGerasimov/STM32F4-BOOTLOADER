#ifndef INIT_H
#define INIT_H

  #ifdef __cplusplus
    extern "C" void Init(void);
    extern "C" void GPIO_INIT_Configuration(void);
  #else
    extern void Init(void);
    extern void GPIO_INIT_Configuration(void);    
  #endif
#endif