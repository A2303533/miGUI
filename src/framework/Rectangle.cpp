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
#include "framework/Rectangle.h"

using namespace mgf;

extern Framework* g_mgf;

namespace mgf
{
	mgColor* Rectangle_onColor1(mgElement_s* e)
	{
		mgf::Rectangle* rectangle = static_cast<mgf::Rectangle*>(e->userData);
		return &rectangle->m_color1;
	}

	mgColor* Rectangle_onColor2(mgElement_s* e)
	{
		mgf::Rectangle* rectangle = static_cast<mgf::Rectangle*>(e->userData);
		return &rectangle->m_color2;
	}

	mgTexture* Rectangle_onTexture(mgElement_s* e)
	{
		mgf::Rectangle* rectangle = static_cast<mgf::Rectangle*>(e->userData);
		return rectangle->m_texture;
	}
}

Rectangle::Rectangle(Window* w)
	: 
	Element(w)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	m_element = mgCreateRectangle(w->m_window, &p, &p);
	m_element->userData = this;
	
	m_elementRectangle = (mgElementRectangle*)m_element->implementation;
	m_elementRectangle->onColor1 = Rectangle_onColor1;
	m_elementRectangle->onColor2 = Rectangle_onColor2;
	m_elementRectangle->onTexture = Rectangle_onTexture;

	m_color1 = g_mgf->GetColor(mgf::ColorName::Black);
	m_color2 = g_mgf->GetColor(mgf::ColorName::Black);

	Element::PostInit();
}

Rectangle::~Rectangle()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void Rectangle::SetColor(mgColor* c1, mgColor* c2)
{
	m_color1 = *c1;
	m_color2 = *c2;
}

void Rectangle::SetColor(int c1, int c2)
{
	mgColorSetAsIntegerARGB(&m_color1, c1);
	mgColorSetAsIntegerARGB(&m_color2, c2);
}

void Rectangle::SetColor(int c)
{
	mgColorSetAsIntegerARGB(&m_color1, c);
	mgColorSetAsIntegerARGB(&m_color2, c);
}

void Rectangle::SetTexture(mgTexture* t)
{
	m_texture = t;
}

