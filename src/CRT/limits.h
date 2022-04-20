#pragma once
#ifndef __LIMITS__
#define __LIMITS__

#include "CRT.h"

//ISO/IEC 9899:TC3, N1256

#define CHAR_BIT 8
#define INT_MAX 2147483647
#define INT_MIN (-2147483647 - 1)
#define LONG_MAX 2147483647L
#define LONG_MIN (-2147483647L - 1)
#define LLONG_MAX 9223372036854775807i64
#define LLONG_MIN (-9223372036854775807i64 - 1)
#define MB_LEN_MAX 5
#define SCHAR_MAX 127
#define SCHAR_MIN (-128)
#define SHRT_MAX 32767
#define SHRT_MIN (-32768)
#define UCHAR_MAX 0xff
#define USHRT_MAX 0xffff
#define UINT_MAX  0xffffffff
#define ULONG_MAX 0xffffffffUL
#define ULLONG_MAX 0xffffffffffffffffui64
#define CHAR_MAX SCHAR_MAX
#define CHAR_MIN SCHAR_MIN

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif


#endif