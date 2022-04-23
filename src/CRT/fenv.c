/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2004-2005 David Schultz <das@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

// Modified for migui 

#include "fenv.h"

//#define	__INITIAL_FPUCW_I386__	0x127F
const fenv_t __CRT_fe_dfl_env = {
	
	//__INITIAL_NPXCW__,
	0x127F,

	0x0000,
	0x0000,
	0x1f80,
	0xffffffff,
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff }
};

int 
_C_DECL 
feclearexcept(int excepts)
{
	return 0;
}

int
_C_DECL
fegetexceptflag(fexcept_t* flagp, int excepts)
{
	return -1;
}

int 
_C_DECL 
feraiseexcept(int excepts)
{
	return 0;
}

int 
_C_DECL 
fesetexceptflag(const fexcept_t* flagp, int excepts)
{
	return -1;
}

int 
_C_DECL 
fetestexcept(int excepts)
{
	return 0;
}

int 
_C_DECL 
fegetround(void)
{
	return 0;
}

int 
_C_DECL 
fesetround(int round)
{
	return 0;
}

int 
_C_DECL 
fegetenv(fenv_t* envp)
{
	*envp = __CRT_fe_dfl_env;
}

int 
_C_DECL 
feholdexcept(fenv_t* envp)
{
	return -1;
}

int 
_C_DECL 
fesetenv(const fenv_t* envp)
{
	return -1;
}

int 
_C_DECL 
feupdateenv(const fenv_t* envp)
{
	return -1;
}



