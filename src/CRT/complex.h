/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2001-2011 The FreeBSD Project.
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

#pragma once
#ifndef __COMPLEX__
#define __COMPLEX__

#include "CRT.h"


/*ISO/IEC 9899:TC3, N1256*/

#ifdef _MSC_VER
#ifndef _C_COMPLEX_T
#define _C_COMPLEX_T // Also defined by UCRT <complex.h>
struct _C_double_complex {
	double _Val[2];
};
struct _C_float_complex {
	float _Val[2];
};
struct _C_ldouble_complex {
	long double _Val[2];
};
#endif // _C_COMPLEX_T
#endif // _MSC_VER

#define	complex_f struct _C_float_complex
#define	complex_d struct _C_double_complex
#define	complex_l struct _C_ldouble_complex
#define	_Complex_I	((float _Complex)1.0i)
#define	I		_Complex_I

#ifdef __cplusplus
extern "C" {
#endif
	// 7.3.5.1
	/// <summary>
	/// the complex arc cosine
	/// </summary>
	complex_d _C_DECL cacos(complex_d);
	complex_f _C_DECL cacosf(complex_f);
	complex_l _C_DECL cacosl(complex_l);

	// 7.3.5.2
	/// <summary>
	/// the complex arc sine
	/// </summary>
	complex_d _C_DECL casin(complex_d);
	complex_f _C_DECL casinf(complex_f);
	complex_l _C_DECL casinl(complex_l);

	// 7.3.5.3
	/// <summary>
	/// the complex arc tangent
	/// </summary>
	complex_d _C_DECL catan(complex_d);
	complex_f _C_DECL catanf(complex_f);
	complex_l _C_DECL catanl(complex_l);

	// 7.3.5.4
	/// <summary>
	/// the complex cosine
	/// </summary>
	complex_d _C_DECL ccos(complex_d);
	complex_f _C_DECL ccosf(complex_f);
	complex_l _C_DECL ccosl(complex_l);

	// 7.3.5.5
	/// <summary>
	/// the complex sine
	/// </summary>
	complex_d _C_DECL csin(complex_d);
	complex_f _C_DECL csinf(complex_f);
	complex_l _C_DECL csinl(complex_l);

	// 7.3.5.6
	/// <summary>
	/// the complex tangent
	/// </summary>
	complex_d _C_DECL ctan(complex_d);
	complex_f _C_DECL ctanf(complex_f);
	complex_l _C_DECL ctanl(complex_l);

	// 7.3.6.1
	/// <summary>
	/// the complex arc hyperbolic cosine
	/// </summary>
	complex_d _C_DECL cacosh(complex_d);
	complex_f _C_DECL cacoshf(complex_f);
	complex_l _C_DECL cacoshl(complex_l);

	// 7.3.6.2
	/// <summary>
	/// the complex arc hyperbolic sine
	/// </summary>
	complex_d _C_DECL casinh(complex_d);
	complex_f _C_DECL casinhf(complex_f);
	complex_l _C_DECL casinhl(complex_l);

	// 7.3.6.3
	/// <summary>
	/// the complex arc hyperbolic tangent
	/// </summary>
	complex_d _C_DECL catanh(complex_d);
	complex_f _C_DECL catanhf(complex_f);
	complex_l _C_DECL catanhl(complex_l);

	// 7.3.6.4
	/// <summary>
	/// the complex hyperbolic cosine
	/// </summary>
	complex_d _C_DECL ccosh(complex_d);
	complex_f _C_DECL ccoshf(complex_f);
	complex_l _C_DECL ccoshl(complex_l);

	// 7.3.6.5
	/// <summary>
	/// the complex hyperbolic sine
	/// </summary>
	complex_d _C_DECL csinh(complex_d);
	complex_f _C_DECL csinhf(complex_f);
	complex_l _C_DECL csinhl(complex_l);

	// 7.3.6.6
	/// <summary>
	/// the complex hyperbolic tangent
	/// </summary>
	complex_d _C_DECL ctanh(complex_d);
	complex_f _C_DECL ctanhf(complex_f);
	complex_l _C_DECL ctanhl(complex_l);

	// 7.3.7.1
	/// <summary>
	/// the complex base-e exponential
	/// </summary>
	complex_d _C_DECL cexp(complex_d);
	complex_f _C_DECL cexpf(complex_f);
	complex_l _C_DECL cexpl(complex_l);

	// 7.3.7.2
	/// <summary>
	/// the complex natural (base-e) logarithm
	/// </summary>
	complex_d _C_DECL clog(complex_d);
	complex_f _C_DECL clogf(complex_f);
	complex_l _C_DECL clogl(complex_l);

	// 7.3.8.1
	double _C_DECL cabs(complex_d);
	float _C_DECL cabsf(complex_f);
	long double _C_DECL cabsl(complex_l);

	// 7.3.8.2
	complex_d _C_DECL cpow(complex_d x, complex_d y);
	complex_f _C_DECL cpowf(complex_f x, complex_f y);
	complex_l _C_DECL cpowl(complex_l x, complex_l y);

	// 7.3.8.3
	complex_d _C_DECL csqrt(complex_d);
	complex_f _C_DECL csqrtf(complex_f);
	complex_l _C_DECL csqrtl(complex_l);

	// 7.3.9.1
	double _C_DECL carg(complex_d);
	float _C_DECL cargf(complex_f);
	long double _C_DECL cargl(complex_l);

	// 7.3.9.2
	double _C_DECL cimag(complex_d);
	float _C_DECL cimagf(complex_f);
	long double _C_DECL cimagl(complex_l);

	// 7.3.9.3
	/// <summary>
	/// the complex conjugate
	/// </summary>
	complex_d _C_DECL conj(complex_d);
	complex_f _C_DECL conjf(complex_f);
	complex_l _C_DECL conjl(complex_l);

	// 7.3.9.4
	/// <summary>
	/// compute a projection of z onto the Riemann sphere:
	/// </summary>
	complex_d _C_DECL cproj(complex_d);
	complex_f _C_DECL cprojf(complex_f);
	complex_l _C_DECL cprojl(complex_l);

	// 7.3.9.5
	double _C_DECL creal(complex_d);
	float _C_DECL crealf(complex_f);
	long double _C_DECL creall(complex_l);

#ifdef __cplusplus
}
#endif

#endif