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
#include "framework/Table.h"

using namespace mgf;

extern Backend* g_backend;

wchar_t Table_onTextInputCharEnter(struct mgElement_s* e, wchar_t c);
int Table_onEndEdit(struct mgElement_s*, int type, const wchar_t* str, uint8_t* editItem);
int Table_onDrawRow(struct mgElement_s*, void* row, uint32_t col, wchar_t** text, uint32_t* textlen);
int Table_onIsRowSelected(struct mgElement_s*, void* row);
void Table_onRowClick(struct mgElement_s*, void* row, uint32_t rowIndex, int mouseButton);
int Table_onCellClick(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t cellIndex, int mouseButton);

Table::Table(Window* w, uint32_t colNum, Font* f)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	
	FontImpl* fi = (FontImpl*)g_backend->GetDefaultFont();
	m_element = mgCreateTable(w->m_window, &p, &p, 0, 0, colNum, ((FontImpl*)f)->m_font);
	m_element->userData = this;
	m_elementTable = (mgElementTable*)m_element->implementation;
	m_elementTable->onDrawRow = Table_onDrawRow;
	m_elementTable->onIsRowSelected = Table_onIsRowSelected;
	m_elementTable->onRowClick = Table_onRowClick;
	m_elementTable->onCellClick = Table_onCellClick;

	for (uint32_t i = 0; i < colNum; ++i)
	{
		m_colSizes.push_back(100);
	}
	m_elementTable->colsSizes = m_colSizes.data();

	/*m_elementTable->onTextInputCharEnter = LB_onTextInputCharEnter;
	m_elementTable->onTextInputEndEdit = LB_onEndEdit;
	m_elementTable->onSelect = LB_onSelect;*/
	Element::PostInit();
}

Table::~Table()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void Table::SetCurSel(uint32_t c)
{
	/*m_elementTable->curSel = c;*/
}
uint32_t Table::GetCurSel()
{
	/*return m_elementTable->curSel;*/
	return 0;
}

void Table::SetScrollSpeed(float s)
{
	m_elementTable->scrollSpeed = s;
}

void** Table::GetData()
{
	return m_elementTable->rows;
}

void Table::SetData(void** arr, uint32_t arrSz)
{
	m_elementTable->rows = arr;
	m_elementTable->numRows = arrSz;
}

void Table::SetFont(Font* f)
{
	m_elementTable->font = ((FontImpl*)f)->m_font;
}

wchar_t Table_onTextInputCharEnter(struct mgElement_s* e, wchar_t c)
{
	/*ListBox* lb = (ListBox*)e->userData;
	if (lb->onTextInputCharEnter)
		return lb->onTextInputCharEnter(lb, c);*/

	return c;
}

int Table_onEndEdit(struct mgElement_s* e, int type, const wchar_t* str, uint8_t* editItem)
{
	/*ListBox* lb = (ListBox*)e->userData;

	if (lb->onTextInputEndEdit)
		return lb->onTextInputEndEdit(lb, type, str, editItem);*/

	return type;
}

void Table::SetSelectWithRMB(bool v)
{
//	m_elementTable->selectWithRMB = (int)v;
}

void Table::SetDrawItemBG(bool v)
{
//	m_elementTable->drawItemBG = (int)v;
}

void Table::SetTextInputCharLimit(uint32_t i)
{
	mgElement* tie = m_elementTable->textInput;
	mgElementTextInput_s* ti = (mgElementTextInput_s*)tie->implementation;
	ti->charLimit = i;
}

int Table_onDrawRow(struct mgElement_s* e, void* row, uint32_t col, wchar_t** text, uint32_t* textlen)
{
	Table* tb = (Table*)e->userData;

	if (tb->onDrawRow)
		return tb->onDrawRow(tb, row, col, text, textlen);

	return 0;
}

int Table_onIsRowSelected(struct mgElement_s* e, void* row)
{
	Table* tb = (Table*)e->userData;

	if (tb->onIsRowSelected)
		return tb->onIsRowSelected(tb, row);

	return 0;
}

void Table_onRowClick(struct mgElement_s* e, void* row, uint32_t rowIndex, int mouseButton)
{
	Table* tb = (Table*)e->userData;

	if (tb->onRowClick)
		tb->onRowClick(tb, row, rowIndex, mouseButton, e->window->context->input);
}

int Table_onCellClick(struct mgElement_s* e, void* row, uint32_t rowIndex, uint32_t cellIndex, int mouseButton)
{
	Table* tb = (Table*)e->userData;

	if (tb->onCellClick)
		return tb->onCellClick(tb, row, rowIndex, cellIndex, mouseButton, e->window->context->input);
	return 0;
}
