/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */


#pragma once
#ifndef __MATH__
#define __MATH__

#include "CRT.h"
#include "limits.h"

/*ISO/IEC 9899:TC3, N1256*/

// 7.12:2
typedef float float_t;
typedef double double_t;

// 7.12:3
#define HUGE_VAL __CRT_infinity()
#define	HUGE_VALF	(float)HUGE_VAL
#define	HUGE_VALL	(long double)HUGE_VAL
// 7.12:4
#define	INFINITY	HUGE_VALF
// 7.12:5
#define	NAN		__CRT_nan()

// 7.12:6
#define	FP_INFINITE	0x01
#define	FP_NAN		0x02
#define	FP_NORMAL	0x04
#define	FP_SUBNORMAL	0x08
#define	FP_ZERO		0x10

// 7.12:7
//#define	FP_FAST_FMA	1

// 7.12:8
#define	FP_ILOGB0	(-INT_MAX)
#define	FP_ILOGBNAN	INT_MAX

// 7.12:9
#define	MATH_ERRNO	1
#define	MATH_ERREXCEPT	2
#define	math_errhandling	MATH_ERREXCEPT

#define	 __fp_type_select(x, f, d, ld)					\
    ((sizeof(x) == sizeof(float)) ? f(x)				\
    : (sizeof(x) == sizeof(double)) ? d(x)				\
    : ld(x))

// 7.12.3.1
#define	fpclassify(x) \
	__fp_type_select(x, __CRT_fpclassifyf, __CRT_fpclassifyd, __CRT_fpclassifyl)

// 7.12.3.2
#define	isfinite(x) __fp_type_select(x, __CRT_isfinitef, __CRT_isfinite, __CRT_isfinitel)

// 7.12.3.3
#define	isinf(x) __fp_type_select(x, __CRT_isinff, __CRT_isinf, __CRT_isinfl)

// 7.12.3.4
#define	isnan(x) \
	__fp_type_select(x, __CRT_inline_isnanf, __CRT_inline_isnan, __CRT_inline_isnanl)

#ifdef __cplusplus
extern "C" {
#endif

	int	_C_DECL __CRT_fpclassifyd(double);
	int	_C_DECL __CRT_fpclassifyf(float);
	int	_C_DECL __CRT_fpclassifyl(long double);
	int	_C_DECL __CRT_isfinitef(float);
	int	_C_DECL __CRT_isfinite(double);
	int	_C_DECL __CRT_isfinitel(long double);
	int	_C_DECL __CRT_isinff(float);
	int	_C_DECL __CRT_isinf(double);
	int	_C_DECL __CRT_isinfl(long double);
	float _C_DECL __CRT_nan(void);
	double _C_DECL __CRT_infinity(void);

	static __inline int
		__CRT_inline_isnan(double __x)
	{

		return (__x != __x);
	}

	static __inline int
		__CRT_inline_isnanf(float __x)
	{

		return (__x != __x);
	}

	static __inline int
		__CRT_inline_isnanl(long double __x)
	{

		return (__x != __x);
	}

// 7.12.6.12
	double _C_DECL modf(double value, double* iptr);
	float _C_DECL modff(float value, float* iptr);
	long double _C_DECL modfl(long double value, long double* iptr);


#ifdef __cplusplus
}
#endif

#endif