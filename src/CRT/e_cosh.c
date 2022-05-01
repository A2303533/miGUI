
/* @(#)e_cosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#include "CRT.h"

/* __ieee754_cosh(x)
 * Method : 
 * mathematically cosh(x) if defined to be (exp(x)+exp(-x))/2
 *	1. Replace x by |x| (cosh(x) = cosh(-x)). 
 *	2. 
 *		                                        [ exp(x) - 1 ]^2 
 *	    0        <= x <= ln2/2  :  cosh(x) := 1 + -------------------
 *			       			           2*exp(x)
 *
 *		                                  exp(x) +  1/exp(x)
 *	    ln2/2    <= x <= 22     :  cosh(x) := -------------------
 *			       			          2
 *	    22       <= x <= lnovft :  cosh(x) := exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  cosh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only cosh(0)=1 is exact for finite x.
 */

#include "float.h"

#include "math.h"
#include "math_private.h"

static const double one = 1.0, half=0.5, huge = 1.0e300;

double
_C_DECL
__CRT_ieee754_cosh(double x)
{
	double t,w;
	int32_t ix;

    /* High word of |x|. */
	GET_HIGH_WORD(ix,x);
	ix &= 0x7fffffff;

    /* x is INF or NaN */
	if(ix>=0x7ff00000) return x*x;	

    /* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
	if(ix<0x3fd62e43) {
	    t = expm1(fabs(x));
	    w = one+t;
	    if (ix<0x3c800000) return w;	/* cosh(tiny) = 1 */
	    return one+(t*t)/(w+w);
	}

    /* |x| in [0.5*ln2,22], return (exp(|x|)+1/exp(|x|)/2; */
	if (ix < 0x40360000) {
		t = __CRT_ieee754_exp(fabs(x));
		return half*t+half/t;
	}

    /* |x| in [22, log(maxdouble)] return half*exp(|x|) */
	if (ix < 0x40862E42)  return half*__CRT_ieee754_exp(fabs(x));

    /* |x| in [log(maxdouble), overflowthresold] */
	if (ix<=0x408633CE)
	    return __ldexp_exp(fabs(x), -1);

    /* |x| > overflowthresold, cosh(x) overflow */
	return huge*huge;
}

#pragma warning(push)
#pragma warning(disable: 4244)
float
_C_DECL
__CRT_ieee754_coshf(float x)
{
	float t, w;
	int32_t ix;

	GET_FLOAT_WORD(ix, x);
	ix &= 0x7fffffff;

	/* x is INF or NaN */
	if (ix >= 0x7f800000) return x * x;

	/* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
	if (ix < 0x3eb17218) {
		t = expm1f(fabsf(x));
		w = one + t;
		if (ix < 0x39800000) return one;	/* cosh(tiny) = 1 */
		return one + (t * t) / (w + w);
	}

	/* |x| in [0.5*ln2,9], return (exp(|x|)+1/exp(|x|))/2; */
	if (ix < 0x41100000) {
		t = __CRT_ieee754_expf(fabsf(x));
		return half * t + half / t;
	}

	/* |x| in [9, log(maxfloat)] return half*exp(|x|) */
	if (ix < 0x42b17217)  return half * __CRT_ieee754_expf(fabsf(x));

	/* |x| in [log(maxfloat), overflowthresold] */
	if (ix <= 0x42b2d4fc)
		return __ldexp_expf(fabsf(x), -1);

	/* |x| > overflowthresold, cosh(x) overflow */
	return huge * huge;
}
#pragma warning(pop)

long double
_C_DECL
__CRT_ieee754_coshl(long double x)
{
	return __CRT_ieee754_cosh(x);
}
