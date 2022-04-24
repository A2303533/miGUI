/* @(#)s_cos.c 5.1 93/09/24 */
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

#include "CRT.h"

/* cos(x)
 * Return cosine function of x.
 *
 * kernel function:
 *	__kernel_sin		... sine function on [-pi/4,pi/4]
 *	__kernel_cos		... cosine function on [-pi/4,pi/4]
 *	__ieee754_rem_pio2	... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *	[-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *	in [-pi/4 , +pi/4], and let n = k mod 4.
 *	We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *	    0	       S	   C		 T
 *	    1	       C	  -S		-1/T
 *	    2	      -S	  -C		 T
 *	    3	      -C	   S		-1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *	TRIG(x) returns trig(x) nearly rounded
 */

#include "float.h"

#include "math.h"
//#define INLINE_REM_PIO2
#include "math_private.h"
//#include "e_rem_pio2.c"

int __ieee754_rem_pio2(double x, double* y);
int __ieee754_rem_pio2f(float x, double* y);
double __kernel_cos(double x, double y);
double __kernel_sin(double x, double y, int iy);
float __kernel_cosdf(double x);
float __kernel_sindf(double x);

double
_C_DECL
__CRT_cos(double x)
{
	double y[2],z=0.0;
	int32_t n, ix;

    /* High word of x. */
	GET_HIGH_WORD(ix,x);

    /* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if(ix <= 0x3fe921fb) {
	    if(ix<0x3e46a09e)			/* if x < 2**-27 * sqrt(2) */
		if(((int)x)==0) return 1.0;	/* generate inexact */
	    return __kernel_cos(x,z);
	}

    /* cos(Inf or NaN) is NaN */
	else if (ix>=0x7ff00000) return x-x;

    /* argument reduction needed */
	else {
	    n = __ieee754_rem_pio2(x,y);
	    switch(n&3) {
		case 0: return  __kernel_cos(y[0],y[1]);
		case 1: return -__kernel_sin(y[0],y[1],1);
		case 2: return -__kernel_cos(y[0],y[1]);
		default:
		        return  __kernel_sin(y[0],y[1],1);
	    }
	}
}

/* Small multiples of pi/2 rounded to double precision. */
static const double
c1pio2 = 1 * M_PI_2,			/* 0x3FF921FB, 0x54442D18 */
c2pio2 = 2 * M_PI_2,			/* 0x400921FB, 0x54442D18 */
c3pio2 = 3 * M_PI_2,			/* 0x4012D97C, 0x7F3321D2 */
c4pio2 = 4 * M_PI_2;			/* 0x401921FB, 0x54442D18 */

float
_C_DECL
__CRT_cosf(float x)
{
	double y;
	int32_t n, hx, ix;

	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;

	if (ix <= 0x3f490fda) {		/* |x| ~<= pi/4 */
		if (ix < 0x39800000)		/* |x| < 2**-12 */
			if (((int)x) == 0) return 1.0;	/* 1 with inexact if x != 0 */
		return __kernel_cosdf(x);
	}
	if (ix <= 0x407b53d1) {		/* |x| ~<= 5*pi/4 */
		if (ix > 0x4016cbe3)		/* |x|  ~> 3*pi/4 */
			return -__kernel_cosdf(x + (hx > 0 ? -c2pio2 : c2pio2));
		else {
			if (hx > 0)
				return __kernel_sindf(c1pio2 - x);
			else
				return __kernel_sindf(x + c1pio2);
		}
	}
	if (ix <= 0x40e231d5) {		/* |x| ~<= 9*pi/4 */
		if (ix > 0x40afeddf)		/* |x|  ~> 7*pi/4 */
			return __kernel_cosdf(x + (hx > 0 ? -c4pio2 : c4pio2));
		else {
			if (hx > 0)
				return __kernel_sindf(x - c3pio2);
			else
				return __kernel_sindf(-c3pio2 - x);
		}
	}

	/* cos(Inf or NaN) is NaN */
	else if (ix >= 0x7f800000) return x - x;

	/* general argument reduction needed */
	else {
		n = __ieee754_rem_pio2f(x, &y);
		switch (n & 3) {
		case 0: return  __kernel_cosdf(y);
		case 1: return  __kernel_sindf(-y);
		case 2: return -__kernel_cosdf(y);
		default:
			return  __kernel_sindf(y);
		}
	}
}

long double
_C_DECL
__CRT_cosl(long double x)
{
	return __CRT_cos((double)x);
}
