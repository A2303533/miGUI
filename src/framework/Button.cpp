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
#include "framework/Button.h"
#include "framework/Icons.h"

using namespace mgf;

extern Backend* g_backend;

Button::Button(Window* w)
	:
	m_elementButton(0),
	m_icons(0)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	
	FontImpl* fi = (FontImpl*)g_backend->GetDefaultFont();
	m_text.assign(L" ");
	m_element = mgCreateButton(w->m_window, &p, &p, m_text.data(), fi->m_font );
	m_elementButton = (mgElementButton*)m_element->implementation;
	Element::PostInit();
}

Button::~Button()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void Button::SetText(const wchar_t* t)
{
	m_text.clear();
	m_elementButton->text = 0;
	int slen = 0;
	if (t)
		slen = (int)wcslen(t);

	if (slen)
	{
		m_text.assign(t);
		m_elementButton->text = m_text.data();
		m_elementButton->textLen = slen;
	}
}

void Button::SetAsPush(bool v)
{
	m_elementButton->pushButton = (int)v;
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

void Button::SetFont(Font* f)
{
	FontImpl* fi = (FontImpl*)f;
	m_elementButton->font = fi->m_font;
}



