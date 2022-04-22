/*- `strtol`, `strtoll`
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. [rescinded 22 July 1999]
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "CRT.h"
#include "stdlib.h"

#include "locale.h"

#include "assert.h"
#include "errno.h"
#include "ctype.h"
#include "string.h"
#include "math.h"

extern int errno;

#ifdef __CRT_WIN32
#include "WinAPI.h"
#endif

extern __CRT_main_struct __crt;

void _C_DECL __CRT_on_atexitProcs();

size_t
_C_DECL
__CRT_mb_cur_max()
{
	/* `never greater than MB_LEN_MAX` */
	return 1;
}

#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))		/* 0xFFFFFFFF */
#endif

#ifndef LONG_MAX
#define	LONG_MAX	((long)(ULONG_MAX >> 1))	/* 0x7FFFFFFF */
#endif

#ifndef LONG_MIN
#define	LONG_MIN	((long)(~LONG_MAX))		/* 0x80000000 */
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX (~(unsigned long long)0) /* 0xFFFFFFFFFFFFFFFF */
#endif

#ifndef LLONG_MAX
#define LLONG_MAX ((long long)(ULLONG_MAX >> 1)) /* 0x7FFFFFFFFFFFFFFF */
#endif

#ifndef LLONG_MIN
#define LLONG_MIN (~LLONG_MAX) /* 0x8000000000000000 */
#endif

const double __stdlib_string_to_double_table[17] =
{
	0.0,
	0.1,
	0.01,
	0.001,
	0.0001,
	0.00001,
	0.000001,
	0.0000001,
	0.00000001,
	0.000000001,
	0.0000000001,
	0.00000000001,
	0.000000000001,
	0.0000000000001,
	0.00000000000001,
	0.000000000000001,
	0.0000000000000001,
};

const long long __stdlib_string_to_double_table_hex[17] =
{
	1,
	16,
	256,
	4096,
	65536,
	1048576,
	16777216,
	268435456,
	4294967296,
	68719476736,
	1099511627776,
	17592186044416,
	281474976710656,
	4503599627370496,
	72057594037927936,
	1152921504606846976,
	-1,
};

const int __stdlib_string_to_int_table_hex[6] =
{
	10, 11, 12, 13, 14, 15
};
const char __stdlib_int_to_hex_table[16] =
{
	'0', '1', '2', '3','4','5','6','7','8','9','a','b','c','d','e','f',
};

const char*
__string_skip_spaces(const char* str)
{
	while (1)
	{
		if (*str > 0 && *str < 33)
			++str;
		else
			break;

		if (!*str)
			break;
	}
	return str;
}

double 
_C_DECL 
strtod(const char* nptr, char** endptr)
{
	assert(nptr);
	double result = 0.0;
	long leftPart = 0;
	long rightPart = 0;
	int e_right = 0;
	int flags = 0; /* 1(-) 2(hex) 3(e) 4(e-)*/

	const char* str_ptr = __string_skip_spaces(nptr);
	const struct lconv* lc = localeconv();
	char decimal_point = *lc->decimal_point;

	/*with - or not*/
	if (*str_ptr == '-')
	{
		++str_ptr;
		flags |= 0x1;
	}

	if (!*str_ptr)
		goto end;

	/*hex or not, skip 00*/
	if (*str_ptr == '0')
	{
		++str_ptr;
		if (!*str_ptr)
			goto end;

		if (*str_ptr == 'x' || *str_ptr == 'X')
		{
			flags |= 0x2;
			++str_ptr;
		}
		else if (*str_ptr == decimal_point)
			--str_ptr;
		else if (*str_ptr < '1' || *str_ptr > '9')
			goto end;
	}

	if (!*str_ptr)
		goto end;

	long part_2_count = 0;

	if (flags & 0x2)/*hex*/
	{
		int charNum = 0;
		const unsigned char* save_str_ptr = str_ptr;
		while (*str_ptr)
		{
			if ((*str_ptr >= 'a' && *str_ptr <= 'f')
				|| (*str_ptr >= 'A' && *str_ptr <= 'F'))
				++charNum;
			else
				break;

			++str_ptr;

			if (!*str_ptr)
				break;
		}

		if (charNum)
		{
			str_ptr = save_str_ptr;
			int charNum2 = charNum;
			for (int i = 0; i < charNum; ++i)
			{
				int curr_char = str_ptr[i];
				if (curr_char < 'a')
					curr_char += 32;

				int curr_int = __stdlib_string_to_int_table_hex[curr_char - 97];

				leftPart += curr_int * (long)__stdlib_string_to_double_table_hex[charNum2 - 1];
				--charNum2;
			}
		}
	}
	else
	{
		/* left part 1234. */
		while (*str_ptr >= '0' && *str_ptr <= '9')
		{
			leftPart *= 10;
			leftPart += *str_ptr - '0';
			++str_ptr;

			if (!*str_ptr)
				break;
		}

		if (*str_ptr == 'e' || *str_ptr == 'E')
		{
			flags |= 0x4;
			++str_ptr;

			goto do_e;
		}

		/* right part .5678 */
		if (*str_ptr == decimal_point)
		{
			++str_ptr;

			if (!*str_ptr)
				goto finish;

			while (*str_ptr >= '0' && *str_ptr <= '9')
			{
				rightPart *= 10;
				rightPart += *str_ptr - '0';
				++str_ptr;
				++part_2_count;
			}

			if (*str_ptr == 'e' || *str_ptr == 'E')
			{
				flags |= 0x4;
				++str_ptr;

				goto do_e;
			}
		}
	}

finish:;
	result = leftPart + ((float)rightPart * __stdlib_string_to_double_table[part_2_count]);

	if (flags & 0x4)
	{
		double em = 1.0;

		for (int i = 0; i < e_right; ++i)
		{
			if (flags & 0x8) /* - */
				em *= 0.1;
			else
				em *= 10.0;
		}

		result = result * em;
	}

	result = (flags & 0x1) ? -result : result;

end:;
	if (endptr)
		*endptr = (char*)str_ptr;

	return result;

do_e:;

	if (!*str_ptr)
		goto finish;

	if (*str_ptr == '-')
	{
		flags |= 0x8;

		++str_ptr;
		if (!*str_ptr)
			goto finish;
	}
	else if (*str_ptr == '+')
	{
		++str_ptr;
		if (!*str_ptr)
			goto finish;
	}

	while (*str_ptr >= '0' && *str_ptr <= '9')
	{
		e_right *= 10;
		e_right += *str_ptr - '0';
		++str_ptr;
	}
	goto finish;
}

