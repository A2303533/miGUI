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

void 
MG_C_DECL 
mgUnicodeFromChar(const char* str, mgUnicodeChar* out, size_t* lenOut)
{
	assert(str);
	assert(out);
	assert(lenOut);

}

void
mgTextProcessor_drawText(
	int reason, 
	struct mgTextProcessor_s* tp, 
	mgPoint* position, 
	const mgUnicodeChar* text, 
	size_t textLen, 
	struct mgStyle_s* style)
{
	mgPoint p = *position;

	for (size_t i = 0; i < textLen; ++i)
	{
		mgFont* font = tp->onFont(reason, tp, text[i]);
		mgColor* color = tp->onColor(reason, tp, text[i], style);
		tp->onDraw(reason, tp, text[i], &p, color, font);
	}
}

mgTextProcessor* 
MG_C_DECL 
mgCreateTextProcessor(mgFont* fonts, struct mgVideoDriverAPI_s* gpu)
{
	assert(fonts);
	assert(gpu);

	mgTextProcessor* tp = calloc(1, sizeof(mgTextProcessor));
	tp->fonts = fonts;
	tp->gpu = gpu;
	tp->drawText = mgTextProcessor_drawText;
	return tp;
}
