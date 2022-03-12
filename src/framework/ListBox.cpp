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
#include "framework/ListBox.h"

using namespace mgf;

extern Backend* g_backend;

ListBox::ListBox(Window* w, void* arr, uint32_t arrSz, uint32_t dataTypeSizeOf, Font* f)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	
	FontImpl* fi = (FontImpl*)g_backend->GetDefaultFont();
	m_element = mgCreateListBox(w->m_window, &p, &p, arr, arrSz, dataTypeSizeOf);
	m_elementList = (mgElementList*)m_element->implementation;
	this->SetFont(f);
	Element::PostInit();
}

ListBox::~ListBox()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void ListBox::SetMultiselect(bool v)
{
	m_elementList->multiselect = (int)v;
}

void ListBox::SetCurSel(uint32_t c)
{
	m_elementList->curSel = c;
}
uint32_t ListBox::GetCurSel()
{
	return m_elementList->curSel;
}

uint32_t ListBox::GetArraySize()
{
	return m_elementList->arraySize;
}

void ListBox::SetFont(Font* f)
{
	m_elementList->font = ((FontImpl*)f)->m_font;
}

void ListBox::SetItemHeight(uint32_t i)
{
	m_elementList->itemHeight = i;
}