float 
_C_DECL 
strtof(const char* nptr, char** endptr)
{
	assert(nptr);

	const struct lconv* lc = localeconv();
	char decimal_point = *lc->decimal_point;

	float result = 0.0;
	long leftPart = 0;
	long rightPart = 0;
	int e_right = 0;
	int flags = 0; /* 1(-) 2(hex) 3(e) 4(e-)*/
	const char* str_ptr = __string_skip_spaces(nptr);
	if (*str_ptr == '-'){
		++str_ptr;
		flags |= 0x1;
	}
	if (!*str_ptr) goto end;
	if (*str_ptr == '0'){
		++str_ptr;
		if (!*str_ptr) goto end;
		if (*str_ptr == 'x' || *str_ptr == 'X'){
			flags |= 0x2;
			++str_ptr;
		}else if (*str_ptr == decimal_point) --str_ptr;
		else if (*str_ptr < '1' || *str_ptr > '9') goto end;
	}
	if (!*str_ptr) goto end;
	long part_2_count = 0;
	if (flags & 0x2){
		int charNum = 0;
		const unsigned char* save_str_ptr = str_ptr;
		while (*str_ptr){
			if ((*str_ptr >= 'a' && *str_ptr <= 'f')|| (*str_ptr >= 'A' && *str_ptr <= 'F'))
				++charNum;
			else break;
			++str_ptr;
			if (!*str_ptr) break;
		}
		if (charNum){
			str_ptr = save_str_ptr;
			int charNum2 = charNum;
			for (int i = 0; i < charNum; ++i){
				int curr_char = str_ptr[i];
				if (curr_char < 'a') curr_char += 32;
				int curr_int = __stdlib_string_to_int_table_hex[curr_char - 97];
				leftPart += curr_int * (long)__stdlib_string_to_double_table_hex[charNum2 - 1];
				--charNum2;
			}
		}
	}else {
		while (*str_ptr >= '0' && *str_ptr <= '9'){
			leftPart *= 10;
			leftPart += *str_ptr - '0';
			++str_ptr;
			if (!*str_ptr) break;
		}
		if (*str_ptr == 'e' || *str_ptr == 'E'){
			flags |= 0x4;
			++str_ptr;
			goto do_e;
		}
		if (*str_ptr == decimal_point){
			++str_ptr;
			if (!*str_ptr) goto finish;
			while (*str_ptr >= '0' && *str_ptr <= '9'){
				rightPart *= 10;
				rightPart += *str_ptr - '0';
				++str_ptr;
				++part_2_count;
			}
			if (*str_ptr == 'e' || *str_ptr == 'E'){
				flags |= 0x4;
				++str_ptr;
				goto do_e;
			}
		}
	}
finish:;
	result = leftPart + ((float)rightPart * (float)__stdlib_string_to_double_table[part_2_count]);
	if (flags & 0x4){
		float em = 1.f;
		for (int i = 0; i < e_right; ++i){
			if (flags & 0x8) em *= 0.1f;
			else em *= 10.0f;
		}
		result = result * em;
	}
	result = (flags & 0x1) ? -result : result;
end:;
	if (endptr) *endptr = (char*)str_ptr;
	return result;
do_e:;
	if (!*str_ptr) goto finish;
	if (*str_ptr == '-') {
		flags |= 0x8;
		++str_ptr;
		if (!*str_ptr) goto finish;
	} else if (*str_ptr == '+'){
		++str_ptr;
		if (!*str_ptr) goto finish;
	}
	while (*str_ptr >= '0' && *str_ptr <= '9'){
		e_right *= 10;
		e_right += *str_ptr - '0';
		++str_ptr;
	}
	goto finish;
}

