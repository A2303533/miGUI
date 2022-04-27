
/* @(#)e_exp.c 1.6 04/04/22 */
/*
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#include "CRT.h"

/* __ieee754_exp(x)
 * Returns the exponential of x.
 *
 * Method
 *   1. Argument reduction:
 *      Reduce x to an r so that |r| <= 0.5*ln2 ~ 0.34658.
 *	Given x, find r and integer k such that
 *
 *               x = k*ln2 + r,  |r| <= 0.5*ln2.  
 *
 *      Here r will be represented as r = hi-lo for better 
 *	accuracy.
 *
 *   2. Approximation of exp(r) by a special rational function on
 *	the interval [0,0.34658]:
 *	Write
 *	    R(r**2) = r*(exp(r)+1)/(exp(r)-1) = 2 + r*r/6 - r**4/360 + ...
 *      We use a special Remes algorithm on [0,0.34658] to generate 
 * 	a polynomial of degree 5 to approximate R. The maximum error 
 *	of this polynomial approximation is bounded by 2**-59. In
 *	other words,
 *	    R(z) ~ 2.0 + P1*z + P2*z**2 + P3*z**3 + P4*z**4 + P5*z**5
 *  	(where z=r*r, and the values of P1 to P5 are listed below)
 *	and
 *	    |                  5          |     -59
 *	    | 2.0+P1*z+...+P5*z   -  R(z) | <= 2 
 *	    |                             |
 *	The computation of exp(r) thus becomes
 *                             2*r
 *		exp(r) = 1 + -------
 *		              R - r
 *                                 r*R1(r)	
 *		       = 1 + r + ----------- (for better accuracy)
 *		                  2 - R1(r)
 *	where
 *			         2       4             10
 *		R1(r) = r - (P1*r  + P2*r  + ... + P5*r   ).
 *	
 *   3. Scale back to obtain exp(x):
 *	From step 1, we have
 *	   exp(x) = 2^k * exp(r)
 *
 * Special cases:
 *	exp(INF) is INF, exp(NaN) is NaN;
 *	exp(-INF) is 0, and
 *	for finite argument, only exp(0)=1 is exact.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Misc. info.
 *	For IEEE double 
 *	    if x >  7.09782712893383973096e+02 then exp(x) overflow
 *	    if x < -7.45133219101941108420e+02 then exp(x) underflow
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */

#include "float.h"

#include "math.h"
#include "math_private.h"

static const double
one	= 1.0,
halF[2]	= {0.5,-0.5,},
o_threshold=  7.09782712893383973096e+02,  /* 0x40862E42, 0xFEFA39EF */
u_threshold= -7.45133219101941108420e+02,  /* 0xc0874910, 0xD52D3051 */
ln2HI[2]   ={ 6.93147180369123816490e-01,  /* 0x3fe62e42, 0xfee00000 */
	     -6.93147180369123816490e-01,},/* 0xbfe62e42, 0xfee00000 */
ln2LO[2]   ={ 1.90821492927058770002e-10,  /* 0x3dea39ef, 0x35793c76 */
	     -1.90821492927058770002e-10,},/* 0xbdea39ef, 0x35793c76 */
invln2 =  1.44269504088896338700e+00, /* 0x3ff71547, 0x652b82fe */
P1   =  1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
P2   = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
P3   =  6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
P4   = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
P5   =  4.13813679705723846039e-08; /* 0x3E663769, 0x72BEA4D0 */

static volatile double
huge	= 1.0e+300,
twom1000= 9.33263618503218878990e-302;     /* 2**-1000=0x01700000,0*/

double
_C_DECL
__CRT_ieee754_exp(double x)	/* default IEEE double exp */
{
	double y,hi=0.0,lo=0.0,c,t,twopk;
	int32_t k=0,xsb;
	uint32_t hx;

	GET_HIGH_WORD(hx,x);
	xsb = (hx>>31)&1;		/* sign bit of x */
	hx &= 0x7fffffff;		/* high word of |x| */

    /* filter out non-finite argument */
	if(hx >= 0x40862E42) {			/* if |x|>=709.78... */
            if(hx>=0x7ff00000) {
	        uint32_t lx;
		GET_LOW_WORD(lx,x);
		if(((hx&0xfffff)|lx)!=0)
		     return x+x; 		/* NaN */
		else return (xsb==0)? x:0.0;	/* exp(+-inf)={inf,0} */
	    }
	    if(x > o_threshold) return huge*huge; /* overflow */
	    if(x < u_threshold) return twom1000*twom1000; /* underflow */
	}

    /* argument reduction */
	if(hx > 0x3fd62e42) {		/* if  |x| > 0.5 ln2 */ 
	    if(hx < 0x3FF0A2B2) {	/* and |x| < 1.5 ln2 */
		hi = x-ln2HI[xsb]; lo=ln2LO[xsb]; k = 1-xsb-xsb;
	    } else {
		k  = (int)(invln2*x+halF[xsb]);
		t  = k;
		hi = x - t*ln2HI[0];	/* t*ln2HI is exact here */
		lo = t*ln2LO[0];
	    }
	    STRICT_ASSIGN(double, x, hi - lo);
	} 
	else if(hx < 0x3e300000)  {	/* when |x|<2**-28 */
	    if(huge+x>one) return one+x;/* trigger inexact */
	}
	else k = 0;

    /* x is now in primary range */
	t  = x*x;
	if(k >= -1021)
	    INSERT_WORDS(twopk,((uint32_t)(0x3ff+k))<<20, 0);
	else
	    INSERT_WORDS(twopk,((uint32_t)(0x3ff+(k+1000)))<<20, 0);
	c  = x - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
	if(k==0) 	return one-((x*c)/(c-2.0)-x); 
	else 		y = one-((lo-(x*c)/(2.0-c))-hi);
	if(k >= -1021) {
	    if (k==1024) return y*2.0*0x1p1023;
	    return y*twopk;
	} else {
	    return y*twopk*twom1000;
	}
}

