#pragma once
#ifndef __STRING__
#define __STRING__

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CRT_WITH_MEMCPY
	/*7.21.2.1*/
	void* _C_DECL memcpy(void* dst, const void* src, size_t n);
#endif //__CRT_WITH_MEMCPY

#ifdef __CRT_WITH_STRCMP
	/*7.21.4.2*/
	int _C_DECL strcmp(const char* s1, const char* s2);
#endif //__CRT_WITH_STRCMP

#ifdef __CRT_WITH_MEMSET
	/*7.21.6.1*/
	void* _C_DECL memset(void* s, int c, size_t n);
#endif //__CRT_WITH_MEMSET

	/*7.21.6.3*/
	size_t _C_DECL strlen(const char* s);


#ifdef __cplusplus
}
#endif

#endif