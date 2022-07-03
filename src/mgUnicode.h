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

typedef uint32_t mgUnicodeChar;

#if defined(__cplusplus)
extern "C" {
#endif

/// <summary>
/// Create Unicode string using UTF-8 string.
/// No size check. Size of `out` must be at least size of `str`.
/// `str` must be 0 terminated string.
/// `lenOut` is length of new mgUnicodeChar string.
/// </summary>
void MG_C_DECL mgUnicodeFromChar(const char* str, mgUnicodeChar* out, size_t* lenOut);

/// <summary>
/// All fonts now here.
/// And functions for text.
/// </summary>
typedef struct mgTextProcessor_s
{
	struct mgFont_s* fonts;
	struct mgVideoDriverAPI_s* gpu;

	// callbacks.
	// You must set this callbacks.
	// reason is mgDrawTextReason_****
	struct mgFont_s* (*onFont)(int reason, struct mgTextProcessor_s*, mgUnicodeChar);
	struct mgColor_s* (*onColor)(int reason, struct mgTextProcessor_s*, mgUnicodeChar, struct mgStyle_s*);
	void (*onGetTextSize)(int reason, struct mgTextProcessor_s*, const mgUnicodeChar* text, mgPoint*);
	void (*onDraw)(int reason, struct mgTextProcessor_s*, mgUnicodeChar, mgPoint* position, struct mgColor_s*, struct mgFont_s*);

	// method
	void(*drawText)(int reason, struct mgTextProcessor_s*, mgPoint* position, const mgUnicodeChar* text, size_t textLen, struct mgStyle_s*);

} mgTextProcessor;

/// <summary>
/// Create mgTextProcessor using this function;
/// Use free() after using.
/// </summary>
mgTextProcessor* MG_C_DECL mgCreateTextProcessor(struct mgFont_s* fonts, struct mgVideoDriverAPI_s* gpu);

#if defined(__cplusplus)
}
#endif

#endif