long double 
_C_DECL 
strtold(const char* nptr, char** endptr)
{
	assert(nptr);
	return strtod(nptr, endptr);
}

double 
_C_DECL 
atof(const char* nptr)
{
	assert(nptr);
	return strtod(nptr, NULL);
}

long int
_C_DECL 
strtol(const char* nptr, char** endptr, int base)
{
	assert(nptr);
	register const char* s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	}
	else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
		c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
		errno = ERANGE;
	}
	else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char*)(any ? s - 1 : nptr);
	return (acc);
	return 0;
}

long long int
_C_DECL 
strtoll(const char* nptr, char** endptr, int base)
{
	assert(nptr);
	register const char* s = nptr;
	register long long int acc;
	register int c;
	register long long int cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	}
	else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
		c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(long long int)LLONG_MIN : LLONG_MAX;
	cutlim = cutoff % (long long int)base;
	cutoff /= (long long int)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LLONG_MIN : LLONG_MAX;
		errno = ERANGE;
	}
	else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char*)(any ? s - 1 : nptr);
	return (acc);
}

unsigned long int 
_C_DECL 
strtoul(const char* nptr, char** endptr, int base)
{
	assert(nptr);
	return (unsigned long int)strtoll(nptr, endptr, base);
}

unsigned long long int 
_C_DECL 
strtoull(const char* nptr, char** endptr, int base)
{
	assert(nptr);
	return (unsigned long long int)strtoll(nptr, endptr, base);
}

int 
_C_DECL 
atoi(const char* nptr)
{
	assert(nptr);
	int result = 0;
	const char* str_ptr = __string_skip_spaces(nptr);
	int isNeg = 0;
	if (*str_ptr == '-')
	{
		isNeg = 1;
		++str_ptr;
	}

	if (!*str_ptr)
		goto end;

	while (*str_ptr >= '0' && *str_ptr <= '9')
	{
		result *= 10;
		result += *str_ptr - '0';
		++str_ptr;

		if (!*str_ptr)
			break;
	}

end:;
	return isNeg ? -result : result;
}

long int 
_C_DECL 
atol(const char* nptr)
{
	return atoi(nptr);
}

long long int 
_C_DECL 
atoll(const char* nptr)
{
	assert(nptr);
	long long int result = 0;
	const char* str_ptr = __string_skip_spaces(nptr);
	int isNeg = 0;
	if (*str_ptr == '-')
	{
		isNeg = 1;
		++str_ptr;
	}

	if (!*str_ptr)
		goto end;

	while (*str_ptr >= '0' && *str_ptr <= '9')
	{
		result *= 10;
		result += *str_ptr - '0';
		++str_ptr;

		if (!*str_ptr)
			break;
	}

end:;
	return isNeg ? -result : result;
}

int 
_C_DECL 
rand(void)
{
	__crt._->rand_seed = __crt._->rand_seed * 1103515245 + 12345;
	return (unsigned int)(__crt._->rand_seed / 65536) % 32768;
}

void 
_C_DECL 
srand(unsigned int seed)
{
	__crt._->rand_seed = seed;
}

void* 
_C_DECL 
calloc(size_t nmemb, size_t size)
{
	void* mem = NULL;
#ifdef __CRT_WIN32
	mem = HeapAlloc(__crt.processHeap, HEAP_ZERO_MEMORY, size * nmemb);
#endif
	return mem;
}

void 
_C_DECL 
free(void* ptr)
{
#ifdef __CRT_WIN32
	HeapFree(__crt.processHeap, 0, ptr);
#endif
}

