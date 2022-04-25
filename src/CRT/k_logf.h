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
 * Float version of k_log.h.  See the latter for most comments.
 */

static const float
/* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
Lgf1 =      0xaaaaaa.0p-24,	/* 0.66666662693 */
Lgf2 =      0xccce13.0p-25,	/* 0.40000972152 */
Lgf3 =      0x91e9ee.0p-25,	/* 0.28498786688 */
Lgf4 =      0xf89e26.0p-26;	/* 0.24279078841 */

static inline float
k_log1pf(float f)
{
	float hfsq,s,z,R,w,t1,t2;

 	s = f/((float)2.0+f);
	z = s*s;
	w = z*z;
	t1= w*(Lgf2+w*Lgf4);
	t2= z*(Lgf1+w*Lgf3);
	R = t2+t1;
	hfsq=(float)0.5*f*f;
	return s*(hfsq+R);
}
