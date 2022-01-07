#ifndef BASTYPES_H
#define BASTYPES_H
#include "stm32f4xx.h"// basic types


typedef u8 *pu8;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef volatile float vf32;
typedef float f32;

typedef
  union bauint { // Byte-addressable UINT
     u16 i; // int: Var.i
     u8 b[2]; // u char: Var.b[0]:Var.b[1]
  } bauint;


typedef
  union bavu16 { // Byte-addressable UINT
     vu16 i; // int: Var.i
     vu8 b[2]; // u char: Var.b[0]:Var.b[1]
  } bavu16;

typedef
  union baulong { // Byte-addressable UINT
     u32  uL;    // 1 DWORD
     u16  i[2]; // 2 WORD
     u8   b[4]; // 4 BYTES
  } baulong;

typedef
  union bauqword { // Byte-addressable QWORD 64bit
    u8   b[8]; //8  BYTES
    u16  i[4]; //4  WORD
    u32  L[2]; //2 DWORD
    signed long long int Q; // QWORD
  }bauqword;
 
typedef 
  union biauint { // Byte-addressable UINT
     u16 i; // int: Var.i
     u8 b[2]; // u char: Var.b[0]:Var.b[1]
	 __packed struct {
	 	unsigned b0:1;
	 	unsigned b1:1;
	 	unsigned b2:1;
	 	unsigned b3:1;
	 	unsigned b4:1;
	 	unsigned b5:1;
	 	unsigned b6:1;
	 	unsigned b7:1;
	 	unsigned b8:1;
	 	unsigned b9:1;
	 	unsigned b10:1;
	 	unsigned b11:1;
	 	unsigned b12:1;
	 	unsigned b13:1;
	 	unsigned b14:1;
	 	unsigned b15:1;
	 }bits;
  } biauint;

typedef
  union biauchar { // Byte-addressable UINT
     u8 b;
	 struct {
	 	unsigned b0:1;
	 	unsigned b1:1;
	 	unsigned b2:1;
	 	unsigned b3:1;
	 	unsigned b4:1;
	 	unsigned b5:1;
	 	unsigned b6:1;
	 	unsigned b7:1;
	 }bits;
  } biauchar;

typedef struct
{
  u16 test_mass[128];
} TSRAM_DATA;

#endif