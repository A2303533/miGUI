/*
  Copyright (C) 2022 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "miGUI.h"
#include <math.h>

#define FPUT_UNI(s,bcs,c,wn,sz) s[bcs++]=c;\
s[bcs]=0; \
++wn; \
if(bcs==sz)return wn;

static const char __stdlib_int_to_hex_table[16] =
{
	'0', '1', '2', '3','4','5','6','7','8','9','a','b','c','d','e','f',
};

uint64_t
MG_C_DECL
mgDecToOctLL(uint64_t dec)
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

void
MG_C_DECL
mgDecToHEX(uint64_t dec, char* buf, int* sz)
{
	static char __CRT_itoa_internalBuffer[30];
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

static struct mgUnicodeCharNode g_UnicodeChars[0x32000] =
{
#include "framework/UnicodeChars.inl"
};

struct mgUnicodeCharNode* 
	MG_C_DECL 
	mgGetUnicodeTable()
{
	return g_UnicodeChars;
}

void 
MG_C_DECL 
mgUnicodeFromUTF8(const char* str, mgUnicodeChar* out, size_t* lenOut)
{
	assert(str);
	assert(out);
	assert(lenOut);

}

void 
MG_C_DECL 
mgUnicodeFromUTF16(const wchar_t* str, mgUnicodeChar* out, size_t* lenOut)
{
	assert(str);
	assert(out);
	assert(lenOut);

}

void 
MG_C_DECL 
mgUnicodeToUTF8(const mgUnicodeChar* str, char* out, size_t* lenOut)
{
	assert(str);
	assert(out);
	assert(lenOut);

}

void 
MG_C_DECL 
mgUnicodeToUTF16(const mgUnicodeChar* str, wchar_t* out, size_t* lenOut)
{
	assert(str);
	assert(out);
	assert(lenOut);

}

size_t 
MG_C_DECL 
mgUnicodeStrlen(const mgUnicodeChar* str)
{
	size_t sz = 0;
	while (*str++)
	{
		++sz;
	}
	return sz;
}

int
MG_C_DECL 
mgUnicodeSnprintf(mgUnicodeChar* str, size_t sz, const mgUnicodeChar* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	int l = mgUnicodeVSnprintf(str, sz, fmt, vl);
	va_end(vl);
	return l;
}

void 
MG_C_DECL 
mgUnicodeLLTOA(int64_t val, mgUnicodeChar* buf, size_t bufSz, int mode)
{
	if (val)
	{
		static char __CRT_itoa_internalBuffer[30];

		int64_t val2 = val;

		switch (mode)
		{
		case 0:
		case 3:
		default:
		{
			if (mode == 3)
				val2 = mgDecToOctLL(val);
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
			mgDecToHEX(val, hexbuf, &bsz);
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
MG_C_DECL
mgUnicodeULLTOA(uint64_t val, mgUnicodeChar* buf, size_t bufSz, int mode)
{
	static char __CRT_itoa_internalBuffer[30];
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
				val2 = mgDecToOctLL(val);
			int bufferIndex = 0;
			while (1)
			{
				uint64_t mod = val2 % 10;
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
			mgDecToHEX(val, hexbuf, &bsz);
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
					if (islower((int)buf[i2]))
						buf[i2] = toupper((int)buf[i2]);

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

const mgUnicodeChar*
MG_C_DECL
_mgUnicode_CRT_vprintf_getNumber(const mgUnicodeChar* fmt, int* number)
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
			buffer[i + 1] = 0;
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
enum _mgUnicode_CRT_vprintf_type {
	_mgUnicode_CRT_vprintf_type_nothing = 0,
	_mgUnicode_CRT_vprintf_type_c,
	_mgUnicode_CRT_vprintf_type_lc,
	_mgUnicode_CRT_vprintf_type_d_i,
	_mgUnicode_CRT_vprintf_type_ld_li,
	_mgUnicode_CRT_vprintf_type_lld_lli,
	_mgUnicode_CRT_vprintf_type_x,
	_mgUnicode_CRT_vprintf_type_X,
	_mgUnicode_CRT_vprintf_type_o,
	_mgUnicode_CRT_vprintf_type_s,
	_mgUnicode_CRT_vprintf_type_ls,
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
const mgUnicodeChar*
MG_C_DECL
_mgUnicode_CRT_vprintf_(const mgUnicodeChar* fmt, int* type, int* leftPads, int* flags)
{
	// *fmt is '%' so next char
	++fmt;

	const mgUnicodeChar* saveFmt = fmt;
	*type = _mgUnicode_CRT_vprintf_type_nothing;
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
			*type = _mgUnicode_CRT_vprintf_type_x;
			*flags |= 0x4;
			++fmt;
			break;
		case 'X':
			*type = _mgUnicode_CRT_vprintf_type_X;
			*flags |= 0x8;
			++fmt;
			break;
		case 'o':
			*type = _mgUnicode_CRT_vprintf_type_o;
			*flags |= 0x10;
			++fmt;
			break;
		}
	}break;
	case 'o':
		++fmt;
		*type = _mgUnicode_CRT_vprintf_type_o;
		break;
	case 'x':
		++fmt;
		*type = _mgUnicode_CRT_vprintf_type_x;
		break;
	case 'X':
		++fmt;
		*type = _mgUnicode_CRT_vprintf_type_X;
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
			fmt = _mgUnicode_CRT_vprintf_getNumber(fmt, leftPads);
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
		fmt = _mgUnicode_CRT_vprintf_getNumber(fmt, leftPads);

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
		*type = _mgUnicode_CRT_vprintf_type_s;
		break;
	case 'c':
	{
		*type = _mgUnicode_CRT_vprintf_type_c;
		++fmt;
	}break;
	case 'i':
	case 'd':
	{
		*type = _mgUnicode_CRT_vprintf_type_d_i;
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
				*type = _mgUnicode_CRT_vprintf_type_ls;
				++fmt;
				break;
			case 'c':
				*type = _mgUnicode_CRT_vprintf_type_lc;
				++fmt;
				break;
			case 'd':
			case 'i':
				*type = _mgUnicode_CRT_vprintf_type_ld_li;
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
						*type = _mgUnicode_CRT_vprintf_type_lld_lli;
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
MG_C_DECL 
mgUnicodeVSnprintf(mgUnicodeChar* str, size_t sz, const mgUnicodeChar* format, va_list ap)
{
	int written_num = 0;
	if (format)
	{
		static mgUnicodeChar unicodeBuffer[30];
		static mgUnicodeChar itoa_buffer[30];

		size_t format_len = mgUnicodeStrlen(format);
		if (format_len)
		{
			size_t buffer_current_size = 0;

			const mgUnicodeChar* arg_constChar = 0;
			const char* arg_constChar8 = 0;
			const wchar_t* arg_constChar16 = 0;

			int type = _mgUnicode_CRT_vprintf_type_nothing;
			int flags = 0;
			int leftPad = 0;

			const mgUnicodeChar* fmt = format;
			while (*fmt)
			{
				type = 0;
				flags = 0;
				leftPad = 0;

				switch (*fmt)
				{
				case '%':
					fmt = _mgUnicode_CRT_vprintf_(fmt, &type, &leftPad, &flags);
					if (!type)
					{
						FPUT_UNI(str, buffer_current_size, *fmt, written_num, sz);
						//str[buffer_current_size++] = *fmt; // must be '%'
						//str[buffer_current_size] = 0;
						//++written_num;
						//if (buffer_current_size == sz)
						//	return written_num;

						//written_num += fputc('%', stream);
						++fmt;
					}
					break;
				default:
					//written_num += fputc(*fmt, stream);
					
					/*str[buffer_current_size++] = *fmt;
					str[buffer_current_size] = 0;
					++written_num;
					if (buffer_current_size == sz)
						return written_num;*/
					FPUT_UNI(str, buffer_current_size, *fmt, written_num, sz);

					++fmt;
					break;
				}

				if (type)
				{
					arg_constChar = 0;

					switch (type)
					{
					case _mgUnicode_CRT_vprintf_type_c:
					case _mgUnicode_CRT_vprintf_type_lc:
					{
						int c = va_arg(ap, int);
						unicodeBuffer[0] = (mgUnicodeChar)c;
						unicodeBuffer[1] = 0;
						//__CRT_itoa_buffer[0] = (char)c;
						//__CRT_itoa_buffer[1] = 0;
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
									
									//written_num += fputc(' ', stream);
									
									/*str[buffer_current_size++] = ' ';
									str[buffer_current_size] = 0;
									++written_num;
									if (buffer_current_size == sz)
										return written_num;*/
									FPUT_UNI(str, buffer_current_size, ' ', written_num, sz);
								}
							}
						}

						arg_constChar = unicodeBuffer;
					}break;
					case _mgUnicode_CRT_vprintf_type_d_i:
					{
						int arg = va_arg(ap, int);
						mgUnicodeLLTOA(arg, itoa_buffer, 30, 0);
						arg_constChar = itoa_buffer;
					}break;
					case _mgUnicode_CRT_vprintf_type_ld_li:
					{
						long int arg = va_arg(ap, long int);
						mgUnicodeLLTOA(arg, itoa_buffer, 30, 0);
						arg_constChar = itoa_buffer;
					}break;
					case _mgUnicode_CRT_vprintf_type_lld_lli:
					{
						long long int arg = va_arg(ap, long long int);
						mgUnicodeLLTOA(arg, itoa_buffer, 30, 0);
						arg_constChar = itoa_buffer;
					}break;
					case _mgUnicode_CRT_vprintf_type_x: //x
					{
						unsigned int arg = va_arg(ap, unsigned int);
						mgUnicodeULLTOA(arg, itoa_buffer, 30, 1);
						arg_constChar = itoa_buffer;
						if (flags & 0x4)
						{
							//fputc('0', stream);
							//fputc('x', stream);
							FPUT_UNI(str, buffer_current_size, '0', written_num, sz);
							FPUT_UNI(str, buffer_current_size, 'x', written_num, sz);
						}
					}break;
					case _mgUnicode_CRT_vprintf_type_X: //X
					{
						unsigned int arg = va_arg(ap, unsigned int);
						mgUnicodeULLTOA(arg, itoa_buffer, 30, 2);
						arg_constChar = itoa_buffer;
						if (flags & 0x8)
						{
							//fputc('0', stream);
							//fputc('X', stream);
							FPUT_UNI(str, buffer_current_size, '0', written_num, sz);
							FPUT_UNI(str, buffer_current_size, 'X', written_num, sz);
						}
					}break;
					case _mgUnicode_CRT_vprintf_type_o: //o
					{
						unsigned int arg = va_arg(ap, unsigned int);
						mgUnicodeULLTOA(arg, itoa_buffer, 30, 3);
						arg_constChar = itoa_buffer;
						if (flags & 0x10)
						{
							//fputc('0', stream);
							FPUT_UNI(str, buffer_current_size, '0', written_num, sz);
						}
					}break;
					case _mgUnicode_CRT_vprintf_type_s: //s
					{
						arg_constChar8 = va_arg(ap, char*);
					}break;
					case _mgUnicode_CRT_vprintf_type_ls:
					{
						arg_constChar16 = va_arg(ap, wchar_t*);
						//if (arg_constChar16)
						//{
						//	fwprintf(stream, "%s", arg_constWchar);

						//}
					}break;
					}

					if (arg_constChar)
					{
						size_t argLen = mgUnicodeStrlen(arg_constChar);
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
											//written_num += fputc(leftPadChar, stream);
											FPUT_UNI(str, buffer_current_size, leftPadChar, written_num, sz);
										}
									}
								}
							}

							for (size_t i2 = 0; i2 < argLen; ++i2)
							{
								FPUT_UNI(str, buffer_current_size, arg_constChar[i2], written_num, sz);
								//written_num += fputc(arg_constChar[i2], stream);
							}
						}
					}
					if (arg_constChar8)
					{
						size_t argLen = strlen(arg_constChar8);
						if (argLen)
						{
							unsigned char c1 = 0;
							unsigned char c2 = 0;
							unsigned char c3 = 0;
							unsigned char c4 = 0;

							for (size_t i2 = 0; i2 < argLen; ++i2)
							{
								c1 = arg_constChar8[i2];

								if (c1 <= 0x7F)
								{
									FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)c1, written_num, sz);
									//Append((char32_t)c1);
								}
								else
								{
									if ((c1 & 0xE0) == 0xC0) //2 bytes
									{
										++i2;
										if (i2 < argLen)
										{
											c2 = arg_constChar8[i2];
											if ((c2 & 0xC0) == 0x80)
											{
												wchar_t wch = (c1 & 0x1F) << 6;
												wch |= (c2 & 0x3F);
												//Append((char32_t)wch);
												FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)wch, written_num, sz);
											}
										}
									}
									else if ((c1 & 0xF0) == 0xE0) //3
									{
										++i2;
										if (i2 < argLen)
										{
											c2 = arg_constChar8[i2];
											if ((c2 & 0xC0) == 0x80)
											{
												++i2;
												if (i2 < argLen)
												{
													c3 = arg_constChar8[i2];
													if ((c3 & 0xC0) == 0x80)
													{
														wchar_t wch = (c1 & 0xF) << 12;
														wch |= (c2 & 0x3F) << 6;
														wch |= (c3 & 0x3F);
														//Append((char32_t)wch);
														FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)wch, written_num, sz);
													}
												}
											}
										}
									}
									else if ((c1 & 0xF8) == 0xF0) //4
									{
										++i2;
										if (i2 < argLen)
										{
											c2 = arg_constChar8[i2];
											if ((c2 & 0xC0) == 0x80)
											{
												++i2;
												if (i2 < argLen)
												{
													c3 = arg_constChar8[i2];
													if ((c3 & 0xC0) == 0x80)
													{
														++i2;
														if (i2 < argLen)
														{
															c4 = arg_constChar8[i2];
															if ((c4 & 0xC0) == 0x80)
															{
																uint32_t u = (c1 & 0x7) << 18;
																u |= (c2 & 0x3F) << 12;
																u |= (c3 & 0x3F) << 6;
																u |= (c4 & 0x3F);

																FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)u, written_num, sz);
																//	Append((char32_t)u);
															}
														}
													}
												}
											}
										}
									}

								}
							}
						}
					}
					if (arg_constChar16)
					{
						size_t argLen = wcslen(arg_constChar16);
						if (argLen)
						{
							for (size_t i2 = 0; i2 < argLen; ++i2)
							{
								wchar_t ch16 = arg_constChar16[i2];
								if (ch16 < 0x80)
								{
									FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)ch16, written_num, sz);
								}
								else if (ch16 < 0x800)
								{
									FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)ch16, written_num, sz);
									//Append((char32_t)ch16); //????
								}
								else if ((ch16 & 0xFC00) == 0xD800)
								{
									++i2;
									if (i2 < argLen)
									{
										wchar_t ch16_2 = arg_constChar16[i2];
										if ((ch16_2 & 0xFC00) == 0xDC00)
										{
											uint32_t u = (ch16 - 0xD800) * 0x400;
											u += (ch16_2 - 0xDC00) + 0x10000;

											FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)u, written_num, sz);
											//Append((char32_t)u);
										}
									}
								}
								else
								{
									FPUT_UNI(str, buffer_current_size, (mgUnicodeChar)ch16, written_num, sz);
									//Append((char32_t)ch16);
								}
							}
						}
					}
				}
			}
			return written_num ? written_num : EDOM;
		}
	}
	return written_num;
}



//void
//mgTextProcessor_drawText(
//	int reason, 
//	struct mgTextProcessor_s* tp, 
//	mgPoint* position, 
//	const mgUnicodeChar* text, 
//	size_t textLen, 
//	struct mgStyle_s* style)
//{
//	mgPoint p = *position;
//
//	for (size_t i = 0; i < textLen; ++i)
//	{
//		mgFont* font = tp->onFont(reason, tp, text[i]);
//		mgColor* color = tp->onColor(reason, tp, text[i], style);
//		tp->onDraw(reason, tp, text[i], &p, color, font);
//	}
//}

mgTextProcessor* 
MG_C_DECL 
mgCreateTextProcessor(struct mgVideoDriverAPI_s* gpu)
{
	assert(gpu);

	mgTextProcessor* tp = calloc(1, sizeof(mgTextProcessor));
	tp->gpu = gpu;
	return tp;
}

void 
MG_C_DECL 
mgDestroyTextProcessor(mgTextProcessor* tp)
{
	assert(tp);
	free(tp);
}
