/**
 * @file Intmash_bastypes.h
 * @brief 
 * @details 
 * @date Created on 12.04.2016
 * @copyright 2009-2016 Intmash Ltd. <info@intmash.ru>
 * @author 
 * 

 */

#ifndef INTMASH_BASTYPES_H_
#define INTMASH_BASTYPES_H_

/*
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
#include <string>
#endif
*/

typedef unsigned char tU8;
typedef signed char tS8;

typedef unsigned short int tU16;
typedef signed short int tS16;

typedef unsigned int tU32;
typedef signed int tS32;

typedef unsigned long long int tU64;
typedef signed long long int tS64;

typedef volatile float tvF32;
typedef float tF32;

//либо так использовать, либо каждый раз где нужно руками подписывать volatile
typedef volatile unsigned char tvU8;
typedef volatile signed char tvS8;
typedef volatile unsigned short int tvU16;
typedef volatile signed short int tvS16;
typedef volatile unsigned int tvU32;
typedef volatile signed int tvS32;
typedef volatile unsigned long long int tvU64;
typedef volatile signed long long int tvS64;



typedef struct _tBits8
{
	tU8 B0 :1;
	tU8 B1 :1;
	tU8 B2 :1;
	tU8 B3 :1;
	tU8 B4 :1;
	tU8 B5 :1;
	tU8 B6 :1;
	tU8 B7 :1;
} tBits8;

typedef struct _tBits16
{
	tU16 B0 :1;
	tU16 B1 :1;
	tU16 B2 :1;
	tU16 B3 :1;
	tU16 B4 :1;
	tU16 B5 :1;
	tU16 B6 :1;
	tU16 B7 :1;
	tU16 B8 :1;
	tU16 B9 :1;
	tU16 B10 :1;
	tU16 B11 :1;
	tU16 B12 :1;
	tU16 B13 :1;
	tU16 B14 :1;
	tU16 B15 :1;
} tBits16;

typedef struct _tBits32
{
	tU32 B0 :1;
	tU32 B1 :1;
	tU32 B2 :1;
	tU32 B3 :1;
	tU32 B4 :1;
	tU32 B5 :1;
	tU32 B6 :1;
	tU32 B7 :1;
	tU32 B8 :1;
	tU32 B9 :1;
	tU32 B10 :1;
	tU32 B11 :1;
	tU32 B12 :1;
	tU32 B13 :1;
	tU32 B14 :1;
	tU32 B15 :1;
	tU32 B16 :1;
	tU32 B17 :1;
	tU32 B18 :1;
	tU32 B19 :1;
	tU32 B20 :1;
	tU32 B21 :1;
	tU32 B22 :1;
	tU32 B23 :1;
	tU32 B24 :1;
	tU32 B25 :1;
	tU32 B26 :1;
	tU32 B27 :1;
	tU32 B28 :1;
	tU32 B29 :1;
	tU32 B30 :1;
	tU32 B31 :1;
} tBits32;

typedef struct _tBits64
{
	tU64 B0 :1;
	tU64 B1 :1;
	tU64 B2 :1;
	tU64 B3 :1;
	tU64 B4 :1;
	tU64 B5 :1;
	tU64 B6 :1;
	tU64 B7 :1;
	tU64 B8 :1;
	tU64 B9 :1;
	tU64 B10 :1;
	tU64 B11 :1;
	tU64 B12 :1;
	tU64 B13 :1;
	tU64 B14 :1;
	tU64 B15 :1;
	tU64 B16 :1;
	tU64 B17 :1;
	tU64 B18 :1;
	tU64 B19 :1;
	tU64 B20 :1;
	tU64 B21 :1;
	tU64 B22 :1;
	tU64 B23 :1;
	tU64 B24 :1;
	tU64 B25 :1;
	tU64 B26 :1;
	tU64 B27 :1;
	tU64 B28 :1;
	tU64 B29 :1;
	tU64 B30 :1;
	tU64 B31 :1;
	tU64 B32 :1;
	tU64 B33 :1;
	tU64 B34 :1;
	tU64 B35 :1;
	tU64 B36 :1;
	tU64 B37 :1;
	tU64 B38 :1;
	tU64 B39 :1;
	tU64 B40 :1;
	tU64 B41 :1;
	tU64 B42 :1;
	tU64 B43 :1;
	tU64 B44 :1;
	tU64 B45 :1;
	tU64 B46 :1;
	tU64 B47 :1;
	tU64 B48 :1;
	tU64 B49 :1;
	tU64 B50 :1;
	tU64 B51 :1;
	tU64 B52 :1;
	tU64 B53 :1;
	tU64 B54 :1;
	tU64 B55 :1;
	tU64 B56 :1;
	tU64 B57 :1;
	tU64 B58 :1;
	tU64 B59 :1;
	tU64 B60 :1;
	tU64 B61 :1;
	tU64 B62 :1;
	tU64 B63 :1;
} tBits64;

