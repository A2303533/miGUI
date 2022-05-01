#include "CRT.h"

#include "stdlib.h"
#include "string.h"
#include "pool.h"

#include "WinAPI.h"


__CRT_main_struct __crt;
int __crt_ready = 0;

void __grow_atexitProcs();

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

double
__cdecl
_except1(
	DWORD fpe, 
    DWORD op,
	double arg, 
	double res, 
	DWORD cw, 
	void* unk)
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

void 
_C_DECL 
__CRT_init()
{
	if (__crt_ready)
		exit(EXIT_FAILURE);

	_except1(0, 0, 0, 0, 0, 0);

	memset(&__crt, 0, sizeof(__CRT_main_struct));
	__crt.processHeap = GetProcessHeap(); // CRT_vs.c - for Visual Studio

	__crt._ = calloc(1, sizeof(__CRT_main_struct_i));
	memcpy(__crt._->tmpnamInternalBuf, "TMP000000000", 13);

	__crt._->fopenModeUnion.i = 0;

	//if (!GetStdHandle(((unsigned int)-11))) // not work in XP
	AttachConsole((unsigned int)-1);

	__crt._->_stdin = fopen("CONIN$", "rb");
	__crt._->_stdout = fopen("CONOUT$", "wb");
	__crt._->_stderr = fopen("CONOUT$", "wb");
	/*__crt._->_stdin = GetStdHandle(((unsigned int)-10));
	__crt._->_stdout = GetStdHandle(((unsigned int)-11));
	__crt._->_stderr = GetStdHandle(((unsigned int)-12));*/

	setlocale(LC_ALL, "C");
	srand(1);

	__grow_atexitProcs();

	signal(SIGABRT, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	__crt_ready = 1;
	
	_global_init_i(__xi_a, __xi_z);
	_global_init_v(__xc_a, __xc_z);
}

void 
_C_DECL 
__CRT_shutdown()
{
	if (!__crt_ready)
		exit(EXIT_FAILURE);

	if (__crt._->_stdin) fclose(__crt._->_stdin);
	if (__crt._->_stdout) fclose(__crt._->_stdout);
	if (__crt._->_stderr) fclose(__crt._->_stderr);

	__crt_ready = 0;
	
	exit(EXIT_SUCCESS);
}
