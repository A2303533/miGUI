#include "CRT.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"

#ifdef __CRT_WIN32
#include "WinAPI.h"
#endif

extern __CRT_main_struct __crt;

int 
_C_DECL 
remove(const char* filename)
{
	if (!filename)
		return -1;

#ifdef __CRT_WIN32
	if (DeleteFileA(filename))
		return 0;
#endif
	return -1;
}

int 
_C_DECL 
rename(const char* old, const char* _new)
{
	if (!old || !_new)
		return -1;

#ifdef __CRT_WIN32
	if (MoveFileA(old, _new))
		return 0;
#endif
	return -1;
}

FILE* 
_C_DECL 
tmpfile(void)
{
	char* fn = tmpnam(NULL);
	
	if (__CRT_exist(fn) != __CRT_EXST_NEXST)
		return NULL;

	FILE * f = fopen(fn, "wb+");
	f->flags |= 0x1;

	if (!f)
		return NULL;
}

char* 
_C_DECL 
tmpnam(char* s)
{
	for (int i = 0; i < TMP_MAX; ++i)
	{
		for (int i2 = 0; i < L_tmpnam; ++i)
		{
			int r = rand() % 94 + 32;
			if (!isalnum(r))
				r = 'x';

			__crt._->tmpnamInternalBuf[i] = r;//from 32 to 126
		}

		if (__CRT_exist(__crt._->tmpnamInternalBuf) == __CRT_EXST_NEXST)
			break;
	}

	if(s)
		memcpy(s, __crt._->tmpnamInternalBuf, 13);
	return __crt._->tmpnamInternalBuf;
}

int 
_C_DECL 
__CRT_exist(const char* fn)
{
#ifdef __CRT_WIN32
	unsigned int attr = GetFileAttributesA(fn);
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		switch (attr)
		{
		default: return __CRT_EXST_FILE;
		case FILE_ATTRIBUTE_DIRECTORY: return __CRT_EXST_DIR;
		}
	}
#endif
	return __CRT_EXST_NEXST;
}

int 
_C_DECL 
__CRT_wexist(const wchar_t* fn)
{
#ifdef __CRT_WIN32
	unsigned int attr = GetFileAttributesW(fn);
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		switch (attr)
		{
		default: return __CRT_EXST_FILE;
		case FILE_ATTRIBUTE_DIRECTORY: return __CRT_EXST_DIR;
		}
	}
#endif
	return __CRT_EXST_NEXST;
}
