#pragma once
#ifndef __STDARG__
#define __STDARG__

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 7.15:3
	typedef char* va_list;

	// 7.15.1.1
#define va_arg(ap,type) *(type*)ap; ap += sizeof(type)

	// 7.15.1.2
#define va_copy(dest,src) dest = src

	// 7.15.1.3
#define va_end(ap) ap = 0

	// 7.15.1.4
#define va_start(ap,parmN) ap = (va_list)&parmN + sizeof(parmN)

#ifdef __cplusplus
}
#endif

#endif