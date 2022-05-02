#pragma once
#ifndef __CRT__
#define __CRT__

// without default lib and exceptions

// MSVS has own functions if enable optimization
// error C2169: 'memcpy': intrinsic function, cannot be defined 
// error C2268: 'memset' is a compiler predefined library helper
//          If enable intrinsic exe file will be larger.
// error C2169 - if /O2 and other optimizations
// error C2268 - if /O1
// so if you want to use Visual Studio optimizations  you need to comment this defines
#define __CRT_WITH_MEMSET // error C2169, C2268
#define __CRT_WITH_MEMCPY   // error C2169
#define __CRT_WITH_STRCMP   // error C2169
//#define __CRT_WITH_ABORT    // error C2381

/*you need to disable exceptions in project parameters*/
#pragma comment(linker, "/NODEFAULTLIB")
/*need to use libcmt.lib;libvcruntime.lib;*/
/*use LinkTimeCodeGeneration*/
// /OPT:REF

#pragma comment(linker, "/ENTRY:\"__CRT_main\"")

#define __CRT_INTRIN

typedef void (*_PVFV)(void);
typedef int (*_PIFV)(void);

#if defined(_WIN32)
#define __CRT_WIN32
#define _C_DECL _cdecl
#else
#define _C_DECL
#endif

#ifdef _WIN64
#define __CRT_ARCH_AMD64
#else
#define __CRT_ARCH_x86
#endif

#include "locale.h"
#include "signal.h"
#include "stdbool.h"
#include "stdio.h"
#include "pool.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void _C_DECL __CRT_init();
	void _C_DECL __CRT_shutdown();

	typedef struct {
		_PVFV* atexitProcs;/*C++ destructors and functions from stdlib atexit*/
		unsigned int atexitProcsSize;
		unsigned int atexitProcsAllocated;
		unsigned int rand_seed;

		struct lconv locale;

		__crt_signal_handler_t currSignalHandlers[6];

		FILE *_stdin, *_stdout, *_stderr;

		union
		{
			unsigned int i;
			char chars[4];
		}
		fopenModeUnion;

		char tmpnamInternalBuf[L_tmpnam+1];

	} __CRT_main_struct_i;

	typedef struct {
#ifdef __CRT_WIN32
		void* processHeap;
#endif

		__CRT_main_struct_i* _;

	} __CRT_main_struct;

#ifdef __cplusplus
}
#endif

#endif