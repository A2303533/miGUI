#include "CRT.h"
#include "assert.h"
#include "signal.h"

extern __CRT_main_struct __crt;

void 
_C_DECL 
__crt_signal_dfl(int sig)
{

}

void 
_C_DECL 
__crt_signal_err(int sig)
{

}

void 
_C_DECL 
__crt_signal_ign(int sig)
{

}

__crt_signal_handler_t
_C_DECL 
signal(int sig, __crt_signal_handler_t func)
{
	__crt_signal_handler_t prevSig = SIG_ERR;

	if (sig > 0 && sig < 7)
	{
		prevSig = __crt._->currSignalHandlers[sig - 1];
		__crt._->currSignalHandlers[sig - 1] = func;
	}

	return prevSig;
}

int 
_C_DECL 
raise(int sig)
{
	int result = -1;
	if (sig > 0 && sig < 7)
	{
		if (__crt._->currSignalHandlers[sig - 1])
		{
			__crt._->currSignalHandlers[sig - 1](sig);
			result = 0;
		}
	}
	return result;
}
