#include "CRT.h"
#include "math.h"
#include "stdint.h"

// FreeBSD : infinity.c

double _C_DECL __CRT_modf(double x, double* iptr);

/*
 * ANSI/POSIX
 */
union __CRT_infinity_un {
	unsigned char	__uc[8];
	double		__ud;
};

union __CRT_nan_un {
	unsigned char	__uc[sizeof(float)];
	float		__uf;
};

/* bytes for +Infinity on a 387 */
const union __CRT_infinity_un __CRT_infinity_ = { { 0, 0, 0, 0, 0, 0, 0xf0, 0x7f } };
/* bytes for NaN */
const union __CRT_nan_un __CRT_nan_ = { { 0, 0, 0xc0, 0xff } };

float 
_C_DECL 
__CRT_nan(void)
{
	return __CRT_nan_.__uf;
}

double 
_C_DECL 
__CRT_infinity(void)
{
	return __CRT_infinity_.__ud;
}


double 
_C_DECL 
modf(double x, double* iptr)
{
	return __CRT_modf(x, iptr);
}

float 
_C_DECL 
modff(float value, float* iptr)
{
	double fr = 0.0;
	float r = (float)__CRT_modf((double)value, &fr);
	*iptr = (float)fr;
	return r;
}

long double 
_C_DECL 
modfl(long double value, long double* iptr)
{
	return __CRT_modf(value, iptr);
}
