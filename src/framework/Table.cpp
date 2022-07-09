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
#include "framework/TextProcessor.h"

using namespace mgf;

extern Backend* g_backend;

int Table_onDrawRow(struct mgElement_s*, void* row, uint32_t col, mgUnicodeChar** text, size_t* textlen);
int Table_onIsRowSelected(struct mgElement_s*, void* row);
void Table_onRowClick(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t mouseButton);
int Table_onCellClick(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t colIndex, uint32_t mouseButton);
const mgUnicodeChar* Table_onCellTextInputActivate(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t colIndex);
mgUnicodeChar Table_onCellTextInputCharEnter(struct mgElement_s*, mgUnicodeChar c);
int Table_onCellTextInputEndEdit(struct mgElement_s*, int type, const mgUnicodeChar* textinputText, void* row, uint32_t rowIndex, uint32_t colIndex);
const mgUnicodeChar* Table_onColTitleText(struct mgElement_s*, size_t* textLen, uint32_t colIndex);
void Table_onColTitleClick(struct mgElement_s*, uint32_t colIndex, uint32_t mouseButton);
int Table_onGetUserElementNum(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t colIndex);
struct mgElement_s* Table_onGetUserElement(struct mgElement_s*, uint32_t index, void* row, uint32_t rowIndex, uint32_t colIndex);
void Table_onBeginGetUserElement(struct mgElement_s*);


Table::Table(Window* w, uint32_t colNum)
	:
	Element(w)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	
	FontImpl* fi = (FontImpl*)g_backend->GetDefaultFont();
#pragma message("!!!!! !!!! !!!! Maybe need to remove font and add SetTextProcessor: " __FILE__ __FUNCTION__ " LINE : ")

	this->SetTextProcessor(g_backend->GetTextProcessor());

	m_element = mgCreateTable(w->m_window, &p, &p, 0, 0, colNum, m_textProcessor->GetTextProcessor());

	m_element->userData = this;
	m_elementTable = (mgElementTable*)m_element->implementation;
	
	m_elementTable->onDrawRow = Table_onDrawRow;

	m_elementTable->onIsRowSelected = Table_onIsRowSelected;
	m_elementTable->onRowClick = Table_onRowClick;
	m_elementTable->onCellClick = Table_onCellClick;
	m_elementTable->onCellTextInputActivate = Table_onCellTextInputActivate;
	m_elementTable->onCellTextInputCharEnter = Table_onCellTextInputCharEnter;
	m_elementTable->onCellTextInputEndEdit = Table_onCellTextInputEndEdit;
	m_elementTable->onColTitleText = Table_onColTitleText;
	m_elementTable->onColTitleClick = Table_onColTitleClick;
	m_elementTable->onGetUserElementNum = Table_onGetUserElementNum;
	m_elementTable->onGetUserElement = Table_onGetUserElement;
	m_elementTable->onBeginGetUserElement = Table_onBeginGetUserElement;

	for (uint32_t i = 0; i < colNum; ++i)
	{
		m_colSizes.push_back(100);
	}
	m_elementTable->colsSizes = &m_colSizes.front();
	Element::PostInit();
}

Table::~Table()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void Table::SetTextProcessor(TextProcessor* tp)
{
	Element::SetTextProcessor(tp);
	if(m_elementTable)
		m_elementTable->textProcessor = tp->GetTextProcessor();
}

void Table::SetRect(int left, int top, int right, int bottom)
{
	Element::SetRect(left, top, right, bottom);
	if (m_elementTable->onColTitleText && m_elementTable->colTitleHeight)
	{
		m_element->transformWorld.clipArea.top += m_elementTable->colTitleHeight;
		m_element->transformWorld.buildArea.top += m_elementTable->colTitleHeight;
	}
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
	//m_elementTable->font = ((FontImpl*)f)->m_font;
#pragma message("!!!!! !!!! !!!! Maybe need to remove font and add SetTextProcessor: " __FILE__ __FUNCTION__ " LINE : ")
}

void Table::SetDrawItemBG(bool v)
{
	m_elementTable->drawItemBG = (int)v;
}

void Table::SetTextInputCharLimit(uint32_t i)
{
	mgElement* tie = m_elementTable->textInput;
	mgElementTextInput_s* ti = (mgElementTextInput_s*)tie->implementation;
	ti->charLimit = i;
}

int Table_onDrawRow(struct mgElement_s* e, void* row, uint32_t col, mgUnicodeChar** text, size_t* textlen)
{
	Table* tb = (Table*)e->userData;

	return tb->OnDrawRow(tb, row, col, text, textlen);
}

