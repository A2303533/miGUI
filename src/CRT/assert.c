#include "CRT.h"
#include "stdlib.h"
#include "assert.h"

#ifdef __CRT_WIN32
#include "WinAPI.h"
#endif


void 
_C_DECL 
__assert(
	const char* exp, 
	const char* file, 
	const char* func, 
	unsigned int line)
{
#ifdef __CRT_WIN32
	MessageBoxA(0, exp, func, 0);
#endif

	exit(EXIT_FAILURE);
	//abort()
}