void* 
_C_DECL 
malloc(size_t size)
{
	void* mem = NULL;
#ifdef __CRT_WIN32
	mem = HeapAlloc(__crt.processHeap, 0, size);
#endif
	return mem;
}

void* 
_C_DECL 
realloc(void* ptr, size_t size)
{
	void* mem = NULL;
#ifdef __CRT_WIN32
	mem = HeapReAlloc(__crt.processHeap, HEAP_ZERO_MEMORY, ptr, size);
#endif
	if (!mem)
		return ptr;

	return mem;
}

#ifdef __CRT_WITH_ABORT
void 
_C_DECL 
abort(void)
{
	exit(EXIT_FAILURE);
}
#endif

void
__grow_atexitProcs()
{
	const int addSize = 100;
	_PVFV* newProcs = malloc(__crt._->atexitProcsAllocated + addSize * sizeof(_PVFV));

	if (__crt._->atexitProcsAllocated && __crt._->atexitProcs)
	{
		memcpy(newProcs, __crt._->atexitProcs, __crt._->atexitProcsAllocated);
		free(__crt._->atexitProcs);
	}
	__crt._->atexitProcs = newProcs;
	__crt._->atexitProcsAllocated = __crt._->atexitProcsAllocated + addSize;
}

void
__add_atexitProcs(_PVFV fnc)
{
	unsigned int newSize = __crt._->atexitProcsSize + 1;
	if (newSize > __crt._->atexitProcsAllocated)
		__grow_atexitProcs();

	__crt._->atexitProcs[__crt._->atexitProcsSize] = fnc;
	__crt._->atexitProcsSize = newSize;
}

// 
void
_C_DECL
__CRT_freeAll()
{
	free(__crt._);
}


int
_cdecl
atexit(void(*func)(void))
{
	if (func)
		__add_atexitProcs(func);
	// The atexit function returns zero if the registration succeeds, nonzero if it fails.
	return 0;
}

void
_C_DECL 
_Exit(int status)
{
	/*The _Exit function causes normal program termination to occur and control to be
returned to the host environment. No functions registered by the atexit function or
signal handlers registered by the signal function are called. The status returned to the
host environment is determined in the same way as for the exit function (7.20.4.3).
Whether open streams with unwritten buffered data are flushed, open streams are closed,
or temporary files are removed is implementation-defined.*/
	__CRT_freeAll();

#ifdef __CRT_WIN32
	TerminateProcess(GetCurrentProcess(), status);
#endif
}


void 
_C_DECL 
exit(int status)
{
	/*First, all functions registered by the atexit function are called, in the reverse order of
their registration,262) except that a function is called after any previously registered
functions that had already been called at the time it was registered. If, during the call to
any such function, a call to the longjmp function is made that would terminate the call
to the registered function, the behavior is undefined*/
	__CRT_on_atexitProcs();

	/*Next, all open streams with unwritten buffered data are flushed, all open streams are
closed, and all files created by the tmpfile function are removed.*/
	__CRT_freeAll();

#ifdef __CRT_WIN32
	TerminateProcess(GetCurrentProcess(), status);
#endif
}

uint32_t
_C_DECL
__CRT_dectooct(uint32_t dec)
{
	uint32_t oct = 0;
	if (dec)
	{
		uint32_t m = 0;
		uint32_t m2 = 0;
		while (1)
		{
			uint32_t rem = dec % 8;
			dec /= 8;

			m2 = m * 10;
			if (!m2)
				oct += rem;
			else
				oct += rem * m2;
			if (!m)
				m = 1;
			else
				m *= 10;

			if (!dec)
				break;
		}
	}
	return oct;
}

uint64_t
_C_DECL
__CRT_dectooctll(uint64_t dec)
{
	uint64_t oct = 0;
	if (dec)
	{
		uint64_t m = 0;
		uint64_t m2 = 0;
		while (1)
		{
			uint64_t rem = dec % 8;
			dec /= 8;

			m2 = m * 10;
			if (!m2)
				oct += rem;
			else
				oct += rem * m2;
			if (!m)
				m = 1;
			else
				m *= 10;

			if (!dec)
				break;
		}
	}
	return oct;
}