int Table_onIsRowSelected(struct mgElement_s* e, void* row)
{
	Table* tb = (Table*)e->userData;
	return tb->OnIsRowSelected(tb, row);
}

void Table_onRowClick(struct mgElement_s* e, void* row, uint32_t rowIndex, uint32_t mouseButton)
{
	Table* tb = (Table*)e->userData;
	tb->OnRowClick(tb, row, rowIndex, mouseButton, e->window->context->input);
}

int Table_onCellClick(struct mgElement_s* e, void* row, uint32_t rowIndex, uint32_t colIndex, uint32_t mouseButton)
{
	Table* tb = (Table*)e->userData;
	return tb->OnCellClick(tb, row, rowIndex, colIndex, mouseButton, e->window->context->input);
}

const mgUnicodeChar* Table_onCellTextInputActivate(struct mgElement_s* e, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	Table* tb = (Table*)e->userData;
	return tb->OnCellTextInputActivate(tb, row, rowIndex, colIndex);
}

mgUnicodeChar Table_onCellTextInputCharEnter(struct mgElement_s* e, mgUnicodeChar c)
{
	Table* tb = (Table*)e->userData;
	return tb->OnCellTextInputCharEnter(tb, c);
}

int Table_onCellTextInputEndEdit(struct mgElement_s* e, int type, const mgUnicodeChar* textinputText, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	Table* tb = (Table*)e->userData;
	return tb->OnCellTextInputEndEdit(tb, type, textinputText, row, rowIndex, colIndex);
}

uint32_t Table::GetRowHeight()
{
	return m_elementTable->rowHeight;
}

void Table::SetRowHeight(uint32_t i)
{
	if (!i)
		i = 15;
	m_elementTable->rowHeight = i;
}

const mgUnicodeChar* Table_onColTitleText(struct mgElement_s* e, size_t* textLen, uint32_t colIndex)
{
	Table* tb = (Table*)e->userData;
	return tb->OnColTitleText(tb, textLen, colIndex);
}

void Table_onColTitleClick(struct mgElement_s* e, uint32_t colIndex, uint32_t mouseButton)
{
	Table* tb = (Table*)e->userData;
	tb->OnColTitleClick(tb, colIndex, mouseButton);
}

void Table::SetActiveColTitle(uint32_t i)
{
	if (i >= m_elementTable->numCols)
		i = m_elementTable->numCols - 1;
	m_elementTable->activeColTitle = i;
}

void Table::SetColTitleHeight(uint32_t h)
{
	m_elementTable->colTitleHeight = h;
}

int Table_onGetUserElementNum(struct mgElement_s* e, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	Table* tb = (Table*)e->userData;
	return tb->OnGetUserElementNum(tb, row, rowIndex, colIndex);
}

struct mgElement_s* Table_onGetUserElement(struct mgElement_s* e, uint32_t index, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	Table* tb = (Table*)e->userData;
	Element* mgfElement = tb->OnGetUserElement(tb, index, row, rowIndex, colIndex);
	if(mgfElement)
		return mgfElement->GetElement();
	return 0;
}

void Table_onBeginGetUserElement(struct mgElement_s* e)
{
	Table* tb = (Table*)e->userData;
	tb->OnBeginGetUserElement(tb);
}

int* Table::GetColSizes()
{
	return m_elementTable->colsSizes;
}

int Table::OnDrawRow(Table*, void* row, uint32_t col, mgUnicodeChar** text, size_t* textlen)
{
	return 1;
}

int Table::OnIsRowSelected(Table*, void* row)
{
	return 0;
}

void Table::OnRowClick(Table*, void* row, uint32_t rowIndex, uint32_t mouseButton, mgInputContext_s* input)
{
}

int Table::OnCellClick(Table*, void* row, uint32_t rowIndex, uint32_t colIndex, uint32_t mouseButton, mgInputContext_s* input)
{
	return 0;
}

const mgUnicodeChar* Table::OnCellTextInputActivate(Table*, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	return 0;
}

mgUnicodeChar Table::OnCellTextInputCharEnter(Table*, mgUnicodeChar c)
{
	return c;
}

int Table::OnCellTextInputEndEdit(Table*, int type, const mgUnicodeChar* textinputText, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	return 1;
}

const mgUnicodeChar* Table::OnColTitleText(Table*, size_t* textLen, uint32_t colIndex)
{
	return 0;
}

void Table::OnColTitleClick(Table*, uint32_t colIndex, uint32_t mouseButton)
{
}

int Table::OnGetUserElementNum(Table*, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	return 0;
}

Element* Table::OnGetUserElement(Table*, uint32_t index, void* row, uint32_t rowIndex, uint32_t colIndex)
{
	return 0;
}

void Table::OnBeginGetUserElement(Table*)
{
}