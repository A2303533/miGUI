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
#include "framework/TextProcessor.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"
#include "framework/Button.h"
#include "framework/Icons.h"

using namespace mgf;

extern Backend* g_backend;

namespace mgf
{
	/*mgFont* Button_onFont(struct mgElement_s* e)
	{
		mgf::Button* btn = static_cast<mgf::Button*>(e->userData);
		return btn->m_font;
	}*/

	const mgUnicodeChar* Button_onText(struct mgElement_s* e, size_t* textLen)
	{
		mgf::Button* btn = static_cast<mgf::Button*>(e->userData);
		*textLen = btn->m_text.Size();
		return btn->m_text.Data();
	}
}

Button::Button(Window* w)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	
	FontImpl* fi = (FontImpl*)g_backend->GetDefaultFont();
	m_text.Assign(L" ", 1);
	m_font = fi->m_font;

	this->SetTextProcessor(g_backend->GetTextProcessor());

	m_element = mgCreateButton(w->m_window, &p, &p, m_textProcessor->GetTextProcessor());
	m_element->userData = this;
	m_elementButton = (mgElementButton*)m_element->implementation;
	//m_elementButton->onFont = Button_onFont;
	m_elementButton->onText = Button_onText;
	Element::PostInit();
}

Button::~Button()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void Button::SetTextProcessor(TextProcessor* tp)
{
	Element::SetTextProcessor(tp);
	if(m_elementButton)
		m_elementButton->textProcessor = tp->GetTextProcessor();
}

void Button::SetText(const char* t)
{
	m_text.Clear();
	size_t slen = 0;
	if (t)
	{
		slen = strlen(t);
		if (slen)
			m_text.Assign(t, slen);
	}
	m_element->onRebuild(m_element);
}

void Button::SetText(const char8_t* t)
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

void Button::SetText(const char16_t* t)
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

void Button::SetText(const char32_t* t)
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

void Button::SetText(const UnicodeString& str)
{
	m_text.Assign(str);
	m_element->onRebuild(m_element);
}


void Button::SetText(const wchar_t* t)
{
	m_text.Clear();
	size_t slen = 0;
	if (t)
	{
		slen = wcslen(t);
		if (slen)
			m_text.Assign(t, slen);
	}
	m_element->onRebuild(m_element);
}

void Button::SetAsPush(bool v)
{
	m_elementButton->pushButton = (int)v;
}

void Button::SetTexture(mgTexture* t)
{
	m_elementButton->BGtexture = t;
}

void Button::SetIcons(
	Icons* icons,
	int id1,
	int id2,
	int id3,
	int id4
)
{
	m_icons = icons;
	m_elementButton->icons = icons->m_icons;
	m_elementButton->iconID_default = id1;
	m_elementButton->iconID_hover = id2;
	m_elementButton->iconID_push = id3;
	m_elementButton->iconID_disable = id4;
}

void Button::OnPushOn() {}
void Button::OnPushOff() {}

