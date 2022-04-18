#pragma once
#ifndef __CRT__
#define __CRT__

/*without default lib and exceptions*/
#define __CRT_PURE 

#ifdef __CRT_PURE
/*you need to disable exceptions in project parameters*/
#pragma comment(linker, "/NODEFAULTLIB")
/*need to use libcmt.lib;libvcruntime.lib;*/
/*use LinkTimeCodeGeneration*/

#pragma comment(linker, "/ENTRY:\"__CRT_main\"")

/*#pragma comment(lib, "libcmt.lib")
#pragma comment(lib, "libvcruntime.lib")
#pragma comment(lib, "libucrt.lib")*/

typedef void (*_PVFV)(void);
typedef int (*_PIFV)(void);
#endif

#if defined(_WIN32)
#define __CRT_WIN32
#define _C_DECL _cdecl
#else
#define _C_DECL
#endif

#include "locale.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void _C_DECL __CRT_init();
	void _C_DECL __CRT_shutdown();

	typedef struct __CRT_main_struct_s {
#ifdef __CRT_WIN32
		void* processHeap;
#endif
		_PVFV* dtors;/*C++ destructors*/
		unsigned int dtorsSize;
		unsigned int dtorsAllocated;
		unsigned int rand_seed;

		lconv locale;

	} __CRT_main_struct;

#ifdef __cplusplus
}
#endif

#endif