typedef union _tU8Union
{
	tU8 I;
	tBits8 Bits;
} tU8Union;

typedef union _tS8Union
{
	tS8 I;
	tBits8 Bits;
} tS8Union;

/**
 * @brief Struct for unsigned short int.
 * @details In case Little Endian
 */
typedef struct _tU16Struct
{
	tU8Union Low;
	tU8Union High;
} tU16Struct;

/**
 * @brief Struct for signed short int.
 * @details In case Little Endian
 */
typedef struct _tS16Struct
{
	tS8Union Low;
	tS8Union High;
} tS16Struct;

/**
 * @brief Union for unsigned short int.
 * In case Little Endian
 * Byte-addressable unsigned short
 * i = 1234
 * b[0] = 34
 * b[1] = 12
 * [1][0]
 *
 * i = 12345678 => i = 5678
 * b[0] = 78
 * b[1] = 56
 * b[2] = 0
 * b[3] = 0
 * [3][2][1][0]
 */
typedef union _tU16Union
{
	tU16 I;

	/**
	 * @brief Bytes view.
	 * @details B[0] - младший; B[1] - старший
	 */
	tU8Union B_Union[2];
	tU8 B[2];

	tBits16 Bits;

	tU16Struct U16;
} tU16Union;

/**
 * @brief Union for signed short int.
 * In case Little Endian
 * Byte-addressable unsigned short
 * i = 1234
 * b[0] = 34
 * b[1] = 12
 * [1][0]
 *
 * i = 12345678 => i = 5678
 * b[0] = 78
 * b[1] = 56
 * b[2] = 0
 * b[3] = 0
 * [3][2][1][0]
 */
typedef union _tS16Union
{
	tS16 I;

	/**
	 * @brief Bytes view.
	 * @details B[0] - младший; B[1] - старший
	 */
	tU8Union B_Union[2];
	tU8 B[2];

	tBits16 Bits;

	tS16Struct S16;
} tS16Union;

/**
 * @brief Struct for unsigned int.
 * @details In case Little Endian
 */
typedef struct _tU32Struct
{
	tU16Union Low;
	tU16Union High;
} tU32Struct;

/**
 * @brief Struct for signed int.
 * @details In case Little Endian
 */
typedef struct _tS32Struct
{
	tS16Union Low;
	tS16Union High;
} tS32Struct;

/**
 * @brief Union for unsigned int.
 * In case Little Endian
 * Byte-addressable unsigned int
 * i = 12345678
 * b[0] = 78
 * b[1] = 56
 * b[2] = 34
 * b[3] = 12
 * [3][2][1][0]
 * Для пересылки по Modbus делаем так: [1][0][3][2]. То есть сперва младший регистр, потом старший
 */
typedef union _tU32Union
{
	float Float;
	tU32 I;

	/**
	 * @brief Unsigned short int view.
	 * @details B[0] - младший; B[1] - старший
	 */
	tU16Union U16_Union[2];
	tU16 U16[2];

	tU8Union B_Union[4];
	tU8 B[4];

	tBits32 Bits;

	tU32Struct U32;
} tU32Union;

/**
 * @brief Union for signed int.
 * In case Little Endian
 * Byte-addressable unsigned int
 * i = 12345678
 * b[0] = 78
 * b[1] = 56
 * b[2] = 34
 * b[3] = 12
 * [3][2][1][0]
 * Для пересылки по Modbus делаем так: [1][0][3][2]. То есть сперва младший регистр, потом старший
 */
typedef union _tS32Union
{
	float Float;
	tS32 I;

	/**
	 * @brief Unsigned short int view.
	 * @details B[0] - младший; B[1] - старший
	 */
	tS16Union S16_Union[2];
	tS16 S16[2];

	tU8Union B_Union[4];
	tU8 B[4];

	tBits32 Bits;

	tS32Struct S32;
} tS32Union;

typedef union _tU64Union
{
	double Double;
	tU64 U64;

	tU32Union U32_Union[2];
	tU32 U32[2];

	tU16Union U16_Union[4];
	tU16 U16[4];

	tU8Union U8_Union[8];
	tU8 U8[8];

	tBits64 Bits;
} tU64Union;

typedef union _tS64Union
{
	double Double;
	tS64 S64;

	tU32Union U32_Union[2];
	tU32 U32[2];

	tU16Union U16_Union[4];
	tU16 U16[4];

	tU8Union U8_Union[8];
	tU8 U8[8];

	tBits64 Bits;
} tS64Union;

#define SCALE_TENTH      ((tF32)0.1)
#define SCALE_HUNDREDTH  ((tF32)0.01) 
#define SCALE_THOUSANDTH ((tF32)0.001)    

#endif /* INTMASH_BASTYPES_H_ */



