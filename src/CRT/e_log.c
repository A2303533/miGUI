
/* @(#)e_log.c 1.3 95/01/18 */
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

/* __ieee754_log(x)
 * Return the logrithm of x
 *
 * Method :                  
 *   1. Argument Reduction: find k and f such that 
 *			x = 2^k * (1+f), 
 *	   where  sqrt(2)/2 < 1+f < sqrt(2) .
 *
 *   2. Approximation of log(1+f).
 *	Let s = f/(2+f) ; based on log(1+f) = log(1+s) - log(1-s)
 *		 = 2s + 2/3 s**3 + 2/5 s**5 + .....,
 *	     	 = 2s + s*R
 *      We use a special Reme algorithm on [0,0.1716] to generate 
 * 	a polynomial of degree 14 to approximate R The maximum error 
 *	of this polynomial approximation is bounded by 2**-58.45. In
 *	other words,
 *		        2      4      6      8      10      12      14
 *	    R(z) ~ Lg1*s +Lg2*s +Lg3*s +Lg4*s +Lg5*s  +Lg6*s  +Lg7*s
 *  	(the values of Lg1 to Lg7 are listed in the program)
 *	and
 *	    |      2          14          |     -58.45
 *	    | Lg1*s +...+Lg7*s    -  R(z) | <= 2 
 *	    |                             |
 *	Note that 2s = f - s*f = f - hfsq + s*hfsq, where hfsq = f*f/2.
 *	In order to guarantee error in log below 1ulp, we compute log
 *	by
 *		log(1+f) = f - s*(f - R)	(if f is not too large)
 *		log(1+f) = f - (hfsq - s*(hfsq+R)).	(better accuracy)
 *	
 *	3. Finally,  log(x) = k*ln2 + log(1+f).  
 *			    = k*ln2_hi+(f-(hfsq-(s*(hfsq+R)+k*ln2_lo)))
 *	   Here ln2 is split into two floating point number: 
 *			ln2_hi + ln2_lo,
 *	   where n*ln2_hi is always exact for |n| < 2000.
 *
 * Special cases:
 *	log(x) is NaN with signal if x < 0 (including -INF) ; 
 *	log(+INF) is +INF; log(0) is -INF with signal;
 *	log(NaN) is that NaN with no signal.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
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
ln2_hi  =  6.93147180369123816490e-01,	/* 3fe62e42 fee00000 */
ln2_lo  =  1.90821492927058770002e-10,	/* 3dea39ef 35793c76 */
two54   =  1.80143985094819840000e+16,  /* 43500000 00000000 */
Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

static const double zero   =  0.0;
static volatile double vzero = 0.0;

extern const union __CRT_nan_un __CRT_nan_;
extern const union __CRT_infinity_un __CRT_infinity_;

double
_C_DECL
__CRT_ieee754_log(double x)
{
	double hfsq,f,s,z,R,w,t1,t2,dk;
	int32_t k,hx,i,j;
	uint32_t lx;

	EXTRACT_WORDS(hx,lx,x);

	k=0;
	if (hx < 0x00100000) {			/* x < 2**-1022  */
		if (((hx & 0x7fffffff) | lx) == 0)
			return __CRT_infinity_.__ud;// -two54 / vzero;		/* log(+-0)=-inf */
		if (hx < 0) return __CRT_nan_.__uf;// (x - x) / zero;	/* log(-#) = NaN */
	    k -= 54; x *= two54; /* subnormal number, scale up x */
	    GET_HIGH_WORD(hx,x);
	} 
	if (hx >= 0x7ff00000) return x+x;
	k += (hx>>20)-1023;
	hx &= 0x000fffff;
	i = (hx+0x95f64)&0x100000;
	SET_HIGH_WORD(x,hx|(i^0x3ff00000));	/* normalize x or x/2 */
	k += (i>>20);
	f = x-1.0;
	if((0x000fffff&(2+hx))<3) {	/* -2**-20 <= f < 2**-20 */
	    if(f==zero) {
		if(k==0) {
		    return zero;
		} else {
		    dk=(double)k;
		    return dk*ln2_hi+dk*ln2_lo;
		}
	    }
	    R = f*f*(0.5-0.33333333333333333*f);
	    if(k==0) return f-R; else {dk=(double)k;
	    	     return dk*ln2_hi-((R-dk*ln2_lo)-f);}
	}
 	s = f/(2.0+f); 
	dk = (double)k;
	z = s*s;
	i = hx-0x6147a;
	w = z*z;
	j = 0x6b851-hx;
	t1= w*(Lg2+w*(Lg4+w*Lg6)); 
	t2= z*(Lg1+w*(Lg3+w*(Lg5+w*Lg7))); 
	i |= j;
	R = t2+t1;
	if(i>0) {
	    hfsq=0.5*f*f;
	    if(k==0) return f-(hfsq-s*(hfsq+R)); else
		     return dk*ln2_hi-((hfsq-(s*(hfsq+R)+dk*ln2_lo))-f);
	} else {
	    if(k==0) return f-s*(f-R); else
		     return dk*ln2_hi-((s*(f-R)-dk*ln2_lo)-f);
	}
}

