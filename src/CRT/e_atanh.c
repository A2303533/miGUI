
/* @(#)e_atanh.c 1.3 95/01/18 */
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

/* __ieee754_atanh(x)
 * Method :
 *    1.Reduced x to positive by atanh(-x) = -atanh(x)
 *    2.For x>=0.5
 *                  1              2x                          x
 *	atanh(x) = --- * log(1 + -------) = 0.5 * log1p(2 * --------)
 *                  2             1 - x                      1 - x
 *	
 * 	For x<0.5
 *	atanh(x) = 0.5*log1p(2x+2x*x/(1-x))
 *
 * Special cases:
 *	atanh(x) is NaN if |x| > 1 with signal;
 *	atanh(NaN) is that NaN with no signal;
 *	atanh(+-1) is +-INF with signal.
 *
 */

#include "float.h"

#include "math.h"
#include "math_private.h"

static const double one = 1.0, huge = 1e300;
static const double zero = 0.0;

extern const union __CRT_nan_un __CRT_nan_;
extern const union __CRT_infinity_un __CRT_infinity_;

#pragma warning(push)
#pragma warning(disable: 4146)
double
_C_DECL
__CRT_ieee754_atanh(double x)
{
	double t;
	int32_t hx,ix;
	uint32_t lx;
	EXTRACT_WORDS(hx,lx,x);
	ix = hx&0x7fffffff;
	if ((ix|((lx|(-lx))>>31))>0x3ff00000) /* |x|>1 */
	    return (x-x)/(x-x);
	if(ix==0x3ff00000) 
	    return __CRT_nan_.__uf;// x/zero; // NAN? __CRT_nan_
	if(ix<0x3e300000&&(huge+x)>zero) return x;	/* x<2**-28 */
	SET_HIGH_WORD(x,ix);
	if(ix<0x3fe00000) {		/* x < 0.5 */
	    t = x+x;
	    t = 0.5*log1p(t+t*x/(one-x));
	} else 
	    t = 0.5*log1p((x+x)/(one-x));
	if(hx>=0) return t; else return -t;
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4244)
float
_C_DECL
__CRT_ieee754_atanhf(float x)
{
	float t;
	int32_t hx, ix;
	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix > 0x3f800000) 		/* |x|>1 */
		return (x - x) / (x - x);
	if (ix == 0x3f800000)
		return __CRT_nan_.__uf;// x / zero;  // NAN? __CRT_nan_
	if (ix<0x31800000 && (huge + x)>zero) return x;	/* x<2**-28 */
	SET_FLOAT_WORD(x, ix);
	if (ix < 0x3f000000) {		/* x < 0.5 */
		t = x + x;
		t = (float)0.5 * log1pf(t + t * x / (one - x));
	}
	else
		t = (float)0.5 * log1pf((x + x) / (one - x));
	if (hx >= 0) return t; else return -t;
}
#pragma warning(pop)

long double
_C_DECL
__CRT_ieee754_atanhl(long double x)
{
	return __CRT_ieee754_atanh((double)x);
}
