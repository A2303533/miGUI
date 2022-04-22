#pragma once
#ifndef __STDARG__
#define __STDARG__

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 7.15:3
	typedef char* va_list;

	// 7.15.1.2
#define va_copy(dest,src) dest = src

	// Visual Studio version
#define _CRT_INTSIZEOF(n)          ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define _CRT_ADDRESSOF(v) (&(v))

#define va_start(ap, v) ((void)(ap = (va_list)_CRT_ADDRESSOF(v) + _CRT_INTSIZEOF(v)))
#define va_arg(ap, t)     (*(t*)((ap += _CRT_INTSIZEOF(t)) - _CRT_INTSIZEOF(t)))
#define va_end(ap)        ((void)(ap = (va_list)0))

#ifdef __cplusplus
}
#endif

#endif