#include "CRT.h"
#include "assert.h"
#include "string.h"

void* 
_C_DECL 
memcpy(void* s1, const void* s2, size_t n)
{
	assert(s1);
	assert(s2);
	
	unsigned char* _s1 = (unsigned char*)s1;
	unsigned char* _s2 = (unsigned char*)s2;

	for (size_t i = 0; i < n; ++i)
	{
		_s1[i] = _s2[i];
	}

	return s1;
}

int 
_C_DECL 
strcmp(const char* s1, const char* s2)
{
	int result = 1;
	while (1)
	{
		if (*s1 && *s2)
		{
			if (*s1 < *s2)
				return -1;

			if (*s1 > *s2)
				return 1;
		}
		++s1;
		++s2;

		result = 0;

		if (!*s1 && !*s2)
			break;
		else if (!*s1 && *s2)
			return -1;
		else if (*s1 && !*s2)
			return 1;
	}

	return result;
}

void* 
_C_DECL 
memset(void* s, int c, size_t n)
{
	assert(s);

	unsigned char converted_c = (unsigned char)c;
	unsigned char* ptr = s;
	for (size_t i = 0; i < n; ++i)
	{
		ptr[i] = converted_c;
	}

	return s;
}


