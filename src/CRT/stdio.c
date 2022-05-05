#include "CRT.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "errno.h"
#include "stdarg.h"
//#include "wchar.h"


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
	if (stream)
	{

		if (stream->handle)
		{
			WriteFile(stream->handle, ptr, nmemb * size, &written, NULL);
		}
		else if (!stream->handle && stream->buf)
		{
			uint8_t* bytes = ptr;
			for (size_t i1 = 0; i1 < size; ++i1)
			{
				for (size_t i2 = 0; i2 < nmemb; ++i2)
				{
					if (stream->pos < stream->bufSz)
					{
						stream->buf[stream->pos] = bytes[i2];
						++stream->pos;
					}
				}
			}
		}
		stream->pos += written;
	}
#endif
	return written;
}

int 
_C_DECL 
setvbuf(FILE* stream, char* buf, int mode, size_t size)
{
	return -1; //not supported;
}

const char*
_C_DECL
__CRT_vprintf_getNumber(const char* fmt, int* number)
{
	static char buffer[30];
	int i = 0;
	while (*fmt)
	{
		switch (*fmt)
		{
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
		{
			buffer[i] = *fmt;
			buffer[i+1] = 0;
			++fmt;
			++i;
		}break;
		default:
			goto out;
		}
	}
out:;

	if (buffer[0])
		*number = atoi(buffer);

	return fmt;
}
// Get valid specifier. That thing after %.
// If not valid, return next char (skip %).
// type:
enum __CRT_vprintf_type {
	__CRT_vprintf_type_nothing = 0,
	__CRT_vprintf_type_c,
	__CRT_vprintf_type_lc,
	__CRT_vprintf_type_d_i,
	__CRT_vprintf_type_ld_li,
	__CRT_vprintf_type_lld_lli,
	__CRT_vprintf_type_x,
	__CRT_vprintf_type_X,
	__CRT_vprintf_type_o,
	__CRT_vprintf_type_s,
	__CRT_vprintf_type_ls,
};
//  0 - nothing, skip
//  1 - c
//  2 - lc
//  3 - d\i
//  4 - ld\li
//  5 - lld\lli
//  6 - x
//  7 - X
//  8 - o
//  9 - s
//  10 - ls
// leftPads: number of ' ' or '0' to write before writing the variable
// flags:
//   0x1 - pads with ' '
//   0x2 - pads with '0'
//   0x4 - '0x'
//   0x8 - '0X'
//   0x10 - '0' for ctal
const char*
_C_DECL
__CRT_vprintf_(const char* fmt, int* type, int* leftPads, int* flags)
{
	// *fmt is '%' so next char
	++fmt;

	const char* saveFmt = fmt;
	*type = __CRT_vprintf_type_nothing;
	*leftPads = 0;
	*flags = 0;

begin:;
	switch (*fmt)
	{
	case '#':
	{
		++fmt;
		// #x #X #o
		switch (*fmt)
		{
		case 'x':
			*type = __CRT_vprintf_type_x;
			*flags |= 0x4;
			++fmt;
			break;
		case 'X':
			*type = __CRT_vprintf_type_X;
			*flags |= 0x8;
			++fmt;
			break;
		case 'o':
			*type = __CRT_vprintf_type_o;
			*flags |= 0x10;
			++fmt;
			break;
		}
	}break;
	case 'o':
		++fmt;
		*type = __CRT_vprintf_type_o;
		break;
	case 'x':
		++fmt;
		*type = __CRT_vprintf_type_x;
		break;
	case 'X':
		++fmt;
		*type = __CRT_vprintf_type_X;
		break;
	case '0':
	{
		++fmt;
		*flags |= 0x2;

		// now how much leftPads
		switch (*fmt)
		{
		case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
		{
			fmt = __CRT_vprintf_getNumber(fmt, leftPads);
			// now *fmt must be d\i\li\lli\ld\lld or other...
			// easy to use goto
			goto begin;
		}break;
		default:
			break;
		}
	}break;
	case '1':case '2':case '3':case '4':
	case '5':case '6':case '7':case '8':case '9':
	{
		// need to get number
		fmt = __CRT_vprintf_getNumber(fmt, leftPads);

		// it can be pads with ' '
		*flags |= 0x1;

		// next fmt can be '.'
		switch (*fmt)
		{
		case '.':
			break;
		default: // no, probably *fmt is c d i or other
			goto begin;
			break;
		}

	}break;
	case 's':
		++fmt;
		*type = __CRT_vprintf_type_s;
		break;
	case 'c':
	{
		*type = __CRT_vprintf_type_c;
		++fmt;
	}break;
	case 'i':
	case 'd':
	{
		*type = __CRT_vprintf_type_d_i;
		++fmt;
	}break;
	case 'l':
	{
		// li\ld lli\lld lc ls

		++fmt;
		if (*fmt)
		{
			switch (*fmt)
			{
			case 's':
				*type = __CRT_vprintf_type_ls;
				++fmt;
				break;
			case 'c':
				*type = __CRT_vprintf_type_lc;
				++fmt;
				break;
			case 'd':
			case 'i':
				*type = __CRT_vprintf_type_ld_li;
				++fmt;
				break;
			case 'l':
			{
				++fmt;
				if (*fmt)
				{
					switch (*fmt)
					{
					case 'd':
					case 'i':
						*type = __CRT_vprintf_type_lld_lli;
						++fmt;
						break;
					}
				}
			}break;
			default:
				break;
			}
		}
	}break;
	default:
		break;
	}

	if (*type == 0)
		fmt = saveFmt;

	return fmt;
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
		{
			FILE f;
			memset(&f, 0, sizeof(FILE));

			f.buf = s;
			f.bufSz = n;

			return vfprintf(&f, format, arg);
		}
	}
	return written_num;
}

