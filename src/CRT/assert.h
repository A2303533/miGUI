#pragma once
#ifndef __ASSERT__
#define __ASSERT__

#include "CRT.h"


/*ISO/IEC 9899:TC3, N1256*/

/*all internal prototypes*/
#ifdef __cplusplus
extern "C" {
#endif

	void _C_DECL __assert(const char* exp, const char* file, const char* func, unsigned int line);

#ifdef __cplusplus
}
#endif

/*7.2.1.1*/
#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
#define assert(exp) if((exp) == 0){__assert(#exp, __FILE__, __FUNCTION__, __LINE__);}
#endif

#endif