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

#include "framework/mgf.h"
#include "framework/TextProcessor.h"
#include "framework/Backend.h"
#include "framework/Font.h"

using namespace mgf;

//extern Backend* g_backend;
void mgf::TextProcessor_onDrawText(
	int reason,
	mgElement* element,
	struct mgTextProcessor_s* tp,
	mgPoint* position,
	const mgUnicodeChar* text,
	size_t textLen,
	struct mgColor_s* c)
{
	TextProcessor* textProcessor = (TextProcessor*)tp->userData;
	
	textProcessor->OnDrawText(reason, (mgf::Element*)element->userData, position, text, textLen, c);
}
struct mgFont_s* TextProcessor_onFont(
	int reason,
	mgElement* element,
	struct mgTextProcessor_s* tp,
	mgUnicodeChar c)
{
	TextProcessor* textProcessor = (TextProcessor*)tp->userData;
	return textProcessor->OnFont(reason, (mgf::Element*)element->userData, c);
}
struct mgColor_s* TextProcessor_onColor(
	int reason,
	mgElement* element,
	struct mgTextProcessor_s* tp,
	mgUnicodeChar c)
{
	TextProcessor* textProcessor = (TextProcessor*)tp->userData;
	return textProcessor->OnColor(reason, (mgf::Element*)element->userData, c);
}
void TextProcessor_onGetTextSize(
	int reason,
	mgElement* element,
	struct mgTextProcessor_s* tp,
	const mgUnicodeChar* text,
	size_t textLen,
	mgPoint* p)
{
	TextProcessor* textProcessor = (TextProcessor*)tp->userData;
	textProcessor->OnGetTextSize(reason, (mgf::Element*)element->userData, text, textLen, p);
}

TextProcessor::TextProcessor(Backend* b)
	:
	m_backend(b)
{
	m_tp = mgCreateTextProcessor();
	m_tp->onDrawText = TextProcessor_onDrawText;
	m_tp->onFont = TextProcessor_onFont;
	m_tp->onColor = TextProcessor_onColor;
	m_tp->onGetTextSize = TextProcessor_onGetTextSize;
	m_tp->userData = this;
}

TextProcessor::~TextProcessor()
{
	if (m_tp)
		mgDestroyTextProcessor(m_tp);
}

mgFont_s* TextProcessor::OnFont(int reason, Element* e, mgUnicodeChar c)
{
	return m_backend->GetDefaultFont()->m_font;
}

mgColor* TextProcessor::OnColor(int reason, Element* e, mgUnicodeChar c)
{
	static mgColor defCol(0x0);
	return &defCol;
}

void TextProcessor::OnGetTextSize(int reason, Element* e, const mgUnicodeChar* text, size_t textLen, mgPoint* p)
{
	p->x = p->y = 0;
	for (size_t i = 0; i < textLen; ++i)
	{
		mgFont* fnt = OnFont(reason, e, text[i]);
		m_backend->GetTextSize(text, textLen, fnt, p);
	}
}

void TextProcessor::OnDrawText(int reason, 
	Element* e,
	mgPoint* position, 
	const mgUnicodeChar* text, 
	size_t textLen, 
	struct mgColor_s* c)
{
	mgPoint p = *position;
	for (size_t i = 0; i < textLen; ++i)
	{
		mgFont* fnt = OnFont(reason, e, text[i]);
		p.x += m_backend->DrawText(reason, &p, &text[i], 1, c ? c : OnColor(reason, e, text[i]), fnt);
	}
}