#include "CRT.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "errno.h"

#ifdef __CRT_WIN32
#include "WinAPI.h"
#endif

extern __CRT_main_struct __crt;

FILE* 
_C_DECL 
__CRT_stdin()
{
	return __crt._->_stdin;
}

FILE* 
_C_DECL 
__CRT_stdout()
{
	return __crt._->_stdout;
}

FILE* 
_C_DECL 
__CRT_stderr()
{
	return __crt._->_stderr;
}

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
	unsigned int dwCreationDisposition = CREATE_ALWAYS;
	unsigned int dwShareMode = 0;// FILE_SHARE_READ;

	if (fl & __CRT_FOPEN_ISDEVICE)
	{
		dwFlagsAndAttributes = 0;
	}

	if (fl & __CRT_FOPEN_TMP)
		dwFlagsAndAttributes = FILE_ATTRIBUTE_TEMPORARY;

	if(fl & __CRT_FOPEN_READ)
		dwDesiredAccess |= GENERIC_READ;

	if (fl & __CRT_FOPEN_WRITE)
		dwDesiredAccess |= GENERIC_WRITE;

	if (fl & __CRT_FOPEN_APPEND)
		dwDesiredAccess |= GENERIC_WRITE;



	if (fl & __CRT_FOPEN_SHAREMODE_READ)
		dwShareMode |= FILE_SHARE_READ;

	if (fl & __CRT_FOPEN_SHAREMODE_WRITE)
		dwShareMode |= FILE_SHARE_WRITE;

	if (fl & __CRT_FOPEN_CREATION_OPENEXST)
		dwCreationDisposition = OPEN_EXISTING;
	else if (fl & __CRT_FOPEN_CREATION_CREATENEW)
		dwCreationDisposition = CREATE_ALWAYS;
	else if (fl & __CRT_FOPEN_CREATION_CREATEOPEN)
		dwCreationDisposition = OPEN_ALWAYS;


	void* handle = CreateFileA(fn, dwDesiredAccess, dwShareMode, NULL, 
		dwCreationDisposition, dwFlagsAndAttributes, NULL);
	
	if(!handle)
		goto end;
#endif

	f = calloc(1, sizeof(FILE));

	if (!f)
	{
#ifdef __CRT_WIN32
		CloseHandle(handle);
#endif
		goto end;
	}

	f->flags = fl;

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
		}
		else
		{
			if (stream->handle)
			{
				fflush(stream);
#ifdef __CRT_WIN32
				CloseHandle(stream->handle);
#endif
				stream->handle = 0;
			}
		}

		free(stream);
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
		if (stream->handle)
		{
			if (FlushFileBuffers(stream->handle))
				return 0;
		}

		stream->erri = EDOM;
#endif
	}
	return EOF;
}

FILE* 
_C_DECL 
fopen(const char* filename, const char* m)
{
	size_t fn_strlen = strlen(filename);
	if (!fn_strlen)
		return NULL;

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
		// The file must exist.
		fopenflags = __CRT_FOPEN_READ 
			| __CRT_FOPEN_CREATION_OPENEXST
			| __CRT_FOPEN_SHAREMODE_READ 
			| __CRT_FOPEN_SHAREMODE_WRITE; 
		break;
	case 119: //w
	case 25207: //wb
		fopenflags = __CRT_FOPEN_WRITE | __CRT_FOPEN_CREATION_CREATENEW; // Create or open and erase all
		break;
	case 97: //a
	case 25185: //ab
		fopenflags = __CRT_FOPEN_WRITE 
			| __CRT_FOPEN_APPEND
			| __CRT_FOPEN_CREATION_CREATEOPEN; // Create or open but not erase all
		break;
	case 11122: //r+
	case 2843250: //rb+
	case 6433650: //r+b
		fopenflags = __CRT_FOPEN_READ 
			| __CRT_FOPEN_WRITE
			| __CRT_FOPEN_CREATION_OPENEXST
			| __CRT_FOPEN_SHAREMODE_READ
			| __CRT_FOPEN_SHAREMODE_WRITE;   // The file must exist.
		break;
	case 11127: //w+
	case 2843255: //wb+
	case 6433655: //w+b
		fopenflags = __CRT_FOPEN_READ
			| __CRT_FOPEN_WRITE
			| __CRT_FOPEN_CREATION_CREATENEW
			| __CRT_FOPEN_SHAREMODE_READ
			| __CRT_FOPEN_SHAREMODE_WRITE;   // create or open and erase
		break;
	case 11105: //a+
	case 2843233: //ab+
	case 6433633: //a+b
		fopenflags = __CRT_FOPEN_READ
			| __CRT_FOPEN_WRITE
			| __CRT_FOPEN_CREATION_CREATEOPEN
			| __CRT_FOPEN_SHAREMODE_READ
			| __CRT_FOPEN_SHAREMODE_WRITE; // Create or open but not erase all
		break;
	default:
		return NULL;
	}

	if (filename[0] == 'C')
	{
		if (strcmp(filename, "CONIN$") == 0)
		{
			fopenflags = __CRT_FOPEN_READ
				| __CRT_FOPEN_WRITE
				| __CRT_FOPEN_ISDEVICE
				| __CRT_FOPEN_SHAREMODE_READ
				| __CRT_FOPEN_CREATION_OPENEXST;
		}
		else if (strcmp(filename, "CONOUT$") == 0)
		{
			fopenflags = __CRT_FOPEN_READ
				| __CRT_FOPEN_WRITE
				| __CRT_FOPEN_ISDEVICE
				| __CRT_FOPEN_SHAREMODE_WRITE
				| __CRT_FOPEN_CREATION_OPENEXST;
		}
	}

	return _CRT_openFile(filename, fopenflags);
}

FILE* 
_C_DECL 
tmpfile(void)
{
	srand((unsigned int)__rdtsc());
	char* fn = tmpnam(NULL);
	return _CRT_openFile(fn, __CRT_FOPEN_TMP 
		| __CRT_FOPEN_SHAREMODE_READ 
		| __CRT_FOPEN_SHAREMODE_WRITE
		| __CRT_FOPEN_WRITE
		| __CRT_FOPEN_CREATION_CREATENEW
	);
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

FILE* 
_C_DECL 
freopen(const char* filename, const char* mode, FILE* stream)
{
	if (stream)
	{
		if (stream->handle)
		{
#ifdef __CRT_WIN32
			CloseHandle(stream->handle);
#endif
		}

		if (filename)
		{
			FILE * nf = fopen(filename, mode);
			if (nf)
			{
				*stream = *nf;
				memset(nf, 0, sizeof(FILE));
				fclose(nf);
				return stream;
			}
		}
		else
		{
			/*If filename is a null pointer, the freopen function attempts to change the mode of
			the stream to that specified by mode, as if the name of the file currently associated with
			the stream had been used. It is implementation-defined which changes of mode are
			permitted (if any), and under what circumstances.*/
		}
	}
	return NULL;
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

size_t 
_C_DECL 
fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	unsigned int written = 0;
#ifdef __CRT_WIN32
	WriteFile(stream->handle, ptr, nmemb * size, &written, NULL);
#endif
	stream->pos += written;
	return written;
}
