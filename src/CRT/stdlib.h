#pragma once
#ifndef __STDLIB__
#define __STDLIB__

#include "CRT.h"
#include "stdint.h"

/*ISO/IEC 9899:TC3, N1256*/

/*7.20:2*/
#include "size_t.h"

#ifndef __cplusplus /*keyword for C++*/
#ifndef __CRT_WCHAR_T_DEFINED
#define __CRT_WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#endif
#endif

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

#ifdef __cplusplus
extern "C"{
#endif

size_t _C_DECL __CRT_mb_cur_max();

#ifdef __cplusplus
}
#endif

/*7.20:3*/
#include "NULL.h"


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
	
	/*7.20.3.1*/
	void* _C_DECL calloc(size_t nmemb, size_t size);
	
	/*7.20.3.2*/
	void _C_DECL free(void* ptr);
	
	/*7.20.3.3*/
	void* _C_DECL malloc(size_t size);

	/*7.20.3.4*/
	void* _C_DECL realloc(void* ptr, size_t size);


#ifdef __CRT_WITH_ABORT
	/*7.20.4.1*/
	void _C_DECL abort(void);
#endif //__CRT_WITH_ABORT


	// 7.20.4.2
	int _C_DECL atexit(void(*func)(void));

	/*7.20.4.3*/
	void _C_DECL exit(int status);
	
	void _C_DECL _Exit(int status);

	// mode: 
	//  0 - normal,  decimal
	//  1 - hex 7fa
	//  2 - hex 7FA
	//  3 - octal 610
	void _C_DECL __CRT_itoa(int val, /*not const*/ char* buf, size_t bufSz, int mode);
	void _C_DECL __CRT_uitoa(uint32_t val, /*not const*/ char* buf, size_t bufSz, int mode);
	void _C_DECL __CRT_lltoa(long long val, /*not const*/ char* buf, size_t bufSz, int mode);
	void _C_DECL __CRT_ulltoa(uint64_t val, /*not const*/ char* buf, size_t bufSz, int mode);

#ifdef __cplusplus
}
#endif

#endif