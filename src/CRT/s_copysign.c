/* @(#)s_copysign.c 5.1 93/09/24 */
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

/*
 * copysign(double x, double y)
 * copysign(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */

#include "math.h"
#include "math_private.h"

double
_C_DECL
__CRT_copysign(double x, double y)
{
	uint32_t hx,hy;
	GET_HIGH_WORD(hx,x);
	GET_HIGH_WORD(hy,y);
	SET_HIGH_WORD(x,(hx&0x7fffffff)|(hy&0x80000000));
        return x;
}

float
_C_DECL
__CRT_copysignf(float x, float y)
{
	uint32_t ix, iy;
	GET_FLOAT_WORD(ix, x);
	GET_FLOAT_WORD(iy, y);
	SET_FLOAT_WORD(x, (ix & 0x7fffffff) | (iy & 0x80000000));
	return x;
}

long double
_C_DECL
__CRT_copysignl(long double x, long double y)
{
	return __CRT_copysign((double)x, (double)y);
}