#pragma warning(push)
#pragma warning(disable: 4305)
static const float
o_threshold_f = 8.8721679688e+01,  /* 0x42b17180 */
u_threshold_f = -1.0397208405e+02,  /* 0xc2cff1b5 */
ln2HI_f[2] = { 6.9314575195e-01,		/* 0x3f317200 */
		 -6.9314575195e-01, },	/* 0xbf317200 */
	ln2LO_f[2] = { 1.4286067653e-06,  	/* 0x35bfbe8e */
			 -1.4286067653e-06, },	/* 0xb5bfbe8e */
	invln2_f = 1.4426950216e+00, 		/* 0x3fb8aa3b */
	/*
	 * Domain [-0.34568, 0.34568], range ~[-4.278e-9, 4.447e-9]:
	 * |x*(exp(x)+1)/(exp(x)-1) - p(x)| < 2**-27.74
	 */
	P1_f = 1.6666625440e-1,		/*  0xaaaa8f.0p-26 */
	P2_f = -2.7667332906e-3;		/* -0xb55215.0p-32 */

static volatile float
huge_f = 1.0e+30,
twom100_f = 7.8886090522e-31;      /* 2**-100=0x0d800000 */
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4244)
float
_C_DECL
__CRT_ieee754_expf(float x)
{
	float y, hi = 0.0, lo = 0.0, c, t, twopk;
	int32_t k = 0, xsb;
	uint32_t hx;

	GET_FLOAT_WORD(hx, x);
	xsb = (hx >> 31) & 1;		/* sign bit of x */
	hx &= 0x7fffffff;		/* high word of |x| */

	/* filter out non-finite argument */
	if (hx >= 0x42b17218) {			/* if |x|>=88.721... */
		if (hx > 0x7f800000)
			return x + x;	 		/* NaN */
		if (hx == 0x7f800000)
			return (xsb == 0) ? x : 0.0;		/* exp(+-inf)={inf,0} */
		if (x > o_threshold_f) return huge_f * huge_f; /* overflow */
		if (x < u_threshold_f) return twom100_f * twom100_f; /* underflow */
	}

	/* argument reduction */
	if (hx > 0x3eb17218) {		/* if  |x| > 0.5 ln2 */
		if (hx < 0x3F851592) {	/* and |x| < 1.5 ln2 */
			hi = x - ln2HI_f[xsb]; lo = ln2LO_f[xsb]; k = 1 - xsb - xsb;
		}
		else {
			k = invln2_f * x + halF[xsb];
			t = k;
			hi = x - t * ln2HI_f[0];	/* t*ln2HI is exact here */
			lo = t * ln2LO_f[0];
		}
		STRICT_ASSIGN(float, x, hi - lo);
	}
	else if (hx < 0x39000000) {	/* when |x|<2**-14 */
		if (huge_f + x > one) return one + x;/* trigger inexact */
	}
	else k = 0;

	/* x is now in primary range */
	t = x * x;
	if (k >= -125)
		SET_FLOAT_WORD(twopk, ((uint32_t)(0x7f + k)) << 23);
	else
		SET_FLOAT_WORD(twopk, ((uint32_t)(0x7f + (k + 100))) << 23);
	c = x - t * (P1_f + t * P2_f);
	if (k == 0) 	return one - ((x * c) / (c - (float)2.0) - x);
	else 		y = one - ((lo - (x * c) / ((float)2.0 - c)) - hi);
	if (k >= -125) {
		if (k == 128) return y * 2.0F * 0x1p127F;
		return y * twopk;
	}
	else {
		return y * twopk * twom100_f;
	}
}
#pragma warning(pop)

long double
_C_DECL
__CRT_ieee754_expl(long double x)
{
	return __CRT_ieee754_exp((double)x);
}
