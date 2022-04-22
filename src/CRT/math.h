#pragma once
#ifndef __MATH__
#define __MATH__

#include "CRT.h"


/*ISO/IEC 9899:TC3, N1256*/

#ifdef __cplusplus
extern "C" {
#endif

// 7.12.6.12
	double _C_DECL modf(double value, double* iptr);
	float _C_DECL modff(float value, float* iptr);
	long double _C_DECL modfl(long double value, long double* iptr);


#ifdef __cplusplus
}
#endif

#endif