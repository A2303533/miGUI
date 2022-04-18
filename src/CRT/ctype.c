#include "CRT.h"
#include "stdlib.h"
#include "ctype.h"

int 
_C_DECL 
isalnum(int c)
{
	if (c > 47 && c < 58)
		return 1;
	if (c > 64 && c < 91)
		return 1;
	if (c > 96 && c < 123)
		return 1;
	return 0;
}

int 
_C_DECL 
isalpha(int c)
{
	if (c > 64 && c < 91)
		return 1;
	if (c > 96 && c < 123)
		return 1;
	return 0;
}

int 
_C_DECL 
isblank(int c)
{
	if (c == ' ' || c == '\t')
		return 1;
	return 0;
}

int 
_C_DECL 
iscntrl(int c)
{
	if ((c >= 0 && c < 32) || c == 127)
		return 1;
	return 0;
}

int 
_C_DECL 
isdigit(int c)
{
	if (c > 47 && c < 58)
		return 1;
	return 0;
}

int 
_C_DECL 
isgraph(int c)
{
	if (c > 32 && c < 129)
		return 1;
	if (c > 129 && c < 140)
		return 1;
	if (c > 144 && c < 157)
		return 1;
	if (c > 142)
		return 1;
	if (c > 157 && c < 160)
		return 1;
	if (c > 160 && c < 256)
		return 1;
	return 0;
}

int 
_C_DECL 
islower(int c)
{
	if (c > 96 && c < 123)
		return 1;
	return 0;
}

int 
_C_DECL 
isprint(int c)
{
	if (isgraph(c))
		return 1;
	return c == ' ' ? 1 : 0;
}

int 
_C_DECL 
ispunct(int c)
{
	if (c > 32 && c < 48)
		return 1;
	if (c > 57 && c < 65)
		return 1;
	if (c > 90 && c < 97)
		return 1;
	if (c > 122 && c < 128)
		return 1;
	return 0;
}

int 
_C_DECL 
isspace(int c)
{
	if (c == ' ') goto good;
	if (c == '\t') goto good;
	if (c == '\n') goto good;
	if (c == '\v') goto good;
	if (c == '\f') goto good;
	if (c == '\r') goto good;
	return 0;
good:;
	return 1;
}

int 
_C_DECL 
isupper(int c)
{
	if (c > 64 && c < 91)
		return 1;
	return 0;
}

int 
_C_DECL 
isxdigit(int c)
{
	if (c > 47 && c < 58)
		return 1;
	if (c > 64 && c < 71)
		return 1;
	if (c > 96 && c < 103)
		return 1;
	return 0;
}

int 
_C_DECL 
tolower(int c)
{
	if (c > 64 && c < 91)
		return c + 32;
	return c;
}

int 
_C_DECL 
toupper(int c)
{
	if (c > 96 && c < 123)
		return c - 32;
	return c;
}
