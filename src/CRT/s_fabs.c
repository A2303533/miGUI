/* @(#)s_fabs.c 5.1 93/09/24 */
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
 * fabs(x) returns the absolute value of x.
 */

#include "math.h"
#include "math_private.h"

double
__CRT_fabs(double x)
{
	uint32_t high;
	GET_HIGH_WORD(high,x);
	SET_HIGH_WORD(x,high&0x7fffffff);
        return x;
}

float
__CRT_fabsf(float x)
{
	uint32_t ix;
	GET_FLOAT_WORD(ix, x);
	SET_FLOAT_WORD(x, ix & 0x7fffffff);
	return x;
}

long double
__CRT_fabsl(long double x)
{
	return fabs(x);
}