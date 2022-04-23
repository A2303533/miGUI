/* @(#)s_floor.c 5.1 93/09/24 */
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

/*
 * floor(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to floor(x).
 */

#include "float.h"

#include "math.h"
#include "math_private.h"

static const double huge = 1.0e300;

#pragma warning(push)
#pragma warning(disable: 4018)
double
_C_DECL
__CRT_floor(double x)
{
	int32_t i0,i1,j0;
	uint32_t i,j;
	EXTRACT_WORDS(i0,i1,x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;
	if(j0<20) {
	    if(j0<0) { 	/* raise inexact if x != 0 */
		if(huge+x>0.0) {/* return 0*sign(x) if |x|<1 */
		    if(i0>=0) {i0=i1=0;}
		    else if(((i0&0x7fffffff)|i1)!=0)
			{ i0=0xbff00000;i1=0;}
		}
	    } else {
		i = (0x000fffff)>>j0;
		if(((i0&i)|i1)==0) return x; /* x is integral */
		if(huge+x>0.0) {	/* raise inexact flag */
		    if(i0<0) i0 += (0x00100000)>>j0;
		    i0 &= (~i); i1=0;
		}
	    }
	} else if (j0>51) {
	    if(j0==0x400) return x+x;	/* inf or NaN */
	    else return x;		/* x is integral */
	} else {
	    i = ((uint32_t)(0xffffffff))>>(j0-20);
	    if((i1&i)==0) return x;	/* x is integral */
	    if(huge+x>0.0) { 		/* raise inexact flag */
		if(i0<0) {
		    if(j0==20) i0+=1;
		    else {
			j = i1+(1<<(52-j0));
			if(j<i1) i0 +=1 ; 	/* got a carry */
			i1=j;
		    }
		}
		i1 &= (~i);
	    }
	}
	INSERT_WORDS(x,i0,i1);
	return x;
}
#pragma warning(pop)

float
_C_DECL
__CRT_floorf(float x)
{
	int32_t i0, j0;
	uint32_t i;
	GET_FLOAT_WORD(i0, x);
	j0 = ((i0 >> 23) & 0xff) - 0x7f;
	if (j0 < 23) {
		if (j0 < 0) { 	/* raise inexact if x != 0 */
			if (huge + x > (float)0.0) {/* return 0*sign(x) if |x|<1 */
				if (i0 >= 0) { i0 = 0; }
				else if ((i0 & 0x7fffffff) != 0)
				{
					i0 = 0xbf800000;
				}
			}
		}
		else {
			i = (0x007fffff) >> j0;
			if ((i0 & i) == 0) return x; /* x is integral */
			if (huge + x > (float)0.0) {	/* raise inexact flag */
				if (i0 < 0) i0 += (0x00800000) >> j0;
				i0 &= (~i);
			}
		}
	}
	else {
		if (j0 == 0x80) return x + x;	/* inf or NaN */
		else return x;		/* x is integral */
	}
	SET_FLOAT_WORD(x, i0);
	return x;
}

long double
_C_DECL
__CRT_floorl(long double x)
{
	return __CRT_floor((double)x);
}