#include "k_logf.h"

#pragma warning(push)
#pragma warning(disable: 4305)
static const float
two25 = 3.3554432000e+07, /* 0x4c000000 */
ivln2hi = 1.4428710938e+00, /* 0x3fb8b000 */
ivln2lo = -1.7605285393e-04; /* 0xb9389ad4 */

static const float
ln2_hi_f = 6.9313812256e-01,	/* 0x3f317180 */
ln2_lo_f = 9.0580006145e-06,	/* 0x3717f7d1 */
two25_f = 3.355443200e+07,	/* 0x4c000000 */
/* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
Lg1_f = 0xaaaaaa.0p-24,	/* 0.66666662693 */
Lg2_f = 0xccce13.0p-25,	/* 0.40000972152 */
Lg3_f = 0x91e9ee.0p-25,	/* 0.28498786688 */
Lg4_f = 0xf89e26.0p-26;	/* 0.24279078841 */
#pragma warning(pop)

extern const union __CRT_nan_un __CRT_nan_;
extern const union __CRT_infinity_un __CRT_infinity_;

#pragma warning(push)
#pragma warning(disable: 4244)
float
_C_DECL
__CRT_ieee754_logf(float x)
{
	float hfsq, f, s, z, R, w, t1, t2, dk;
	int32_t k, ix, i, j;

	GET_FLOAT_WORD(ix, x);

	k = 0;
	if (ix < 0x00800000) {			/* x < 2**-126  */
		if ((ix & 0x7fffffff) == 0)
			return __CRT_infinity_.__ud;// -two25 / vzero;		/* log(+-0)=-inf */
		if (ix < 0) return __CRT_nan_.__uf;// (x - x) / zero;	/* log(-#) = NaN */
		k -= 25; x *= two25; /* subnormal number, scale up x */
		GET_FLOAT_WORD(ix, x);
	}
	if (ix >= 0x7f800000) return x + x;
	k += (ix >> 23) - 127;
	ix &= 0x007fffff;
	i = (ix + (0x95f64 << 3)) & 0x800000;
	SET_FLOAT_WORD(x, ix | (i ^ 0x3f800000));	/* normalize x or x/2 */
	k += (i >> 23);
	f = x - (float)1.0;
	if ((0x007fffff & (0x8000 + ix)) < 0xc000) {	/* -2**-9 <= f < 2**-9 */
		if (f == zero) {
			if (k == 0) {
				return zero;
			}
			else {
				dk = (float)k;
				return dk * ln2_hi_f + dk * ln2_lo_f;
			}
		}
		R = f * f * ((float)0.5 - (float)0.33333333333333333 * f);
		if (k == 0) return f - R; else {
			dk = (float)k;
			return dk * ln2_hi_f - ((R - dk * ln2_lo_f) - f);
		}
	}
	s = f / ((float)2.0 + f);
	dk = (float)k;
	z = s * s;
	i = ix - (0x6147a << 3);
	w = z * z;
	j = (0x6b851 << 3) - ix;
	t1 = w * (Lg2_f + w * Lg4_f);
	t2 = z * (Lg1_f + w * Lg3_f);
	i |= j;
	R = t2 + t1;
	if (i > 0) {
		hfsq = (float)0.5 * f * f;
		if (k == 0) return f - (hfsq - s * (hfsq + R)); else
			return dk * ln2_hi_f - ((hfsq - (s * (hfsq + R) + dk * ln2_lo_f)) - f);
	}
	else {
		if (k == 0) return f - s * (f - R); else
			return dk * ln2_hi_f - ((s * (f - R) - dk * ln2_lo_f) - f);
	}
}
#pragma warning(pop)

long double
_C_DECL
__CRT_ieee754_logl(long double x)
{
	return __CRT_ieee754_log(x);
}