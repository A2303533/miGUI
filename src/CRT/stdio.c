#include "CRT.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "errno.h"

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
_CRT_openFile(const char* fn, unsigned int fl)
{
	FILE* f = NULL;
#ifdef __CRT_WIN32
	unsigned int dwDesiredAccess = 0;
	unsigned int dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

	if (fl & __CRT_FOPEN_TMP)
		dwFlagsAndAttributes = FILE_ATTRIBUTE_TEMPORARY;

	if (fl & __CRT_FOPEN_UPDATE)
		dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	else if (fl & __CRT_FOPEN_READ)
		dwDesiredAccess = GENERIC_READ;
	else if ((fl & __CRT_FOPEN_WRITE) || (fl & __CRT_FOPEN_APPEND))
		dwDesiredAccess = GENERIC_WRITE;

	unsigned int dwCreationDisposition = 0;

	if (fl & __CRT_FOPEN_READ)
		dwCreationDisposition = OPEN_EXISTING;
	else if (fl & __CRT_FOPEN_WRITE)
		dwCreationDisposition = CREATE_ALWAYS;
	else if (fl & __CRT_FOPEN_APPEND)
		dwCreationDisposition = OPEN_ALWAYS;

	unsigned int dwShareMode = FILE_SHARE_READ;

	void* handle = CreateFileA(fn, dwDesiredAccess, dwShareMode, NULL, 
		dwCreationDisposition, dwFlagsAndAttributes, NULL);
	
	if(!handle)
		goto end;
#endif

	if (fl & __CRT_FOPEN_TMP)
		f = (FILE*)poolMalloc(&__crt._->tmpMaxPool);
	else
		f = (FILE*)poolMalloc(&__crt._->fopenMaxPool);

	if (!f)
	{
#ifdef __CRT_WIN32
		CloseHandle(handle);
#endif
		goto end;
	}

#ifdef __CRT_WIN32
	f->handle = handle;
#endif
	
	if (fl & __CRT_FOPEN_TMP)
	{
		size_t fnsz = strlen(fn);
		f->tmpfn = malloc(fnsz +1);
		memcpy(f->tmpfn, fn, fnsz);
		f->tmpfn[fnsz] = 0;
	}

end:;
	return f;
}



FILE* 
_C_DECL 
_CRT_fopen(const char* fn, const char* m, int fl)
{
	size_t fn_strlen = strlen(fn);
	if (!fn_strlen)
		return NULL;

	if (fl & __CRT_FOPEN_TMP)
		return _CRT_openFile(fn, __CRT_FOPEN_TMP | __CRT_FOPEN_WRITE | __CRT_FOPEN_UPDATE);

	size_t mode_strlen = strlen(m);
	if (!mode_strlen)
		return NULL;

	if (mode_strlen > 3)
		mode_strlen = 3;

	memcpy(__crt._->fopenModeUnion.chars, m, mode_strlen);

	unsigned int fopenflags = 0;

	switch (__crt._->fopenModeUnion.i)
	{
	case 114: //r
	case 25202: //rb
		fopenflags = __CRT_FOPEN_READ; // The file must exist.
		break;
	case 119: //w
	case 25207: //wb
		fopenflags = __CRT_FOPEN_WRITE; // Create or open and erase all
		break;
	case 97: //a
	case 25185: //ab
		fopenflags = __CRT_FOPEN_APPEND; // Create or open but not erase all
		break;
	case 11122: //r+
	case 2843250: //rb+
	case 6433650: //r+b
		fopenflags = __CRT_FOPEN_READ;   // The file must exist.
		fopenflags |= __CRT_FOPEN_UPDATE; // read and write
		break;
	case 11127: //w+
	case 2843255: //wb+
	case 6433655: //w+b
		fopenflags = __CRT_FOPEN_WRITE;   // create or open and erase
		fopenflags |= __CRT_FOPEN_UPDATE; //  read and write
		break;
	case 11105: //a+
	case 2843233: //ab+
	case 6433633: //a+b
		fopenflags = __CRT_FOPEN_APPEND; // Create or open but not erase all
		fopenflags |= __CRT_FOPEN_UPDATE; //  read and write
		break;
	default:
		return NULL;
	}
	
	if (fopenflags)
		return _CRT_openFile(fn, fopenflags);

	return NULL;
}

int 
_C_DECL 
fclose(FILE* stream)
{
	if (stream)
	{
		if (stream->tmpfn)
		{
#ifdef __CRT_WIN32
			CloseHandle(stream->handle);
#endif
			stream->handle = 0;
			remove(stream->tmpfn);
			free(stream->tmpfn);
			stream->tmpfn = 0;
			poolFree(&__crt._->tmpMaxPool, stream);			
		}
		else
		{
			fflush(stream);
#ifdef __CRT_WIN32
			CloseHandle(stream->handle);
#endif
			stream->handle = 0;
			poolFree(&__crt._->fopenMaxPool, stream);
		}
		return 0;
	}
	return EOF;
}

int 
_C_DECL 
fflush(FILE* stream)
{
	if (stream)
	{
#ifdef __CRT_WIN32
		if (FlushFileBuffers(stream->handle))
			return 0;

		stream->erri = EDOM;
#endif
	}
	return EOF;
}

FILE* 
_C_DECL 
fopen(const char* filename, const char* mode)
{
	return _CRT_fopen(filename, mode, 0);
}

FILE* 
_C_DECL 
tmpfile(void)
{
	srand((unsigned int)__rdtsc());
	char* fn = tmpnam(NULL);
	
	//if (__CRT_exist(fn) != __CRT_EXST_NEXST) // maybe ignore?
	//	return NULL;

	return _CRT_fopen(fn, "wb+", __CRT_FOPEN_TMP);
}

char* 
_C_DECL 
tmpnam(char* s)
{
	for (int i = 0; i < TMP_MAX; ++i)
	{
		for (int i2 = 0; i < L_tmpnam; ++i)
		{
		loop_begin:;
			int r = rand() % 94 + 32;
			if (!isalnum(r))
				goto loop_begin;

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
