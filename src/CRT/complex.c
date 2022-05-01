#include "CRT.h"
#include "complex.h"


double_complex 
_C_DECL 
cacos(double_complex z)
{
	return __CRT_cacos(z);
}

float_complex 
_C_DECL 
cacosf(float_complex z)
{
	return __CRT_cacosf(z);
}

long_double_complex 
_C_DECL 
cacosl(long_double_complex z)
{
	return __CRT_cacosl(z);
}

double_complex 
_C_DECL 
casin(double_complex z)
{
	return __CRT_casin(z);
}

float_complex 
_C_DECL 
casinf(float_complex z)
{
	return __CRT_casinf(z);
}

long_double_complex 
_C_DECL 
casinl(long_double_complex z)
{
	return __CRT_casinl(z);
}

double_complex 
_C_DECL 
catan(double_complex z)
{
	return __CRT_catan(z);
}

float_complex 
_C_DECL 
catanf(float_complex z)
{
	return __CRT_catanf(z);
}

long_double_complex 
_C_DECL 
catanl(long_double_complex z)
{
	return __CRT_catanl(z);
}

double_complex 
_C_DECL 
ccos(double_complex z)
{
	return __CRT_ccos(z);
}

float_complex 
_C_DECL 
ccosf(float_complex z)
{
	return __CRT_ccosf(z);
}

long_double_complex 
_C_DECL 
ccosl(long_double_complex z)
{
	return __CRT_ccosl(z);
}

double_complex 
_C_DECL 
csin(double_complex z)
{
	return __CRT_csin(z);
}

float_complex 
_C_DECL 
csinf(float_complex z)
{
	return __CRT_csinf(z);
}

long_double_complex 
_C_DECL 
csinl(long_double_complex z)
{
	return __CRT_csinl(z);
}

double_complex 
_C_DECL 
ctan(double_complex z)
{
	return __CRT_ctan(z);
}

float_complex 
_C_DECL 
ctanf(float_complex z)
{
	return __CRT_ctanf(z);
}

long_double_complex 
_C_DECL 
ctanl(long_double_complex z)
{
	return __CRT_ctanl(z);
}

double_complex 
_C_DECL 
cacosh(double_complex z)
{
	return __CRT_cacosh(z);
}

float_complex 
_C_DECL 
cacoshf(float_complex z)
{
	return __CRT_cacoshf(z);
}

long_double_complex 
_C_DECL 
cacoshl(long_double_complex z)
{
	return __CRT_cacoshl(z);
}

double_complex 
_C_DECL 
casinh(double_complex z)
{
	return __CRT_casinh(z);
}

float_complex 
_C_DECL 
casinhf(float_complex z)
{
	return __CRT_casinhf(z);
}

long_double_complex 
_C_DECL 
casinhl(long_double_complex z)
{
	return __CRT_casinhl(z);
}

double_complex 
_C_DECL 
catanh(double_complex z)
{
	return __CRT_catanh(z);
}

float_complex 
_C_DECL 
catanhf(float_complex z)
{
	return __CRT_catanhf(z);
}

long_double_complex 
_C_DECL 
catanhl(long_double_complex z)
{
	return __CRT_catanhl(z);
}

double_complex 
_C_DECL 
ccosh(double_complex z)
{
	return __CRT_ccosh(z);
}

float_complex 
_C_DECL 
ccoshf(float_complex z)
{
	return __CRT_ccoshf(z);
}

long_double_complex 
_C_DECL 
ccoshl(long_double_complex z)
{
	return __CRT_ccoshl(z);
}

double_complex 
_C_DECL 
csinh(double_complex z)
{
	return __CRT_csinh(z);
}

float_complex 
_C_DECL 
csinhf(float_complex z)
{
	return __CRT_csinhf(z);
}

long_double_complex 
_C_DECL 
csinhl(long_double_complex z)
{
	return __CRT_csinhl(z);
}

double_complex 
_C_DECL 
ctanh(double_complex z)
{
	return __CRT_ctanh(z);
}

float_complex 
_C_DECL 
ctanhf(float_complex z)
{
	return __CRT_ctanhf(z);
}

