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

wchar_t LB_onTextInputCharEnter(struct mgElement_s* e, wchar_t c);
int LB_onEndEdit(struct mgElement_s*, int type, const wchar_t* str, void* editItem);
int LB_onIsItemSelected(struct mgElement_s* e, void* item);
void LB_onItemClick(struct mgElement_s* e, void* item, uint32_t itemIndex, uint32_t mouseButton);
int LB_onDrawItem(struct mgElement_s*, void* item, uint32_t itemIndex, wchar_t** text, uint32_t* textlen);

ListBox::ListBox(Window* w, Font* f)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	
	FontImpl* fi = (FontImpl*)g_backend->GetDefaultFont();
	m_element = mgCreateListBox(w->m_window, &p, &p, 0, 0, ((FontImpl*)f)->m_font);
	m_element->userData = this;
	m_elementList = (mgElementList*)m_element->implementation;
	m_elementList->onTextInputCharEnter = LB_onTextInputCharEnter;
	m_elementList->onTextInputEndEdit = LB_onEndEdit;
	m_elementList->onIsItemSelected = LB_onIsItemSelected;
	m_elementList->onItemClick = LB_onItemClick;
	m_elementList->onDrawItem = LB_onDrawItem;
	Element::PostInit();
}

ListBox::~ListBox()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void ListBox::SetData(void** arr, uint32_t arrSz)
{
	m_elementList->items = arr;
	m_elementList->itemsSize = arrSz;
}

void** ListBox::GetData()
{
	return m_elementList->items;
}

void ListBox::SetFont(Font* f)
{
	m_elementList->font = ((FontImpl*)f)->m_font;
}

void ListBox::SetItemHeight(uint32_t i)
{
	m_elementList->itemHeight = i;
}

void ListBox::CanEdit(bool v)
{
	m_elementList->editText = (int)v;
}

wchar_t LB_onTextInputCharEnter(struct mgElement_s* e, wchar_t c)
{
	ListBox* lb = (ListBox*)e->userData;
	if (lb->onTextInputCharEnter)
		return lb->onTextInputCharEnter(lb, c);

	return c;
}

int LB_onEndEdit(struct mgElement_s* e, int type, const wchar_t* str, void* editItem)
{
	ListBox* lb = (ListBox*)e->userData;

	if (lb->onTextInputEndEdit)
		return lb->onTextInputEndEdit(lb, type, str, editItem);

	return type;
}

void ListBox::SetDrawItemBG(bool v)
{
	m_elementList->drawItemBG = (int)v;
}

void ListBox::SetTextInputCharLimit(uint32_t i)
{
	mgElement* tie = m_elementList->textInput;
	mgElementTextInput_s* ti = (mgElementTextInput_s*)tie->implementation;
	ti->charLimit = i;
}

int LB_onIsItemSelected(struct mgElement_s* e, void* item)
{
	ListBox* lb = (ListBox*)e->userData;
	if (lb->onIsItemSelected)
		return lb->onIsItemSelected(lb, item);
	return 0;
}

void LB_onItemClick(struct mgElement_s* e, void* item, uint32_t itemIndex, uint32_t mouseButton)
{
	ListBox* lb = (ListBox*)e->userData;
	if (lb->onItemClick)
		return lb->onItemClick(lb, item, itemIndex, mouseButton);
}

int LB_onDrawItem(struct mgElement_s* e, void* item, uint32_t itemIndex, wchar_t** text, uint32_t* textlen)
{
	ListBox* lb = (ListBox*)e->userData;
	if (lb->onDrawItem)
		return lb->onDrawItem(lb, item, itemIndex, text, textlen);
	return 0;
}
