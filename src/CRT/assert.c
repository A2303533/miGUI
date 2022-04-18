#include "CRT.h"
#include "stdlib.h"
#include "assert.h"

#ifdef __CRT_WIN32
//#include <Windows.h>
__declspec(dllimport) int __stdcall MessageBoxA( void*, const char*, const char*, unsigned int);
#endif

#ifdef __CRT_PURE

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
}

#endif /*__CRT_PURE*/
