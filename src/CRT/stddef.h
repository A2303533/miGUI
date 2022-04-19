#pragma once
#ifndef __STDDEF__
#define __STDDEF__

#include "CRT.h"

// ISO/IEC 9899:TC3, N1256

#ifdef __cplusplus
extern "C" {
#endif

	// 7.17:2
#include "size_t.h"

		// 7.17:2
		typedef size_t ptrdiff_t;

		// 7.17:2
#ifndef __cplusplus
#ifndef __CRT_WCHAR_T_DEFINED
#define __CRT_WCHAR_T_DEFINED
		typedef unsigned short wchar_t;
#endif
#endif

		// 7.17:3
#ifndef NULL
#ifdef __cplusplus
#if (__cplusplus > 199711L) // MSVS compiler option: /Zc:__cplusplus
#define NULL nullptr

#else
#define NULL 0
#endif
#else
#define NULL ((void *)0)
#endif
#endif

		// 7.17:3
#ifndef offsetof
#define offsetof(type,member) ((size_t)&(((type*)0)->member))
#endif

#ifdef __cplusplus
}
#endif

#endif