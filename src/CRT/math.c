#include "CRT.h"
#include "math.h"
#include "stdint.h"
#include "string.h"

#ifdef __CRT_INTRIN
#include <intrin.h>
#endif

extern __CRT_main_struct __crt;

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
atan2(double y, double x)
{
	return __CRT_ieee754_atan2(y, x);
}

float 
_C_DECL 
atan2f(float y, float x)
{
	return __CRT_ieee754_atan2f(y, x);
}

long double 
_C_DECL 
atan2l(long double y, long double x)
{
	return __CRT_ieee754_atan2l(y, x);
}

double 
_C_DECL 
cos(double x)
{
	return __CRT_cos(x);
}

float 
_C_DECL 
cosf(float x)
{
	return __CRT_cosf(x);
}

long double 
_C_DECL 
cosl(long double x)
{
	return __CRT_cosl(x);
}

double 
_C_DECL 
sin(double x)
{
	return __CRT_sin(x);
}

float 
_C_DECL 
sinf(float x)
{
	return __CRT_sinf(x);
}

long double 
_C_DECL 
sinl(long double x)
{
	return __CRT_sinl(x);
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
acosh(double x)
{
	return __CRT_ieee754_acosh(x);
}

float 
_C_DECL 
acoshf(float x)
{
	return __CRT_ieee754_acoshf(x);
}

long double 
_C_DECL 
acoshl(long double x)
{
	return __CRT_ieee754_acoshl(x);
}

double 
_C_DECL 
asinh(double x)
{
	return __CRT_asinh(x);
}

float 
_C_DECL 
asinhf(float x)
{
	return __CRT_asinhf(x);
}

long double 
_C_DECL 
asinhl(long double x)
{
	return __CRT_asinhl(x);
}

double 
_C_DECL 
atanh(double x)
{
	return __CRT_ieee754_atanh(x);
}

float 
_C_DECL 
atanhf(float x)
{
	return __CRT_ieee754_atanhf(x);
}

long double 
_C_DECL 
atanhl(long double x)
{
	return __CRT_ieee754_atanhl(x);
}

double 
_C_DECL 
cosh(double x)
{
	return __CRT_ieee754_cosh(x);
}

float 
_C_DECL 
coshf(float x)
{
	return __CRT_ieee754_coshf(x);
}

long double 
_C_DECL 
coshl(long double x)
{
	return __CRT_ieee754_coshl(x);
}

double 
_C_DECL 
sinh(double x)
{
	return __CRT_ieee754_sinh(x);
}

float 
_C_DECL 
sinhf(float x)
{
	return __CRT_ieee754_sinhf(x);
}

long double 
_C_DECL 
sinhl(long double x)
{
	return __CRT_ieee754_sinhl(x);
}

double 
_C_DECL 
exp(double x)
{
	return __CRT_ieee754_exp(x);
}

float 
_C_DECL 
expf(float x)
{
	return __CRT_ieee754_expf(x);
}

long double 
_C_DECL 
expl(long double x)
{
	return __CRT_ieee754_expl(x);
}

double 
_C_DECL 
exp2(double x)
{
	return __CRT_exp2(x);
}

float 
_C_DECL 
exp2f(float x)
{
	return __CRT_exp2f(x);
}

long double 
_C_DECL 
exp2l(long double x)
{
	return __CRT_exp2l(x);
}

double 
_C_DECL 
expm1(double x)
{
	return __CRT_expm1(x);
}

float 
_C_DECL 
expm1f(float x)
{
	return __CRT_expm1f(x);
}

long double 
_C_DECL 
expm1l(long double x)
{
	return __CRT_expm1l(x);
}

double 
_C_DECL 
frexp(double value, int* exp)
{
	return __CRT_frexp(value, exp);
}

float 
_C_DECL 
frexpf(float value, int* exp)
{
	return __CRT_frexpf(value, exp);
}

long double 
_C_DECL 
frexpl(long double value, int* exp)
{
	return __CRT_frexpl(value, exp);
}

int 
_C_DECL 
ilogb(double x)
{
	return __CRT_ilogb(x);
}

int 
_C_DECL 
ilogbf(float x)
{
	return __CRT_ilogbf(x);
}

int 
_C_DECL 
ilogbl(long double x)
{
	return __CRT_ilogbl(x);
}

double 
_C_DECL 
ldexp(double value, int exp)
{
	return scalbln(value, exp);
}

float 
_C_DECL 
ldexpf(float value, int exp)
{
	return scalblnf(value, exp);
}

long double 
_C_DECL 
ldexpl(long double value, int exp)
{
	return scalblnl(value, exp);
}

double 
_C_DECL 
log(double x)
{
	return __CRT_ieee754_log(x);
}

float 
_C_DECL 
logf(float x)
{
	return __CRT_ieee754_logf(x);
}

long double 
_C_DECL 
logl(long double x)
{
	return __CRT_ieee754_logl(x);
}

double 
_C_DECL 
log10(double x)
{
	return __CRT_ieee754_log10(x);
}

float 
_C_DECL 
log10f(float x)
{
	return __CRT_ieee754_log10f(x);
}

long double 
_C_DECL 
log10l(long double x)
{
	return __CRT_ieee754_log10l(x);
}

double 
_C_DECL 
log1p(double x)
{
	return __CRT_log1p(x);
}

float 
_C_DECL 
log1pf(float x)
{
	return __CRT_log1pf(x);
}

long double 
_C_DECL 
log1pl(long double x)
{
	return __CRT_log1pl(x);
}

double 
_C_DECL 
log2(double x)
{
	return __CRT_ieee754_log2(x);
}

float 
_C_DECL 
log2f(float x)
{
	return __CRT_ieee754_log2f(x);
}

long double 
_C_DECL 
log2l(long double x)
{
	return __CRT_ieee754_log2l(x);
}

double 
_C_DECL 
logb(double x)
{
	return __CRT_logb(x);
}

float 
_C_DECL 
logbf(float x)
{
	return __CRT_logbf(x);
}

long double 
_C_DECL 
logbl(long double x)
{
	return __CRT_logbl(x);
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
hypot(double x, double y)
{
	return __CRT_ieee754_hypot(x, y);
}

float 
_C_DECL 
hypotf(float x, float y)
{
	return __CRT_ieee754_hypotf(x, y);
}

long double 
_C_DECL 
hypotl(long double x, long double y)
{
	return __CRT_ieee754_hypotl(x, y);
}

double 
_C_DECL 
pow(double x, double y)
{
	return __CRT_ieee754_pow(x, y);
}

float 
_C_DECL 
powf(float x, float y)
{
	return __CRT_ieee754_powf(x, y);
}

long double 
_C_DECL 
powl(long double x, long double y)
{
	return __CRT_ieee754_powl(x, y);
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
cbrt(double x)
{
	return __CRT_cbrt(x);
}

float 
_C_DECL 
cbrtf(float x)
{
	return __CRT_cbrtf(x);
}

long double 
_C_DECL 
cbrtl(long double x)
{
	return __CRT_cbrtl(x);
}

double 
_C_DECL 
sqrt(double x)
{
#ifdef __CRT_INTRIN
	__m128d d;
	d.m128d_f64[0] = x;
	d.m128d_f64[1] = 0;

	__m128d d2 = _mm_sqrt_pd(d);
	return d2.m128d_f64[0];
#else
	return __CRT_ieee754_sqrt(x);
#endif
}

//#define __CRT_INTRIN

float 
_C_DECL 
sqrtf(float x)
{
#ifdef __CRT_INTRIN
	__m128d d;
	d.m128d_f64[0] = x;
	d.m128d_f64[1] = 0;

	__m128d d2 = _mm_sqrt_pd(d);
	return d2.m128d_f64[0];
#else
	return __CRT_ieee754_sqrtf(x);
#endif
}

long double 
_C_DECL 
sqrtl(long double x)
{
#ifdef __CRT_INTRIN
	__m128d d;
	d.m128d_f64[0] = x;
	d.m128d_f64[1] = 0;

	__m128d d2 = _mm_sqrt_pd(d);
	return d2.m128d_f64[0];
#else
	return __CRT_sqrtl(x);
#endif
}

double 
_C_DECL 
erf(double x)
{
	return __CRT_erf(x);
}

float 
_C_DECL 
erff(float x)
{
	return __CRT_erff(x);
}

long double 
_C_DECL 
erfl(long double x)
{
	return __CRT_erfl(x);
}

double 
_C_DECL 
erfc(double x)
{
	return __CRT_erfc(x);
}

float 
_C_DECL 
erfcf(float x)
{
	return __CRT_erfcf(x);
}

long double 
_C_DECL 
erfcl(long double x)
{
	return __CRT_erfcl(x);
}

double 
_C_DECL 
lgamma(double x)
{
	return __CRT_ieee754_lgamma(x);
}

float 
_C_DECL 
lgammaf(float x)
{
	return __CRT_ieee754_lgammaf(x);
}

long double 
_C_DECL 
lgammal(long double x)
{
	return __CRT_ieee754_lgammal(x);
}

double 
_C_DECL 
tgamma(double x)
{
	return __CRT_tgamma((double)x);
}

float 
_C_DECL 
tgammaf(float x)
{
	return (float)tgamma((double)x);
}

long double 
_C_DECL 
tgammal(long double x)
{
	return tgamma((double)x);
}

double 
_C_DECL 
ceil(double x)
{
	return __CRT_ceil(x);
}

float 
_C_DECL 
ceilf(float x)
{
	return __CRT_ceilf(x);
}

long double 
_C_DECL 
ceill(long double x)
{
	return __CRT_ceill(x);
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

double 
_C_DECL 
nearbyint(double x)
{
	return rint(x);
}

float 
_C_DECL 
nearbyintf(float x)
{
	return rintf(x);
}

long double 
_C_DECL 
nearbyintl(long double x)
{
	return rintl(x);
}

double 
_C_DECL 
rint(double x)
{
	return __CRT_rint(x);
}

float 
_C_DECL 
rintf(float x)
{
	return __CRT_rintf(x);
}

long double 
_C_DECL 
rintl(long double x)
{
	return __CRT_rintl(x);
}

long int 
_C_DECL 
lrint(double x)
{
	long int result = (long int)x;
	x -= result;
	if (x > 0.5)
		++result;
	return result;
}

long int 
_C_DECL 
lrintf(float x)
{
	long int result = (long int)x;
	x -= result;
	if (x > 0.5)
		++result;
	return result;
}

long int 
_C_DECL 
lrintl(long double x)
{
	long int result = (long int)x;
	x -= result;
	if (x > 0.5)
		++result;
	return result;
}

long long int 
_C_DECL 
llrint(double x)
{
	long long int result = (long long int)x;
	x -= result;
	if (x > 0.5)
		++result;
	return result;
}

long long int 
_C_DECL 
llrintf(float x)
{
	long long int result = (long long int)x;
	x -= result;
	if (x > 0.5)
		++result;
	return result;
}

long long int 
_C_DECL 
llrintl(long double x)
{
	long long int result = (long long int)x;
	x -= result;
	if (x > 0.5)
		++result;
	return result;
}

double 
_C_DECL 
round(double x)
{
	return __CRT_round(x);
}

float 
_C_DECL 
roundf(float x)
{
	return __CRT_roundf(x);
}

long double 
_C_DECL 
roundl(long double x)
{
	return __CRT_roundl(x);
}

long int 
_C_DECL 
lround(double x)
{
	return lrint(x);
}

long int 
_C_DECL 
lroundf(float x)
{
	return lrintf(x);
}

long int 
_C_DECL 
lroundl(long double x)
{
	return lrintl(x);
}

long long int 
_C_DECL 
llround(double x)
{
	return llrint(x);
}

long long int 
_C_DECL 
llroundf(float x)
{
	return llrintf(x);
}

long long int 
_C_DECL 
llroundl(long double x)
{
	return llrintl(x);
}

double 
_C_DECL 
trunc(double x)
{
	return __CRT_trunc(x);
}

float 
_C_DECL 
truncf(float x)
{
	return __CRT_truncf(x);
}

long double 
_C_DECL 
truncl(long double x)
{
	return __CRT_truncl(x);
}

double 
_C_DECL 
fmod(double x, double y)
{
	return __CRT_ieee754_fmod(x, y);
}

float 
_C_DECL 
fmodf(float x, float y)
{
	return __CRT_ieee754_fmodf(x, y);
}

long double 
_C_DECL 
fmodl(long double x, long double y)
{
	return __CRT_ieee754_fmodl(x, y);
}

double 
_C_DECL 
remainder(double x, double y)
{
	return __CRT_ieee754_remainder(x, y);
}

float 
_C_DECL 
remainderf(float x, float y)
{
	return __CRT_ieee754_remainderf(x, y);
}

long double 
_C_DECL 
remainderl(long double x, long double y)
{
	return __CRT_ieee754_remainderl(x, y);
}

double 
_C_DECL 
remquo(double x, double y, int* quo)
{
	return __CRT_remquo(x, y, quo);
}

float 
_C_DECL 
remquof(float x, float y, int* quo)
{
	return __CRT_remquof(x, y, quo);
}

long double 
_C_DECL 
remquol(long double x, long double y, int* quo)
{
	return __CRT_remquol(x, y, quo);
}

double 
_C_DECL 
copysign(double x, double y)
{
	return __CRT_copysign(x, y);
}

float 
_C_DECL 
copysignf(float x, float y)
{
	return __CRT_copysignf(x, y);
}

long double 
_C_DECL 
copysignl(long double x, long double y)
{
	return __CRT_copysignl(x, y);
}

static char __CRT_nan_buffer[100];

double 
_C_DECL 
nan(const char* tagp)
{
	double quetNAN = 0.0;

	size_t len = strlen(tagp);

	if (len)
	{
		sprintf(__CRT_nan_buffer, "NAN(%s)", tagp);
		quetNAN = strtod(__CRT_nan_buffer, NULL);
	}
	else
	{
		quetNAN = strtod("NAN", NULL);
	}

	return quetNAN;
}

float 
_C_DECL 
nanf(const char* tagp)
{
	return (float)nan(tagp);
}

long double 
_C_DECL 
nanl(const char* tagp)
{
	return nan(tagp);
}

double 
_C_DECL 
nextafter(double x, double y)
{
	return __CRT_nextafter(x, y);
}

float 
_C_DECL 
nextafterf(float x, float y)
{
	return __CRT_nextafterf(x, y);
}

long double 
_C_DECL 
nextafterl(long double x, long double y)
{
	return __CRT_nextafterl(x, y);
}

double 
_C_DECL 
nexttoward(double x, long double y)
{
	return __CRT_nextafter(x, y);
}

float 
_C_DECL 
nexttowardf(float x, long double y)
{
	return __CRT_nextafterf(x, (float)y);
}

long double 
_C_DECL 
nexttowardl(long double x, long double y)
{
	return __CRT_nextafterl(x, y);
}

double 
_C_DECL 
fdim(double x, double y)
{
	if (__CRT_inline_isnan(x))
		return (x);
	if (__CRT_inline_isnan(y))
		return (y);
	return (x > y ? x - y : 0.0);
}

float 
_C_DECL 
fdimf(float x, float y)
{
	if (__CRT_inline_isnanf(x))
		return (x);
	if (__CRT_inline_isnanf(y))
		return (y);
	return (x > y ? x - y : 0.0f);
}

long double 
_C_DECL 
fdiml(long double x, long double y)
{
	if (__CRT_inline_isnanl(x))
		return (x);
	if (__CRT_inline_isnanl(y))
		return (y);
	return (x > y ? x - y : 0.0);
}

double 
_C_DECL 
fmax(double x, double y)
{
	return __CRT_fmax(x, y);
}

float 
_C_DECL 
fmaxf(float x, float y)
{
	return __CRT_fmaxf(x, y);
}

long double 
_C_DECL 
fmaxl(long double x, long double y)
{
	return __CRT_fmaxl(x, y);
}

double 
_C_DECL 
fmin(double x, double y)
{
	return __CRT_fmin(x, y);
}

float 
_C_DECL 
fminf(float x, float y)
{
	return __CRT_fminf(x, y);
}

long double 
_C_DECL 
fminl(long double x, long double y)
{
	return __CRT_fminl(x, y);
}

double 
_C_DECL 
fma(double x, double y, double z)
{
	return __CRT_fma(x, y, z);
}

float 
_C_DECL 
fmaf(float x, float y, float z)
{
	return __CRT_fmaf(x, y, z);
}

long double 
_C_DECL 
fmal(long double x, long double y, long double z)
{
	return __CRT_fmal(x, y, z);
}
