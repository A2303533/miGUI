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
#ifndef _COMPLEX_H
#define _COMPLEX_H

#include "CRT.h"


#ifdef _MSC_VER
#define	complex 
#endif // _MSC_VER
#include "math.h"
#include "math_private.h"


/*ISO/IEC 9899:TC3, N1256*/

#define	_Complex_I	((float _Complex)1.0i)
#define	I		_Complex_I

#ifdef __cplusplus
extern "C" {
#endif

	double_complex _C_DECL __CRT_casinh(double_complex z);
	float_complex _C_DECL __CRT_casinhf(float_complex z);
	long_double_complex _C_DECL __CRT_casinhl(long_double_complex z);

	double_complex _C_DECL __CRT_casin(double_complex z);
	float_complex _C_DECL __CRT_casinf(float_complex z);
	long_double_complex _C_DECL __CRT_casinl(long_double_complex z);

	double_complex _C_DECL __CRT_cacos(double_complex z);
	float_complex _C_DECL __CRT_cacosf(float_complex z);
	long_double_complex _C_DECL __CRT_cacosl(long_double_complex z);

	double_complex _C_DECL __CRT_cacosh(double_complex z);
	float_complex _C_DECL __CRT_cacoshf(float_complex z);
	long_double_complex _C_DECL __CRT_cacoshl(long_double_complex z);

	double_complex _C_DECL __CRT_catanh(double_complex z);
	float_complex _C_DECL __CRT_catanhf(float_complex z);
	long_double_complex _C_DECL __CRT_catanhl(long_double_complex z);

	double_complex _C_DECL __CRT_catan(double_complex z);
	float_complex _C_DECL __CRT_catanf(float_complex z);
	long_double_complex _C_DECL __CRT_catanl(long_double_complex z);

	double_complex _C_DECL __CRT_ccosh(double_complex z);
	float_complex _C_DECL __CRT_ccoshf(float_complex z);
	long_double_complex _C_DECL __CRT_ccoshl(long_double_complex z);

	double_complex _C_DECL __CRT_ccos(double_complex z);
	float_complex _C_DECL __CRT_ccosf(float_complex z);
	long_double_complex _C_DECL __CRT_ccosl(long_double_complex z);

	double_complex _C_DECL __CRT_csinh(double_complex z);
	float_complex _C_DECL __CRT_csinhf(float_complex z);
	long_double_complex _C_DECL __CRT_csinhl(long_double_complex z);

	double_complex _C_DECL __CRT_csin(double_complex z);
	float_complex _C_DECL __CRT_csinf(float_complex z);
	long_double_complex _C_DECL __CRT_csinl(long_double_complex z);

	long_double_complex _C_DECL __CRT_ctanhl(long_double_complex z);
	float_complex _C_DECL __CRT_ctanhf(float_complex z);
	double_complex _C_DECL __CRT_ctanh(double_complex z);

	double_complex _C_DECL __CRT_ctan(double_complex z);
	float_complex _C_DECL __CRT_ctanf(float_complex z);
	long_double_complex _C_DECL __CRT_ctanl(long_double_complex z);

	long_double_complex _C_DECL __CRT_cexpl(long_double_complex z);
	float_complex _C_DECL __CRT_cexpf(float_complex z);
	double_complex _C_DECL __CRT_cexp(double_complex z);

	long_double_complex _C_DECL __CRT_clogl(long_double_complex z);
	float_complex _C_DECL __CRT_clogf(float_complex z);
	double_complex _C_DECL __CRT_clog(double_complex z);

	double_complex _C_DECL __CRT_cpow(double_complex a, double_complex z);
	float_complex _C_DECL __CRT_cpowf(float_complex a, float_complex z);
	long_double_complex _C_DECL __CRT_cpowl(long_double_complex a, long_double_complex z);

	double_complex _C_DECL __CRT_csqrt(double_complex z);
	long_double_complex _C_DECL __CRT_csqrtl(long_double_complex z);
	float_complex _C_DECL __CRT_csqrtf(float_complex z);

	// 7.3.5.1
	/// <summary>
	/// the complex arc cosine
	/// </summary>
	double_complex _C_DECL cacos(double_complex);
	float_complex _C_DECL cacosf(float_complex);
	long_double_complex _C_DECL cacosl(long_double_complex);

	// 7.3.5.2
	/// <summary>
	/// the complex arc sine
	/// </summary>
	double_complex _C_DECL casin(double_complex);
	float_complex _C_DECL casinf(float_complex);
	long_double_complex _C_DECL casinl(long_double_complex);

	// 7.3.5.3
	/// <summary>
	/// the complex arc tangent
	/// </summary>
	double_complex _C_DECL catan(double_complex);
	float_complex _C_DECL catanf(float_complex);
	long_double_complex _C_DECL catanl(long_double_complex);

	// 7.3.5.4
	/// <summary>
	/// the complex cosine
	/// </summary>
	double_complex _C_DECL ccos(double_complex);
	float_complex _C_DECL ccosf(float_complex);
	long_double_complex _C_DECL ccosl(long_double_complex);

	// 7.3.5.5
	/// <summary>
	/// the complex sine
	/// </summary>
	double_complex _C_DECL csin(double_complex);
	float_complex _C_DECL csinf(float_complex);
	long_double_complex _C_DECL csinl(long_double_complex);

	// 7.3.5.6
	/// <summary>
	/// the complex tangent
	/// </summary>
	double_complex _C_DECL ctan(double_complex);
	float_complex _C_DECL ctanf(float_complex);
	long_double_complex _C_DECL ctanl(long_double_complex);

	// 7.3.6.1
	/// <summary>
	/// the complex arc hyperbolic cosine
	/// </summary>
	double_complex _C_DECL cacosh(double_complex);
	float_complex _C_DECL cacoshf(float_complex);
	long_double_complex _C_DECL cacoshl(long_double_complex);

	// 7.3.6.2
	/// <summary>
	/// the complex arc hyperbolic sine
	/// </summary>
	double_complex _C_DECL casinh(double_complex);
	float_complex _C_DECL casinhf(float_complex);
	long_double_complex _C_DECL casinhl(long_double_complex);

	// 7.3.6.3
	/// <summary>
	/// the complex arc hyperbolic tangent
	/// </summary>
	double_complex _C_DECL catanh(double_complex);
	float_complex _C_DECL catanhf(float_complex);
	long_double_complex _C_DECL catanhl(long_double_complex);

	// 7.3.6.4
	/// <summary>
	/// the complex hyperbolic cosine
	/// </summary>
	double_complex _C_DECL ccosh(double_complex);
	float_complex _C_DECL ccoshf(float_complex);
	long_double_complex _C_DECL ccoshl(long_double_complex);

	// 7.3.6.5
	/// <summary>
	/// the complex hyperbolic sine
	/// </summary>
	double_complex _C_DECL csinh(double_complex);
	float_complex _C_DECL csinhf(float_complex);
	long_double_complex _C_DECL csinhl(long_double_complex);

	// 7.3.6.6
	/// <summary>
	/// the complex hyperbolic tangent
	/// </summary>
	double_complex _C_DECL ctanh(double_complex);
	float_complex _C_DECL ctanhf(float_complex);
	long_double_complex _C_DECL ctanhl(long_double_complex);

	// 7.3.7.1
	/// <summary>
	/// the complex base-e exponential
	/// </summary>
	double_complex _C_DECL cexp(double_complex);
	float_complex _C_DECL cexpf(float_complex);
	long_double_complex _C_DECL cexpl(long_double_complex);

	// 7.3.7.2
	/// <summary>
	/// the complex natural (base-e) logarithm
	/// </summary>
	double_complex _C_DECL clog(double_complex);
	float_complex _C_DECL clogf(float_complex);
	long_double_complex _C_DECL clogl(long_double_complex);

	// 7.3.8.1
	double _C_DECL cabs(double_complex);
	float _C_DECL cabsf(float_complex);
	long double _C_DECL cabsl(long_double_complex);

	// 7.3.8.2
	double_complex _C_DECL cpow(double_complex x, double_complex y);
	float_complex _C_DECL cpowf(float_complex x, float_complex y);
	long_double_complex _C_DECL cpowl(long_double_complex x, long_double_complex y);

	// 7.3.8.3
	double_complex _C_DECL csqrt(double_complex);
	float_complex _C_DECL csqrtf(float_complex);
	long_double_complex _C_DECL csqrtl(long_double_complex);

	// 7.3.9.1
	double _C_DECL carg(double_complex);
	float _C_DECL cargf(float_complex);
	long double _C_DECL cargl(long_double_complex);

	// 7.3.9.2
	double _C_DECL cimag(double_complex);
	float _C_DECL cimagf(float_complex);
	long double _C_DECL cimagl(long_double_complex);

	// 7.3.9.3
	/// <summary>
	/// the complex conjugate
	/// </summary>
	double_complex _C_DECL conj(double_complex);
	float_complex _C_DECL conjf(float_complex);
	long_double_complex _C_DECL conjl(long_double_complex);

	// 7.3.9.4
	/// <summary>
	/// compute a projection of z onto the Riemann sphere:
	/// </summary>
	double_complex _C_DECL cproj(double_complex);
	float_complex _C_DECL cprojf(float_complex);
	long_double_complex _C_DECL cprojl(long_double_complex);

	// 7.3.9.5
	double _C_DECL creal(double_complex);
	float _C_DECL crealf(float_complex);
	long double _C_DECL creall(long_double_complex);

#ifdef __cplusplus
}
#endif

#endif