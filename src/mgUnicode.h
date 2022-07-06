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

#pragma once
#ifndef _MG_UNICODE_H_
#define _MG_UNICODE_H_

#include <stdint.h>

#if defined(__cplusplus)
typedef char32_t mgUnicodeChar;
#else
typedef uint32_t mgUnicodeChar;
#endif

#if defined(__cplusplus)
extern "C" {
#endif

uint64_t MG_C_DECL mgDecToOctLL(uint64_t dec);
void MG_C_DECL mgDecToHEX(uint64_t dec, char* buf, int* sz);

/// <summary>
/// Create Unicode string using UTF-8 string.
/// No size check. Size of `out` must be at least size of `str`.
/// `str` must be 0 terminated string.
/// `lenOut` is length of new mgUnicodeChar string.
/// </summary>
	void MG_C_DECL mgUnicodeFromUTF8(const char* str, mgUnicodeChar* out, size_t* lenOut);
	void MG_C_DECL mgUnicodeFromUTF16(const wchar_t* str, mgUnicodeChar* out, size_t* lenOut);
	void MG_C_DECL mgUnicodeToUTF8(const mgUnicodeChar* str, char* out, size_t* lenOut);
	void MG_C_DECL mgUnicodeToUTF16(const mgUnicodeChar* str, wchar_t* out, size_t* lenOut);
	
	int MG_C_DECL mgUnicodeSnprintf(mgUnicodeChar* str, size_t sz, const mgUnicodeChar* fmt, ...);
	int MG_C_DECL mgUnicodeVSnprintf(mgUnicodeChar* str, size_t sz, const mgUnicodeChar* fmt, va_list);

	size_t MG_C_DECL mgUnicodeStrlen(const mgUnicodeChar* str);

// mode: 
//  0 - normal,  decimal
//  1 - hex 7fa
//  2 - hex 7FA
//  3 - octal 610
void MG_C_DECL mgUnicodeULLTOA(uint64_t val, mgUnicodeChar* buf, size_t bufSz, int mode);
void MG_C_DECL mgUnicodeLLTOA(int64_t val, mgUnicodeChar* buf, size_t bufSz, int mode);

struct mgUnicodeCharNode
{
	uint32_t m_utf8;
	uint32_t m_utf16;
};

struct mgUnicodeCharNode* MG_C_DECL mgGetUnicodeTable();

union mgUnicodeUC
{
	uint8_t bytes[4];
	uint16_t shorts[2];
	uint32_t integer;
};

/// <summary>
/// And functions for text.
/// </summary>
typedef struct mgTextProcessor_s
{
	struct mgVideoDriverAPI_s* gpu;

	// callbacks.
	// You must set this callbacks.
	// reason is mgDrawTextReason_****
	void (*onDrawText)(int reason, struct mgTextProcessor_s*, mgPoint* position, const mgUnicodeChar* text, size_t textLen, struct mgColor_s*);
	struct mgFont_s* (*onFont)(int reason, struct mgTextProcessor_s*, mgUnicodeChar);
	struct mgColor_s* (*onColor)(int reason, struct mgTextProcessor_s*, mgUnicodeChar, struct mgStyle_s*);
	void (*onGetTextSize)(int reason, struct mgTextProcessor_s*, const mgUnicodeChar* text, size_t textLen, mgPoint*);
	//void (*onDraw)(int reason, struct mgTextProcessor_s*, mgUnicodeChar, mgPoint* position, struct mgColor_s*, struct mgFont_s*);

} mgTextProcessor;

/// <summary>
/// Create mgTextProcessor using this function;
/// Use mgDestroyTextProcessor after using.
/// </summary>
mgTextProcessor* MG_C_DECL mgCreateTextProcessor(struct mgVideoDriverAPI_s* gpu);
void MG_C_DECL mgDestroyTextProcessor(mgTextProcessor*);

#if defined(__cplusplus)
}
#endif

#endif