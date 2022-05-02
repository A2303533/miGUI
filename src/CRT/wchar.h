#pragma once
#ifndef __WCHAR__
#define __WCHAR__

#include "CRT.h"


/*ISO/IEC 9899:TC3, N1256*/

#ifdef __cplusplus
extern "C" {
#endif

#include "size_t.h"

#ifndef __cplusplus
	typedef signed short wchar_t;
#endif

	typedef struct {
		int i;
	} mbstate_t;

#ifdef __cplusplus
}
#endif


#endif