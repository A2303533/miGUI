#pragma once
#ifndef __STDLIB__
#define __STDLIB__

#include "CRT.h"

/*ISO/IEC 9899:TC3, N1256*/

/*7.20:2*/
#ifdef __CRT_WIN32
#ifndef _WIN64
typedef unsigned int size_t;
#endif
#else
typedef unsigned int size_t;
#endif /*__CRT_WIN32*/
#ifndef __cplusplus /*keyword for C++*/
typedef unsigned short wchar_t;
#endif

#ifdef __CRT_PURE
typedef struct {
	int quot;
	int rem;
} div_t;

typedef struct {
	long int quot;
	long int rem;
} ldiv_t;

typedef struct {
	long long quot;
	long long rem;
} lldiv_t;
#endif /*__CRT_PURE*/

/*all internal prototypes*/
#ifdef __cplusplus
extern "C"{
#endif

size_t _C_DECL __CRT_mb_cur_max();

#ifdef __cplusplus
}
#endif

/*7.20:3*/
#ifdef __cplusplus
#if (__cplusplus > 199711L) /*MSVS compiler option: /Zc:__cplusplus*/

#ifndef NULL
#define NULL nullptr
#endif

#else
#define NULL 0
#endif
#else
#define NULL ((void *)0)
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define RAND_MAX 0x7fff

#define MB_CUR_MAX __CRT_mb_cur_max()

#ifdef __cplusplus
extern "C" {
#endif

	/*7.20.1.1*/
	double _C_DECL atof(const char* nptr);
	
	/*7.20.1.2*/
	int _C_DECL atoi(const char* nptr);
	long int _C_DECL atol(const char* nptr);
	long long int _C_DECL atoll(const char* nptr);

	/*7.20.1.3*/
	double _C_DECL strtod(const char* nptr, char** endptr);
	float _C_DECL strtof(const char* nptr, char** endptr);
	long double _C_DECL strtold(const char* nptr, char** endptr);

	/*7.20.1.4*/
	long int _C_DECL strtol(const char* nptr, char** endptr, int base);
	long long int _C_DECL strtoll(const char* nptr, char** endptr, int base);
	unsigned long int _C_DECL strtoul(const char* nptr, char** endptr, int base);
	unsigned long long int _C_DECL strtoull(const char* nptr, char** endptr, int base);

	/*7.20.2.1*/
	int _C_DECL rand(void);

	/*7.20.2.2*/
	void _C_DECL srand(unsigned int seed);
	
#ifdef __CRT_PURE
	/*7.20.4.3*/
	void _C_DECL exit(int status);
#endif /*__CRT_PURE*/

	/*7.20.3.1*/
	void* _C_DECL calloc(size_t nmemb, size_t size);
	
	/*7.20.3.2*/
	void _C_DECL free(void* ptr);
	
	/*7.20.3.3*/
	void* _C_DECL malloc(size_t size);

	/*7.20.3.4*/
	void* _C_DECL realloc(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif