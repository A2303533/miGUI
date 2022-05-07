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

using namespace mgf;

Text::Text(Window* w, const wchar_t* t, Font* f)
	:
	m_elementText(0)
{
	assert(t);
	assert(f);

	mgPoint p;
	mgPointSet(&p, 0, 0);
	mgColor c;
	mgColorSet(&c, 1.f, 1.f, 1.f, 1.f);

	m_text.assign(t);

	m_element = mgCreateText(w->m_window, &p, m_text.data(), ((FontImpl*)f)->m_font);
	m_elementText = (mgElementText*)m_element->implementation;
	Element::PostInit();
}

Text::~Text()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void Text::SetText(const wchar_t* t)
{
	m_text.assign(t);
	m_elementText->text = m_text.data();
	m_elementText->textLen = (int)m_text.size();
}

void Text::SetFont(Font* f)
{
	m_elementText->font = ((FontImpl*)f)->m_font;
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


void Text::SetColor(mgColor* c)
{
	m_elementText->color = *c;
}

void Text::SetColor(int i)
{
	mgColorSetAsIntegerARGB(&m_elementText->color, i);
}

