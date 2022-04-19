#pragma once
#ifndef __STDBOOL__
#define __STDBOOL__

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 7.16
#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#endif


#ifdef __cplusplus
}
#endif

#endif