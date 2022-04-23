#include "CRT.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "errno.h"
#include "stdarg.h"


#ifdef __CRT_WIN32
#include "WinAPI.h"
#endif

extern __CRT_main_struct __crt;
extern char __CRT_itoa_buffer[31];
extern char __CRT_dtoa_buffer[31];

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

int 
_C_DECL 
setvbuf(FILE* stream, char* buf, int mode, size_t size)
{
	return -1; //not supported;
}

int
_C_DECL
__CRT_vsnprintf(
	char* __restrict s, 
	size_t n, 
	const char* __restrict format, 
	va_list ap,
	FILE* stream, 
	int(*onFull)(FILE*, const char*, size_t) // return 1 for end
)
{
	size_t format_len = strlen(format);

	int state = 0; // 1 = %
	int buffer_current_size = 0;
	int result = 0;
	
	const char* arg_constChar = 0;

	for (size_t i = 0; i < format_len; ++i)
	{
		switch (format[i])
		{
		case '%':
			if (state == 1)
				goto _default;
			if (!state)
				state = 1;
			break;
		case 'd':
		case 'i':
		{
			if (!state)
				goto _default;
			int arg = va_arg(ap, int);
			__CRT_itoa(arg, __CRT_itoa_buffer, 30, 0);
			arg_constChar = __CRT_itoa_buffer;
			if (arg_constChar)
				goto putStr;
		}break;
		case 'x':
		case 'X':
		case 'o':
		case 'u':
		{
			if (!state)
				goto _default;
			unsigned int arg = va_arg(ap, unsigned int);

			int md = 0;
			switch (format[i])
			{
			case 'o':
				md = 3;
				break;
			case 'x':
				md = 1;
				break;
			case 'X':
				md = 2;
				break;
			default:
				break;
			}
			__CRT_uitoa(arg, __CRT_itoa_buffer, 30, md);

			arg_constChar = __CRT_itoa_buffer;
			if (arg_constChar)
				goto putStr;
		}break;
		case 'F':
		case 'f':
		case 'E':
		case 'e':
		{
			if (!state)
				goto _default;
			double arg = va_arg(ap, double);
			
			int md = 0;
			switch (format[i])
			{
			case 'F':
				md = 1;
				break;
			case 'f':
				break;
			case 'E':
				md = 3;
				break;
			case 'e':
				md = 2;
				break;
			}

			__CRT_dtoa(arg, __CRT_dtoa_buffer, 30, md);
			arg_constChar = __CRT_dtoa_buffer;
			if (arg_constChar)
				goto putStr;
			/*
			switch (format[i])
			{
			case 'o':
				md = 3;
				break;
			case 'x':
				md = 1;
				break;
			case 'X':
				md = 2;
				break;
			default:
				break;
			}
			__CRT_uitoa(arg, __CRT_itoa_buffer, 30, md);

			arg_constChar = __CRT_itoa_buffer;
			if (arg_constChar)
				goto putStr;*/
		}break;
		case 's':
		{
			if (!state)
				goto _default;
			arg_constChar = va_arg(ap, const char*);
			if (arg_constChar)
				goto putStr;
		}break;
		case 'c':
		{
			if (!state)
				goto _default;
			char c = va_arg(ap, char);
			__CRT_itoa_buffer[0] = c;
			__CRT_itoa_buffer[1] = 0;
			arg_constChar = __CRT_itoa_buffer;
			goto putStr;
		}break;
		default:
		_default:;
			state = 0;
			s[buffer_current_size++] = format[i];
			s[buffer_current_size] = 0;
			break;
		}
		if(arg_constChar)
		{
		putStr:;
			size_t argLen = strlen(arg_constChar);
			if (argLen)
			{
				for (size_t i2 = 0; i2 < argLen; ++i2)
				{
					s[buffer_current_size++] = arg_constChar[i2];
					s[buffer_current_size] = 0;
					if (buffer_current_size == n) // or n-1 ?
					{
						result += buffer_current_size;
						if (onFull)
						{
							if (onFull(stream, s, n))
							{
								return result;
							}
							buffer_current_size = 0;
						}
					}
				}
			}
			arg_constChar = 0;
		}

		if (buffer_current_size == n) // or n-1 ?
		{
			result += buffer_current_size;
			if (onFull)
			{
				if (onFull(stream, s, n))
				{
					return result;
				}
				buffer_current_size = 0;
			}
		}
	}
	return result ? result : -1;
}

int
_C_DECL
vsnprintf(char* __restrict s, size_t n, const char* __restrict format, va_list arg)
{
	int written_num = -1;
	if (s && n && format)
	{
		size_t format_len = strlen(format);
		if (format_len)
			written_num = __CRT_vsnprintf(s, n, format, arg, NULL, NULL);
	}
	return written_num;
}


int
_C_DECL
snprintf(char* __restrict str, size_t n, char const* __restrict fmt, ...)
{
	int r = -1;
	if (str && n && fmt)
	{
		va_list ap;
		va_start(ap, fmt);
		r = __CRT_vsnprintf(str, n, fmt, ap, NULL, NULL);
		va_end(ap);
	}
	return r;
}


char __CRT_vfprintf_buffer[200];
int __CRT_vfprintf_buffer_onFull(
	FILE* stream, 
	const char* buf, size_t sz)
{
	fwrite(buf, 1, sz, stream); // __CRT_vsnprintf must have `n` parameter < 200
	__CRT_vfprintf_buffer[0] = 0;
	return 0;
}
int
_C_DECL
__CRT_vfprintf(FILE* stream, const char* __restrict format, va_list vl)
{
	__CRT_vfprintf_buffer[199] = 0;
	int written_num = -1;
	if (format)
	{
		size_t format_len = strlen(format);
		if (format_len)
		{
			__CRT_vsnprintf(__CRT_vfprintf_buffer, 150, format, vl, stream, __CRT_vfprintf_buffer_onFull);
			if (__CRT_vfprintf_buffer[0])
			{
				fwrite(__CRT_vfprintf_buffer, 1, strlen(__CRT_vfprintf_buffer), stream);
				__CRT_vfprintf_buffer[0] = 0;
			}
		}
		else
			stream->erri = EDOM;
	}
	else
		stream->erri = EDOM;
	return written_num;
}

int 
_C_DECL 
fprintf(FILE* stream, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int r = __CRT_vfprintf(stream, format, ap);
	va_end(ap);
	return r;
}

int 
_C_DECL 
fscanf(FILE* stream, const char* format, ...)
{

}

int 
_C_DECL 
printf(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int r = __CRT_vfprintf(__crt._->_stdout, format, ap);
	va_end(ap);
	return r;
}

int 
_C_DECL 
scanf(const char* format, ...)
{

}

int 
_C_DECL 
sprintf(char* s, const char* format, ...)
{

}

int 
_C_DECL 
sscanf(char* s, const char* format, ...)
{

}

int 
_C_DECL 
vfprintf(FILE* stream, const char* format, va_list vl)
{
	return __CRT_vfprintf(stream, format, vl);
}

int 
_C_DECL 
vfscanf(FILE* stream, const char* format, va_list arg)
{

}

int 
_C_DECL 
vprintf(const char* format, va_list arg)
{
	return __CRT_vfprintf(__crt._->_stdout, format, arg);
}

int 
_C_DECL 
vscanf(const char* format, va_list arg)
{

}

int 
_C_DECL 
vsprintf(const char* s, const char* format, va_list arg)
{

}

int 
_C_DECL 
vsscanf(const char* s, const char* format, va_list arg)
{

}

