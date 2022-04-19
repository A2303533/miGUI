#pragma once
#ifndef __SIGNAL__
#define __SIGNAL__

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef	void (*__crt_signal_handler_t)(int);

	// 7.14:2
	typedef volatile int sig_atomic_t;

	void _C_DECL __crt_signal_dfl(int);
	void _C_DECL __crt_signal_err(int);
	void _C_DECL __crt_signal_ign(int);

	// 7.14:3
#define SIG_DFL __crt_signal_dfl
#define SIG_ERR __crt_signal_err
#define SIG_IGN __crt_signal_ign

#define SIGABRT 1
#define SIGFPE 2
#define SIGILL 3
#define SIGINT 4
#define SIGSEGV 5
#define SIGTERM 6

	// 7.14.1.1
	__crt_signal_handler_t _C_DECL signal(int sig, __crt_signal_handler_t func);
	
	// 7.14.2.1 
	int _C_DECL raise(int sig);

#ifdef __cplusplus
}
#endif

#endif