int
_C_DECL
snprintf(char* /*__restrict*/ str, size_t n, char const* /*__restrict*/ fmt, ...)
{
	int r = -1;
	if (str && n && fmt)
	{
		va_list ap;
		va_start(ap, fmt);
		r = vsnprintf(str, n, fmt, ap, NULL, NULL);
		va_end(ap);
	}
	return r;
}

int 
_C_DECL 
fprintf(FILE* stream, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int r = vfprintf(stream, format, ap);
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
	int r = vfprintf(__crt._->_stdout, format, ap);
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
	va_list ap;
	va_start(ap, format);
	int r = vsprintf(s, format, ap);
	va_end(ap);
	return r;
}

int 
_C_DECL 
sscanf(char* s, const char* format, ...)
{

}

int 
_C_DECL 
vfprintf(FILE* stream, const char* format, va_list ap)
{
	int written_num = 0;
	if (format)
	{
		size_t format_len = strlen(format);
		if (format_len)
		{
			size_t format_len = strlen(format);
			//int buffer_current_size = 0;
			const char* arg_constChar = 0;
			const wchar_t* arg_constWchar = 0;

			int type = __CRT_vprintf_type_nothing;
			int flags = 0;
			int leftPad = 0;

			const char* fmt = format;
			while (*fmt)
			{
				type = 0;
				flags = 0;
				leftPad = 0;

				switch (*fmt)
				{
				case '%':
					fmt = __CRT_vprintf_(fmt, &type, &leftPad, &flags);
					if (!type)
					{
						//	s[buffer_current_size++] = *fmt; // must be '%'
						//	s[buffer_current_size] = 0;
						written_num += fputc('%', stream);
						++fmt;
					}
					break;
				default:
					written_num += fputc(*fmt, stream);
					++fmt;
					break;
				}

				if (type)
				{
					arg_constChar = 0;

					switch (type)
					{
					case __CRT_vprintf_type_c:
					case __CRT_vprintf_type_lc:
					{
						int c = va_arg(ap, int);
						__CRT_itoa_buffer[0] = (char)c;
						__CRT_itoa_buffer[1] = 0;
						if (leftPad && (flags & 0x1))
						{
							flags = 0;
							int leftPadNum = leftPad - 1;
							if (leftPadNum)
							{
								//__CRT_itoa_buffer[leftPadNum] = (char)c;
								//__CRT_itoa_buffer[leftPadNum + 1] = 0;
								for (int i = 0; i < leftPadNum; ++i)
								{
									//__CRT_itoa_buffer[i] = ' ';
									written_num += fputc(' ', stream);
								}
							}
						}

						arg_constChar = __CRT_itoa_buffer;
					}break;
					case __CRT_vprintf_type_d_i:
					{
						int arg = va_arg(ap, int);
						__CRT_itoa(arg, __CRT_itoa_buffer, 30, 0);
						arg_constChar = __CRT_itoa_buffer;
					}break;
					case __CRT_vprintf_type_ld_li:
					{
						long int arg = va_arg(ap, long int);
						__CRT_itoa(arg, __CRT_itoa_buffer, 30, 0);
						arg_constChar = __CRT_itoa_buffer;
					}break;
					case __CRT_vprintf_type_lld_lli:
					{
						long long int arg = va_arg(ap, long long int);
						__CRT_lltoa(arg, __CRT_itoa_buffer, 30, 0);
						arg_constChar = __CRT_itoa_buffer;
					}break;
					case __CRT_vprintf_type_x: //x
					{
						unsigned int arg = va_arg(ap, unsigned int);
						__CRT_uitoa(arg, __CRT_itoa_buffer, 30, 1);
						arg_constChar = __CRT_itoa_buffer;
						if (flags & 0x4)
						{
							fputc('0', stream);
							fputc('x', stream);
						}
					}break;
					case __CRT_vprintf_type_X: //X
					{
						unsigned int arg = va_arg(ap, unsigned int);
						__CRT_uitoa(arg, __CRT_itoa_buffer, 30, 2);
						arg_constChar = __CRT_itoa_buffer;
						if (flags & 0x8)
						{
							fputc('0', stream);
							fputc('X', stream);
						}
					}break;
					case __CRT_vprintf_type_o: //o
					{
						unsigned int arg = va_arg(ap, unsigned int);
						__CRT_uitoa(arg, __CRT_itoa_buffer, 30, 3);
						arg_constChar = __CRT_itoa_buffer;
						if (flags & 0x10)
							fputc('0', stream);
					}break;
					case __CRT_vprintf_type_s: //s
					{
						arg_constChar = va_arg(ap, char*);
					}break;
					case __CRT_vprintf_type_ls:
					{
						arg_constWchar = va_arg(ap, wchar_t*);
						if (arg_constWchar)
							fwprintf(stream, "%s", arg_constWchar);
					}break;
					}

					if (arg_constChar)
					{
						size_t argLen = strlen(arg_constChar);
						if (argLen)
						{
							if (leftPad)
							{
								if ((flags & 0x1) || (flags & 0x2))
								{
									int leftPadNum = leftPad - argLen;
									if (leftPadNum > 0)
									{
										char leftPadChar = ' ';
										if (flags & 0x2)
											leftPadChar = '0';
										// copy of code below
										for (int i = 0; i < leftPadNum; ++i)
										{
											written_num += fputc(leftPadChar, stream);
										}
									}
								}
							}

							for (size_t i2 = 0; i2 < argLen; ++i2)
							{
								written_num += fputc(arg_constChar[i2], stream);
							}
						}
					}
				}
			}
			return written_num ? written_num : EDOM;
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
vfscanf(FILE* stream, const char* format, va_list arg)
{

}

int 
_C_DECL 
vprintf(const char* format, va_list arg)
{
	return vfprintf(__crt._->_stdout, format, arg);
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
	return vsnprintf(s, 0xffff, format, arg);
}

int 
_C_DECL 
vsscanf(const char* s, const char* format, va_list arg)
{

}

int
_C_DECL
fputc(int c, FILE* stream)
{
	char ch = (char)c;

	/*if (stream->handle)
	{*/
	if (fwrite(&c, 1, 1, stream))
		return 1;
	/*}
	else if(stream->buf)
	{

	}*/

	return EOF;
}