// lower case
// size of `buf` must be enough for 'ffffffffffffffff' so buf[16]
char __CRT_itoa_internalBuffer[30]; // need to fix itoa(123) = "321"
void
_C_DECL
__CRT_dectohex(uint64_t dec, char* buf, int* sz)
{
	if (dec)
	{
		double decInDouble = (double)dec;
		int bufInd = 0;
		while (1)
		{
			decInDouble /= 16.0;

			double ipart = 0.0;
			int i = (int)(modf(decInDouble, &ipart) * 16.0);//(dec % 16) * 16;
			dec /= 16; // for exit from loop

			__CRT_itoa_internalBuffer[bufInd++] = __stdlib_int_to_hex_table[i];

			if (!dec)
				break;
		}
		*sz = bufInd;
		if (bufInd)
		{
			for (int i = 0; i < bufInd; ++i)
			{
				buf[i] = __CRT_itoa_internalBuffer[bufInd - i - 1];
			}
			buf[bufInd] = 0;
		}
	}
	else
	{
		buf[0] = '0';
		buf[1] = 0;
	}
}

// mode: 
//  0 - normal,  decimal
//  1 - hex 7fa
//  2 - hex 7FA
//  3 - octal 610
char __CRT_itoa_buffer[31]; // for vsnprintf
void 
_C_DECL 
__CRT_itoa(int val, char* buf, size_t bufSz, int mode)
{
	__CRT_lltoa(val, buf, bufSz, mode);
}

void 
_C_DECL 
__CRT_uitoa(uint32_t val, /*not const*/ char* buf, size_t bufSz, int mode)
{
	__CRT_ulltoa(val, buf, bufSz, mode);
}

void 
_C_DECL 
__CRT_lltoa(long long val, /*not const*/ char* buf, size_t bufSz, int mode)
{
	if (val)
	{
		long long val2 = val;

		switch (mode)
		{
		case 0:
		case 3:
		default:
		{
			if (mode == 3)
				val2 = __CRT_dectooctll(val);
			int bufferIndex = 0;
			while (1)
			{
				long long mod = val2 % 10;
				if (mod < 0) mod *= -1;
				__CRT_itoa_internalBuffer[bufferIndex++] = (char)mod;
				val2 = val2 / 10;

				if (!val2)
					break;
			}
			if (bufferIndex)
			{
				int dstBufInd = 0;
				if (val < 0)
					buf[dstBufInd++] = '-';

				for (int i = 0; i < bufferIndex; ++i)
				{
					buf[dstBufInd++] = __CRT_itoa_internalBuffer[bufferIndex - i - 1] + '0';
					if (dstBufInd == bufSz)
						break;
				}
				buf[dstBufInd] = 0;
			}
		}break;
		case 1:
		case 2:
		{
			int bsz = 0;
			char hexbuf[10];
			__CRT_dectohex(val, hexbuf, &bsz);
			if (bsz)
			{
				for (int i = 0; i < bsz; ++i)
				{
					buf[i] = hexbuf[i];
					buf[i + 1] = 0;
				}
			}

			if (mode == 2)
			{
				for (size_t i2 = 0; i2 < bufSz; ++i2)
				{
					if (islower(buf[i2]))
						buf[i2] = toupper(buf[i2]);

					if (!buf[i2])
						break;
				}
			}
		}break;
		}
	}
	else
	{
		buf[0] = '0';
		buf[1] = 0;
	}
}

void 
_C_DECL 
__CRT_ulltoa(uint64_t val, /*not const*/ char* buf, size_t bufSz, int mode)
{
	if (val)
	{
		uint64_t val2 = val;

		switch (mode)
		{
		case 0:
		case 3:
		default:
		{
			if (mode == 3)
				val2 = __CRT_dectooctll(val);
			int bufferIndex = 0;
			while (1)
			{
				unsigned long long mod = val2 % 10;
				__CRT_itoa_internalBuffer[bufferIndex++] = (char)mod;

				val2 = val2 / 10;

				if (!val2)
					break;
			}
			if (bufferIndex)
			{
				int dstBufInd = 0;
				for (int i = 0; i < bufferIndex; ++i)
				{
					buf[dstBufInd++] = __CRT_itoa_internalBuffer[bufferIndex - i - 1] + '0';
					if (dstBufInd == bufSz)
						break;
				}
				buf[dstBufInd] = 0;
			}
		}
		break;
		case 1:
		case 2:
		{
			int bsz = 0;
			char hexbuf[10];
			__CRT_dectohex(val, hexbuf, &bsz);
			if (bsz)
			{
				for (int i = 0; i < bsz; ++i)
				{
					buf[i] = hexbuf[i];
					buf[i + 1] = 0;
				}
			}
			if (mode == 2)
			{
				for (size_t i2 = 0; i2 < bufSz; ++i2)
				{
					if (islower(buf[i2]))
						buf[i2] = toupper(buf[i2]);

					if (!buf[i2])
						break;
				}
			}
		}break;
		}
	}
	else
	{
		buf[0] = '0';
		buf[1] = 0;
	}
}