long_double_complex 
_C_DECL 
ctanhl(long_double_complex z)
{
	return __CRT_ctanhl(z);
}

double_complex 
_C_DECL 
cexp(double_complex z)
{
	return __CRT_cexp(z);
}

float_complex 
_C_DECL 
cexpf(float_complex z)
{
	return __CRT_cexpf(z);
}

long_double_complex 
_C_DECL 
cexpl(long_double_complex z)
{
	return __CRT_cexpl(z);
}

double_complex 
_C_DECL 
clog(double_complex z)
{
	return __CRT_clog(z);
}

float_complex 
_C_DECL 
clogf(float_complex z)
{
	return __CRT_clogf(z);
}

long_double_complex 
_C_DECL 
clogl(long_double_complex z)
{
	return __CRT_clogl(z);
}

double 
_C_DECL 
cabs(double_complex z)
{
	return hypot(creal(z), cimag(z));
}

float 
_C_DECL 
cabsf(float_complex z)
{
	return hypotf(crealf(z), cimagf(z));
}

long double 
_C_DECL 
cabsl(long_double_complex z)
{
	return hypotl(creall(z), cimagl(z));
}

double_complex 
_C_DECL 
cpow(double_complex x, double_complex y)
{
	return __CRT_cpow(x, y);
}

float_complex 
_C_DECL 
cpowf(float_complex x, float_complex y)
{
	return __CRT_cpowf(x, y);
}

long_double_complex 
_C_DECL 
cpowl(long_double_complex x, long_double_complex y)
{
	return __CRT_cpowl(x, y);
}

double_complex 
_C_DECL 
csqrt(double_complex z)
{
	return __CRT_csqrt(z);
}

float_complex 
_C_DECL 
csqrtf(float_complex z)
{
	return __CRT_csqrtf(z);
}

long_double_complex 
_C_DECL 
csqrtl(long_double_complex z)
{
	return __CRT_csqrtl(z);
}

double 
_C_DECL 
carg(double_complex z)
{
	return (atan2(cimag(z), creal(z)));
}

float 
_C_DECL 
cargf(float_complex z)
{
	return (atan2f(cimagf(z), crealf(z)));
}

long double 
_C_DECL 
cargl(long_double_complex z)
{
	return (atan2l(cimagl(z), creall(z)));
}

double 
_C_DECL 
cimag(double_complex z)
{
	return z.a[1];
}

float 
_C_DECL 
cimagf(float_complex z)
{
	return z.a[1];
}

long double 
_C_DECL 
cimagl(long_double_complex z)
{
	return z.a[1];
}

double_complex 
_C_DECL 
conj(double_complex z)
{
	return (CMPLX(creal(z), -cimag(z)));
}

float_complex 
_C_DECL 
conjf(float_complex z)
{
	return (CMPLXF(crealf(z), -cimagf(z)));
}

long_double_complex 
_C_DECL 
conjl(long_double_complex z)
{
	return (CMPLXL(creall(z), -cimagl(z)));
}

#pragma warning(push)
#pragma warning(disable: 4244)
double_complex 
_C_DECL 
cproj(double_complex z)
{
	if (!isinf(creal(z)) && !isinf(cimag(z)))
		return (z);
	else
		return (CMPLX(INFINITY, copysign(0.0, cimag(z))));
}

float_complex 
_C_DECL 
cprojf(float_complex z)
{
	if (!isinf(crealf(z)) && !isinf(cimagf(z)))
		return (z);
	else
		return (CMPLXF(INFINITY, copysignf(0.0, cimagf(z))));
}

long_double_complex 
_C_DECL 
cprojl(long_double_complex z)
{
	if (!isinf(creall(z)) && !isinf(cimagl(z)))
		return (z);
	else
		return (CMPLXL(INFINITY, copysignl(0.0, cimagl(z))));
}
#pragma warning(pop)

double 
_C_DECL 
creal(double_complex z)
{
	return z.a[0];
}

float 
_C_DECL 
crealf(float_complex z)
{
	return z.a[0];
}

long double 
_C_DECL 
creall(long_double_complex z)
{
	return z.a[0];
}
