
/* @(#)e_acosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

#include "CRT.h"

/* __ieee754_acosh(x)
 * Method :
 *	Based on 
 *		acosh(x) = log [ x + sqrt(x*x-1) ]
 *	we have
 *		acosh(x) := log(x)+ln2,	if x is large; else
 *		acosh(x) := log(2x-1/(sqrt(x*x-1)+x)) if x>2; else
 *		acosh(x) := log1p(t+sqrt(2.0*t+t*t)); where t=x-1.
 *
 * Special cases:
 *	acosh(x) is NaN with signal if x<1.
 *	acosh(NaN) is NaN without signal.
 */

#include "float.h"

#include "math.h"
#include "math_private.h"

double _C_DECL __CRT_ieee754_log(double x);
float _C_DECL __CRT_ieee754_log2f(float x);

static const double
one	= 1.0,
ln2	= 6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */

double
_C_DECL
__CRT_ieee754_acosh(double x)
{
	double t;
	int32_t hx;
	uint32_t lx;
	EXTRACT_WORDS(hx,lx,x);
	if(hx<0x3ff00000) {		/* x < 1 */
	    return (x-x)/(x-x);
	} else if(hx >=0x41b00000) {	/* x > 2**28 */
	    if(hx >=0x7ff00000) {	/* x is inf of NaN */
	        return x+x;
	    } else 
		return __CRT_ieee754_log(x)+ln2;	/* acosh(huge)=log(2x) */
	} else if(((hx-0x3ff00000)|lx)==0) {
	    return 0.0;			/* acosh(1) = 0 */
	} else if (hx > 0x40000000) {	/* 2**28 > x > 2 */
	    t=x*x;
	    return __CRT_ieee754_log(2.0*x-one/(x+sqrt(t-one)));
	} else {			/* 1<x<2 */
	    t = x-one;
	    return log1p(t+sqrt(2.0*t+t*t));
	}
}

#pragma warning(push)
#pragma warning(disable: 4305)
static const float
ln2f = 6.9314718246e-01;  /* 0x3f317218 */
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4244)
float
_C_DECL
__CRT_ieee754_acoshf(float x)
{
	float t;
	int32_t hx;
	GET_FLOAT_WORD(hx, x);
	if (hx < 0x3f800000) {		/* x < 1 */
		return (x - x) / (x - x);
	}
	else if (hx >= 0x4d800000) {	/* x > 2**28 */
		if (hx >= 0x7f800000) {	/* x is inf of NaN */
			return x + x;
		}
		else
			return __CRT_ieee754_logf(x) + ln2f;	/* acosh(huge)=log(2x) */
	}
	else if (hx == 0x3f800000) {
		return 0.0;			/* acosh(1) = 0 */
	}
	else if (hx > 0x40000000) {	/* 2**28 > x > 2 */
		t = x * x;
		return __CRT_ieee754_logf((float)2.0 * x - one / (x + sqrtf(t - one)));
	}
	else {			/* 1<x<2 */
		t = x - one;
		return log1pf(t + sqrtf((float)2.0 * t + t * t));
	}
}
#pragma warning(pop)

long double
_C_DECL
__CRT_ieee754_acoshl(long double x)
{
	return __CRT_ieee754_acosh((double)x);
}