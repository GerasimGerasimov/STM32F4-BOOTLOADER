#ifndef STR_H
#define STR_H

  #ifdef __cplusplus
    extern "C" int getStrLenght(char * str);
    extern "C" void u8_mem_cpy( unsigned char *s, unsigned char *d, unsigned int c);
  #else
    extern int getStrLenght(char * str);
    extern void u8_mem_cpy( unsigned char *s, unsigned char *d, unsigned int c);
  #endif

#endif