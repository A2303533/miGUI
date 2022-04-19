#include "CRT.h"

#include "stdlib.h"
#include "string.h"

#include "WinAPI.h"


#ifdef __CRT_PURE
#else
#ifdef _NDEBUG
#else
#endif /*_NDEBUG*/
#endif

// /NODEFAULTLIB 

#ifdef __CRT_PURE

__CRT_main_struct __crt;
int __crt_ready = 0;

#ifdef __cplusplus
extern "C" {
#endif
	int _fltused = 0; // it should be a single underscore since the double one is the mangled name
#ifdef __cplusplus
}
#endif

#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCZ",long,read)
#pragma section(".CRT$XIA",long,read)
#pragma section(".CRT$XIZ",long,read)
#pragma section(".CRT$XPA",long,read)
#pragma section(".CRT$XPZ",long,read)
#pragma section(".CRT$XTA",long,read)
#pragma section(".CRT$XTZ",long,read)

#pragma data_seg(".CRT$XCA")
#pragma data_seg(".CRT$XCZ")
// C initializers
__declspec(allocate(".CRT$XIA")) _PIFV __xi_a[] = { 0 };
__declspec(allocate(".CRT$XIZ")) _PIFV __xi_z[] = { 0 };

// C++ initializers
__declspec(allocate(".CRT$XCA")) _PVFV __xc_a[] = { 0 };
__declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[] = { 0 };

// C pre-terminators
__declspec(allocate(".CRT$XPA")) _PVFV __xp_a[] = { 0 };
__declspec(allocate(".CRT$XPZ")) _PVFV __xp_z[] = { 0 };

// C terminators
__declspec(allocate(".CRT$XTA")) _PVFV __xt_a[] = { 0 };
__declspec(allocate(".CRT$XTZ")) _PVFV __xt_z[] = { 0 };


#pragma data_seg()
#if defined(_M_IA64) || defined(_M_AMD64)
#pragma comment(linker, "/merge:.CRT=.rdata")
#else
#pragma comment(linker, "/merge:.CRT=.data")
#endif

int 
__cdecl 
_purecall()
{
	int pp = 0;
	//_purecall_handler const purecall_handler = _get_purecall_handler();
	//if (purecall_handler)
	//{
	//	purecall_handler();

	//	// The user-registered purecall handler should not return, but if it does,
	//	// continue with the default termination behavior.
	//}

	//abort();
}

void
_C_DECL
__CRT_on_atexitProcs()
{
	if (__crt._->atexitProcs && __crt._->atexitProcsSize)
	{
		for (unsigned int i = __crt._->atexitProcsSize - 1; ; )
		{
			_PVFV fnc = __crt._->atexitProcs[i];
			fnc();
			if (!i)
				break;
			--i;
		}

		free(__crt._->atexitProcs);
	}
}

void
__grow_atexitProcs()
{
	const int addSize = 100;
	_PVFV* newProcs = malloc(__crt._->atexitProcsAllocated + addSize * sizeof(_PVFV));

	if (__crt._->atexitProcsAllocated && __crt._->atexitProcs)
	{
		memcpy(newProcs, __crt._->atexitProcs, __crt._->atexitProcsAllocated);
		free(__crt._->atexitProcs);
	}
	__crt._->atexitProcs = newProcs;
	__crt._->atexitProcsAllocated = __crt._->atexitProcsAllocated + addSize;
}

void
__add_atexitProcs(_PVFV fnc)
{
	unsigned int newSize = __crt._->atexitProcsSize + 1;
	if (newSize > __crt._->atexitProcsAllocated)
		__grow_atexitProcs();

	__crt._->atexitProcs[__crt._->atexitProcsSize] = fnc;
	__crt._->atexitProcsSize = newSize;
}

double 
_except1(int flags, int opcode, double arg, double res, unsigned int* cw)
{
	/*???????*/
	return 0.;
}

int 
_global_init_i(_PIFV* a, _PIFV* b)
{
	_PIFV* pfbegin = a;
	_PIFV* pfend = b;

	int ret = 0;

	while (pfbegin < pfend && ret == 0)
	{
		if (*pfbegin != 0)
			ret = (**pfbegin)();
		++pfbegin;
	}
	return ret;
}

int 
_global_init_v(_PVFV* a, _PVFV* b)
{
	_PVFV* pfbegin = a;
	_PVFV* pfend = b;

	int ret = 0;

	while (pfbegin < pfend && ret == 0)
	{
		if (*pfbegin != 0)
			(**pfbegin)();
		++pfbegin;
	}
	return 1;
}
#endif

void 
_C_DECL 
__CRT_init()
{
#ifdef __CRT_PURE
	if (__crt_ready)
		exit(EXIT_FAILURE);
#endif

	memset(&__crt, 0, sizeof(__CRT_main_struct));
	__crt.processHeap = GetProcessHeap();

	__crt._ = calloc(1, sizeof(__CRT_main_struct_i));
	memcpy(__crt._->tmpnamInternalBuf, "TMP000000000", 13);

	setlocale(LC_ALL, "C");
	srand(1);

#ifdef __CRT_PURE
	__grow_atexitProcs();
#endif

	signal(SIGABRT, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	__crt_ready = 1;
	
#ifdef __CRT_PURE
	_global_init_i(__xi_a, __xi_z);
	_global_init_v(__xc_a, __xc_z);
#endif
}

void 
_C_DECL 
__CRT_shutdown()
{
#ifdef __CRT_PURE
	if (!__crt_ready)
		exit(EXIT_FAILURE);

	__crt_ready = 0;
	
	exit(EXIT_SUCCESS);
#endif
}
