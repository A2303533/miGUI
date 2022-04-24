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

 // Modified for migui 

#pragma once
#ifndef __MATH__
#define __MATH__

#include "CRT.h"
#include "limits.h"

/*ISO/IEC 9899:TC3, N1256*/

// 7.12:2
typedef float float_t;
typedef double double_t;

#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

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

// 7.12.3.4
#define	isnormal(x) __fp_type_select(x, __CRT_isnormalf, __CRT_isnormal, __CRT_isnormall)

// 7.12.3.6
#define	signbit(x) __fp_type_select(x, __CRT_signbitf, __CRT_signbit, __CRT_signbitl)

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
	int	_C_DECL __CRT_isnormalf(float);
	int	_C_DECL __CRT_isnormal(double);
	int	_C_DECL __CRT_isnormall(long double);
	int	_C_DECL __CRT_signbit(double);
	int	_C_DECL __CRT_signbitf(float);
	int	_C_DECL __CRT_signbitl(long double);

	double _C_DECL __CRT_modf(double x, double* iptr);

	double _C_DECL __CRT_scalbn(double x, int n);
	float _C_DECL __CRT_scalbnf(float x, int n);
	long double _C_DECL __CRT_scalbnl(long double x, int n);
	double _C_DECL __CRT_scalbln(double x, long n);
	float _C_DECL __CRT_scalblnf(float x, long n);
	long double _C_DECL __CRT_scalblnl(long double x, long n);

	double _C_DECL __CRT_ieee754_acos(double x);
	float _C_DECL __CRT_ieee754_acosf(float x);
	long double _C_DECL __CRT_acosl(long double x);

	double _C_DECL __CRT_ieee754_sqrt(double x);
	float _C_DECL __CRT_ieee754_sqrtf(float x);
	long double _C_DECL __CRT_sqrtl(long double x);

	double _C_DECL __CRT_fabs(double x);
	float _C_DECL __CRT_fabsf(float x);
	long double _C_DECL __CRT_fabsl(long double x);

	double _C_DECL __CRT_ieee754_asin(double x);
	float _C_DECL __CRT_ieee754_asinf(float x);
	long double _C_DECL __CRT_asinl(long double x);

	double _C_DECL __CRT_floor(double x);
	float _C_DECL __CRT_floorf(float x);
	long double _C_DECL __CRT_floorl(long double x);

	double _C_DECL __CRT_tan(double x);
	float _C_DECL __CRT_tanf(float x);
	long double _C_DECL __CRT_tanl(long double x);

	double _C_DECL __CRT_atan(double x);
	float _C_DECL __CRT_atanf(float x);
	long double _C_DECL __CRT_atanl(long double x);

	double _C_DECL __CRT_ieee754_atan2(double y, double x);
	float _C_DECL __CRT_ieee754_atan2f(float y, float x);
	long double _C_DECL __CRT_ieee754_atan2l(long double y, long double x);

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

	// 7.12.4.1
	double _C_DECL acos(double x);
	float _C_DECL acosf(float x);
	long double _C_DECL acosl(long double x);

	// 7.12.4.2
	double _C_DECL asin(double x);
	float _C_DECL asinf(float x);
	long double _C_DECL asinl(long double x);

	// 7.12.4.3
	double _C_DECL atan(double x);
	float _C_DECL atanf(float x);
	long double _C_DECL atanl(long double x);

	// 7.12.4.4
	double _C_DECL atan2(double y, double x);
	float _C_DECL atan2f(float y, float x);
	long double _C_DECL atan2l(long double y, long double x);

	// 7.12.4.7
	double _C_DECL tan(double x);
	float _C_DECL tanf(float x);
	long double _C_DECL tanl(long double x);

	// 7.12.6.12
	double _C_DECL modf(double value, double* iptr);
	float _C_DECL modff(float value, float* iptr);
	long double _C_DECL modfl(long double value, long double* iptr);

	// 7.12.6.13
	double _C_DECL scalbn(double x, int n);
	float _C_DECL scalbnf(float x, int n);
	long double _C_DECL scalbnl(long double x, int n);
	double _C_DECL scalbln(double x, int n);
	float _C_DECL scalblnf(float x, int n);
	long double _C_DECL scalblnl(long double x, int n);

	// 7.12.7.2
	double _C_DECL fabs(double x);
	float _C_DECL fabsf(float x);
	long double _C_DECL fabsl(long double x);
	
	// 7.12.7.5
	double _C_DECL sqrt(double x);
	float _C_DECL sqrtf(float x);
	long double _C_DECL sqrtl(long double x);
	
	// 7.12.9.2
	double _C_DECL floor(double x);
	float _C_DECL floorf(float x);
	long double _C_DECL floorl(long double x);

#ifdef __cplusplus
}
#endif

#endif