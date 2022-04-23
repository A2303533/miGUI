#include "CRT.h"
#include "math.h"
#include "stdint.h"

extern __CRT_main_struct __crt;

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
acos(double x)
{
	return __CRT_ieee754_acos(x);
}

float 
_C_DECL 
acosf(float x)
{
	return __CRT_ieee754_acosf(x);
}

long double 
_C_DECL 
acosl(long double x)
{
	return __CRT_acosl(x);
}

double 
_C_DECL 
asin(double x)
{
	return __CRT_ieee754_asin(x);
}

float 
_C_DECL 
asinf(float x)
{
	return __CRT_ieee754_asinf(x);
}

long double 
_C_DECL 
asinl(long double x)
{
	return __CRT_asinl(x);
}

double 
_C_DECL 
atan(double x)
{
	return __CRT_atan(x);
}

float 
_C_DECL 
atanf(float x)
{
	return __CRT_atanf(x);
}

long double 
_C_DECL 
atanl(long double x)
{
	return __CRT_atanl(x);
}

double 
_C_DECL 
tan(double x)
{
	return __CRT_tan(x);
}

float 
_C_DECL 
tanf(float x)
{
	return __CRT_tanf(x);
}

long double 
_C_DECL 
tanl(long double x)
{
	return __CRT_tanl(x);
}

double 
_C_DECL 
fabs(double x)
{
	return __CRT_fabs(x);
}

float 
_C_DECL 
fabsf(float x)
{
	return __CRT_fabsf(x);
}

long double 
_C_DECL 
fabsl(long double x)
{
	return __CRT_fabsl(x);
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

double 
_C_DECL 
scalbn(double x, int n)
{
	return __CRT_scalbn(x, n);
}

float 
_C_DECL 
scalbnf(float x, int n)
{
	return __CRT_scalbnf(x, n);
}

long double 
_C_DECL 
scalbnl(long double x, int n)
{
	return __CRT_scalbnl(x, n);
}

double 
_C_DECL 
scalbln(double x, int n)
{
	return __CRT_scalbln(x, n);
}

float 
_C_DECL 
scalblnf(float x, int n)
{
	return __CRT_scalblnf(x, n);
}

long double 
_C_DECL 
scalblnl(long double x, int n)
{
	return __CRT_scalblnl(x, n);
}

double 
_C_DECL 
sqrt(double x)
{
	return __CRT_ieee754_sqrt(x);
}

float 
_C_DECL 
sqrtf(float x)
{
	return __CRT_ieee754_sqrtf(x);
}

long double 
_C_DECL 
sqrtl(long double x)
{
	return __CRT_sqrtl(x);
}

double 
_C_DECL 
floor(double x)
{
	return __CRT_floor(x);
}

float 
_C_DECL 
floorf(float x)
{
	return __CRT_floorf(x);
}

long double 
_C_DECL 
floorl(long double x)
{
	return __CRT_floorl(x);
}


