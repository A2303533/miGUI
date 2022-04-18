#pragma once
#ifndef __STRING__
#define __STRING__

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

	/*7.21.2.1*/
	void* _C_DECL memcpy(void* s1, const void* s2, size_t n);

	/*7.21.4.2*/
	int _C_DECL strcmp(const char* s1, const char* s2);

	/*7.21.6.1*/
	void* _C_DECL memset(void* s, int c, size_t n);


#ifdef __cplusplus
}
#endif

#endif