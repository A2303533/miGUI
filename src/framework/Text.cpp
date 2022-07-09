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
#include "framework/Window.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"
#include "framework/Text.h"
#include "framework/TextProcessor.h"

#include <stdarg.h>
#include <wchar.h>

using namespace mgf;
extern Backend* g_backend;

namespace mgf
{
	const mgUnicodeChar* Text_onText(struct mgElement_s* e, size_t* textLen)
	{
		mgf::Text* text = static_cast<mgf::Text*>(e->userData);
		*textLen = text->m_text.Size();
		return text->m_text.Data();
	}
}

Text::Text(Window* w, const mgUnicodeChar* t)
	:
	Element(w)
{
	assert(t);

	mgPoint p;
	mgPointSet(&p, 0, 0);

	m_text.Assign(t, mgUnicodeStrlen(t));

	this->SetTextProcessor(g_backend->GetTextProcessor());

	m_element = mgCreateText(w->m_window, &p, m_textProcessor->GetTextProcessor());

	m_elementText = (mgElementText*)m_element->implementation;
	//m_elementText->onColor = Text_onColor;
	//m_elementText->onFont = Text_onFont;
	m_elementText->onText = Text_onText;

	Element::PostInit();
}

Text::~Text()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void Text::SetTextProcessor(TextProcessor* tp)
{
	Element::SetTextProcessor(tp);
	if(m_elementText)
		m_elementText->textProcessor = tp->GetTextProcessor();
}

void Text::SetText(const wchar_t* t)
{
	m_text.Assign(t, wcslen(t));
	m_element->onRebuild(m_element);
}

void Text::SetText(const char* t)
{
	m_text.Assign(t, strlen(t));
	m_element->onRebuild(m_element);
}

void Text::SetText(const char8_t* t)
{
	m_text.Clear();
	size_t slen = 0;
	if (t)
	{
		slen = strlen((const char*)t);
		if (slen)
			m_text.Assign(t, slen);
	}
	m_element->onRebuild(m_element);
}

void Text::SetText(const char16_t* t)
{
	m_text.Clear();
	size_t slen = 0;
	if (t)
	{
		slen = wcslen((const wchar_t*)t);
		if (slen)
			m_text.Assign(t, slen);
	}
	m_element->onRebuild(m_element);
}

void Text::SetText(const char32_t* t)
{
	m_text.Clear();
	size_t slen = 0;
	if (t)
	{
		slen = mgUnicodeStrlen(t);
		if (slen)
			m_text.Assign(t, slen);
	}
	m_element->onRebuild(m_element);
}

void Text::SetText(const UnicodeString& str)
{
	m_text.Assign(str);
	m_element->onRebuild(m_element);
}


void Text::SetTextF(const wchar_t* f, ...)
{
	va_list ap;
	va_start(ap, f);
	wchar_t buffer[500];
	buffer[0] = 0;
	vswprintf(buffer, 500, f, ap);
	va_end(ap);
	SetText(buffer);
}

void Text::SetTextF(const char* f, ...)
{
	va_list ap;
	va_start(ap, f);
	char buffer[500];
	buffer[0] = 0;
	vsprintf(buffer, f, ap);
	va_end(ap);
	SetText(buffer);
}

void Text::SetTextF(const char8_t* f, ...)
{
	va_list ap;
	va_start(ap, f);
	char buffer[500];
	buffer[0] = 0;
	vsprintf(buffer, (const char*)f, ap);
	va_end(ap);
	SetText(buffer);
}

void Text::SetTextF(const char16_t* f, ...)
{
	va_list ap;
	va_start(ap, f);
	wchar_t buffer[500];
	buffer[0] = 0;
	vswprintf(buffer, 500, (const wchar_t*)f, ap);
	va_end(ap);
	SetText(buffer);
}
void Text::SetTextF(const char32_t* f, ...)
{
	va_list ap;
	va_start(ap, f);
	mgUnicodeChar buffer[500];
	buffer[0] = 0;
	mgUnicodeVSnprintf(buffer, 500, f, ap);
	va_end(ap);
	SetText(buffer);
}

void Text::SetPosition(mgPoint* p)
{
	m_element->transformLocal.buildArea.left = p->x;
	m_element->transformLocal.buildArea.top = p->y;
	m_element->transformLocal.buildArea.right = p->x;
	m_element->transformLocal.buildArea.bottom = p->y;
	m_element->transformLocal.clipArea.left = p->x;
	m_element->transformLocal.clipArea.top = p->y;
	m_element->transformLocal.clipArea.right = p->x;
	m_element->transformLocal.clipArea.bottom = p->y;
	m_element->creationRect.left = p->x;
	m_element->creationRect.top = p->y;
	m_element->creationRect.right = p->x;
	m_element->creationRect.bottom = p->y;
	m_element->transformWorld = m_element->transformLocal;
}

void Text::SetPosition(int x, int y)
{
	mgPoint p;
	p.x = x;
	p.y = y;
	SetPosition(&p);
}

uint32_t Text::GetWidth()
{
	uint32_t v = 0;
	if (m_text.Size() /*&& m_font*/)
	{
		mgPoint p;
		//m_window->m_gui_context->getTextSize(m_text.c_str(), m_font, &p);
		m_textProcessor->OnGetTextSize(0, m_text.Data(), m_text.Size(), &p);

		v = p.x;
	}
	return v;
}
