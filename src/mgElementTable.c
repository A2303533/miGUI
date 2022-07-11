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
#include "mgFunctions.h"

#include <assert.h>
#include <stdlib.h>
#include <wchar.h>

void miGUI_onUpdateTransform_rectangle(mgElement* e);
void miGUI_onUpdate_rectangle(mgElement* e);
void miGUI_onUpdateTransform_textinput(mgElement* e);
float lerp(float v0, float v1, float t);

struct lbData1
{
	const mgUnicodeChar* text;
};
struct lbData2
{
	const mgUnicodeChar* text;
	uint32_t flags;
};

int mgElementTable_textinput_onEndEdit(struct mgElement_s* e, int type)
{
	e->visible = 0;
	e->window->context->textInputActive = 0;
	
	mgSetCursor(e->window->context, e->window->context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);

	mgElementTextInput* ti = e->implementation;
	mgElement* table = (mgElement*)e->userData;
	mgElementTable* tableImpl = (mgElementTable*)table->implementation;

	if (tableImpl->onCellTextInputEndEdit)
		return tableImpl->onCellTextInputEndEdit(table, type, ti->text, 
			tableImpl->editRow,
			tableImpl->editRowIndex,
			tableImpl->editColIndex
		);

	return 1;
}

mgUnicodeChar mgElementTable_textinput_onCharEnter(struct mgElement_s* e, mgUnicodeChar c)
{
	mgElementTextInput* ti = e->implementation;
	mgElement* table = (mgElement*)e->userData;
	mgElementTable* tableImpl = (mgElementTable*)table->implementation;
	
	if (tableImpl->onCellTextInputCharEnter)
		return tableImpl->onCellTextInputCharEnter(table, c);

	return c;
}
void mgElementTable_textinput_onActivate(struct mgElement_s* e)
{
	mgElement* table = (mgElement*)e->userData;
	mgElementTable* tableImpl = (mgElementTable*)table->implementation;

	mgElementTextInput* ti = e->implementation;
	if (tableImpl->onCellTextInputActivate)
		mgTextInputSetText(ti, tableImpl->onCellTextInputActivate(table, tableImpl->hoverRow, tableImpl->hoverRowIndex, tableImpl->hoverColIndex));


	ti->isSelected = 1;
	ti->selectionStart = 0;
	ti->selectionEnd = ti->textLen;
	ti->textCursor = ti->textLen;
}
int mgElementTable_textinput_onPaste(struct mgElement_s* e, mgUnicodeChar* str, size_t sz)
{
	return 1;
}

void
miGUI_onUpdateTransform_table(mgElement* e)
{
	miGUI_onUpdateTransform_rectangle(e);

	mgElementTable* impl = (mgElementTable*)e->implementation;

	e->transformWorld.buildArea.top += impl->colTitleHeight;
	e->transformWorld.clipArea.top += impl->colTitleHeight;

	impl->numOfLines = 0;
	int w = e->transformWorld.buildArea.bottom - e->transformWorld.buildArea.top;
	if (w > 0)
	{
		if (impl->rowHeight < 2)
			impl->rowHeight = 2;

		impl->numOfLines = (w / impl->rowHeight) + 4;
	}

	e->contentHeight = impl->numRows * impl->rowHeight;
}

void 
miGUI_onUpdate_table(mgElement* e)
{
	miGUI_onUpdate_rectangle(e);

	mgContext* c = e->window->context;
	mgElementTable* impl = (mgElementTable*)e->implementation;

	if (e->contentHeight > e->clientHeight)
	{
		if (mgPointInRect(&e->transformWorld.clipArea, &c->input->mousePosition))
		{
			if (c->input->mouseWheelDelta < 0.f)
			{
				impl->rowScrollValueTarget += impl->scrollSpeed;
				if (impl->rowScrollValueTarget > (e->contentHeight - e->clientHeight))
					impl->rowScrollValueTarget = (float)(e->contentHeight - e->clientHeight);
			}
			else if (c->input->mouseWheelDelta > 0.f)
			{
				impl->rowScrollValueTarget -= impl->scrollSpeed;

				if (impl->rowScrollValueTarget < 0.f)
					impl->rowScrollValueTarget = 0.f;
			}
		}

		impl->rowScrollValue = lerp(impl->rowScrollValue, impl->rowScrollValueTarget, 0.1f);
		impl->rowScrollValueWorld = impl->rowScrollValue;/*parent must be somewhere*/
	}

	if (impl->numRows && (impl->numRows > impl->numOfLines))
	{
		mgPoint pos;
		pos.x = e->transformWorld.buildArea.left;
		pos.y = e->transformWorld.buildArea.top;
		
		pos.y = pos.y + (impl->rowHeight * impl->firstRowIndexForDraw) - (int)impl->rowScrollValueWorld;
		if (!pos.y)
			pos.y = 1;

		int szY = e->transformWorld.buildArea.top - (int)impl->rowHeight;/*NEED TO MAKE SAME CHANGES FOR LISTBOX*/
		int nm = 1;/*NEED TO MAKE SAME CHANGES FOR LISTBOX*/
		if (pos.y < szY)
		{
			//if (szY > (int)impl->rowHeight) /*NEED TO MAKE SAME CHANGES FOR LISTBOX*/
			//	nm = pos.y / szY;/*NEED TO MAKE SAME CHANGES FOR LISTBOX*/
			nm = (szY - pos.y) / impl->rowHeight;

			impl->firstRowIndexForDraw += nm;/*NEED TO MAKE SAME CHANGES FOR LISTBOX*/

			uint32_t lastIndex = impl->numRows - impl->numOfLines;
			if (impl->firstRowIndexForDraw > lastIndex)
				impl->firstRowIndexForDraw = lastIndex;
		}
		
		if (pos.y > szY)
		{
			/*it's still not good but better than nothing*/
			//if (pos.y > (int)impl->rowHeight)/*NEED TO MAKE SAME CHANGES FOR LISTBOX*/
			//	nm = pos.y / szY;/*NEED TO MAKE SAME CHANGES FOR LISTBOX*/
			
			nm = (pos.y - szY) / impl->rowHeight; /*now better...maybe*/

			if (impl->firstRowIndexForDraw)
				impl->firstRowIndexForDraw -= nm;/*NEED TO MAKE SAME CHANGES FOR LISTBOX*/
		}
	}

	

	if (impl->hoverRow)
	{
		int mouseBtn = 0;
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			mouseBtn = 1;
		else if (c->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
			mouseBtn = 2;
		else if (c->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN)
			mouseBtn = 3;

		if (impl->onRowClick && mouseBtn)
			impl->onRowClick(e, impl->hoverRow, impl->hoverRowIndex, mouseBtn);
		
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			impl->clickedRow = impl->hoverRow;

		if (impl->onCellClick && mouseBtn)
		{
			if (impl->onCellClick(e, impl->hoverRow, impl->hoverRowIndex, impl->hoverColIndex, mouseBtn))
			{
				mgElementTextInput* ti = (mgElementTextInput*)impl->textInput->implementation;
				impl->textInput->visible = 1;
				mgTextInputActivate(c, ti, 1, 0);
				mgRect r;
				r.left = impl->hoverCellClipRect.left - e->transformWorld.buildArea.left;
				r.right = r.left + (impl->hoverCellClipRect.right - impl->hoverCellClipRect.left);

				r.top = impl->hoverCellClipRect.top - e->transformWorld.buildArea.top;
				r.bottom = r.top + impl->rowHeight;

				r.top += (int)e->scrollValueWorld;
				r.bottom += (int)e->scrollValueWorld;
				impl->textInput->transformLocal.buildArea = r;
				impl->textInput->transformLocal.clipArea = r;
				impl->editRow = impl->hoverRow;
				impl->editRowIndex = impl->hoverRowIndex;
				impl->editColIndex = impl->hoverColIndex;

				miGUI_onUpdateTransform_textinput(impl->textInput);
			}

		}
	}

	if (impl->onColTitleClick && (impl->hoverColTitle > -1))
	{
		int mouseBtn = 0;
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			mouseBtn = 1;
		else if (c->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
			mouseBtn = 2;
		else if (c->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN)
			mouseBtn = 3;

		if (mouseBtn)
			impl->onColTitleClick(e, impl->hoverColTitle, mouseBtn);
	}

	static int cursorXOnLMB = 0;
	static int saveColSize = 0;
	static int cursorInColSplitterOld = 0;
	if (impl->cursorInColSplitter || impl->colSplitterMode)
	{
		mgSetCursor(c, c->defaultCursors[mgCursorType_SizeWE], mgCursorType_Arrow);
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
		{
			impl->colSplitterMode = 1;
			cursorXOnLMB = c->input->mousePosition.x;
			saveColSize = impl->colsSizes[impl->colIndexSplitterMode];
		}
	}

	if (impl->colSplitterMode)
	{
		if (c->input->mouseMoveDelta.x)
		{
			//impl->colsSizes[impl->colIndexSplitterMode] += c->input->mouseMoveDelta.x;
			int px = c->input->mousePosition.x - cursorXOnLMB;
			impl->colsSizes[impl->colIndexSplitterMode] = px + saveColSize;

			if (impl->colsSizes[impl->colIndexSplitterMode] < 10)
				impl->colsSizes[impl->colIndexSplitterMode] = 10;
			if (impl->colsSizes[impl->colIndexSplitterMode] > 500)
				impl->colsSizes[impl->colIndexSplitterMode] = 500;
		}

		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			impl->colSplitterMode = 0;
			mgSetCursor(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		}
	}

	if (cursorInColSplitterOld && !impl->cursorInColSplitter && !impl->colSplitterMode)
		mgSetCursor(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
	cursorInColSplitterOld = impl->cursorInColSplitter;
}

void 
miGUI_onDraw_table(mgElement* e)
{
	mgStyle* style = e->userStyle;
	if (!style)
		style = e->window->context->activeStyle;

	mgContext* ctx = e->window->context;

	mgElementTable* impl = (mgElementTable*)e->implementation;

	if (impl->onColTitleText && impl->colTitleHeight)
	{
		mgRect rect = e->transformWorld.clipArea;
		rect.top -= impl->colTitleHeight;
		e->window->context->gpu->setClipRect(&rect);
		e->window->context->gpu->drawRectangle(mgDrawRectangleReason_tableColTitleBG, 
			impl,
			&rect,
			&style->tableColTitleBG,
			&style->tableColTitleBG, 0, 0);

		impl->hoverColTitle = -1;
		impl->cursorInColSplitter = 0;
		const mgUnicodeChar* str = 0;
		uint32_t strLen = 0;
		mgPoint pos;
		pos.x = rect.left;
		pos.y = rect.top;
		for (uint32_t i2 = 0; i2 < impl->numCols; ++i2)
		{
			str = 0;
			strLen = 0;

			str = impl->onColTitleText(e, &strLen, i2);
			if (str && strLen)
			{
				pos.x += 2;
				mgRect rctCol;
				rctCol.left = pos.x-2;
				rctCol.top = pos.y;
				rctCol.right = rctCol.left + impl->colsSizes[i2]+2;
				rctCol.bottom = rctCol.top + impl->colTitleHeight;

				e->window->context->gpu->setClipRect(&rctCol);
				e->window->context->gpu->drawRectangle(mgDrawRectangleReason_tableColTitleColBG,
					impl,
					&rctCol,
					&style->tableColTitleColBG,
					&style->tableColTitleColBG, 0, 0);

				if (mgPointInRect(&rctCol, &ctx->input->mousePosition))
				{
					impl->hoverColTitle = (int)i2;
					e->window->context->gpu->drawRectangle(mgDrawRectangleReason_tableColTitleColHover,
						impl,
						&rctCol,
						&style->tableColTitleColHover,
						&style->tableColTitleColHover, 0, 0);
				}
				mgRect rctColSplt;
				rctColSplt.left = rctCol.right - 2;
				rctColSplt.right = rctCol.right;
				rctColSplt.top = rctCol.top;
				rctColSplt.bottom = rctCol.bottom;
				if (mgPointInRect(&rctColSplt, &ctx->input->mousePosition) && !impl->colSplitterMode)
				{
					impl->cursorInColSplitter = 1;
					impl->colIndexSplitterMode = i2;
				}

				if (impl->activeColTitle == i2)
				{
					e->window->context->gpu->drawRectangle(mgDrawRectangleReason_tableColTitleColActive,
						impl,
						&rctCol,
						&style->tableColTitleColActive,
						&style->tableColTitleColActive, 0, 0);
				}
				
				/*e->window->context->gpu->drawText(mgDrawTextReason_tableColTitle,
					impl,
					&pos,
					str,
					strLen,
					&style->tableCellText,
					impl->font);*/
				impl->textProcessor->onDrawText(
					mgDrawTextReason_tableColTitle,
					e,
					impl->textProcessor,
					&pos,
					str,
					(size_t)strLen,
					&style->tableCellText);
			}

			pos.x += impl->colsSizes[i2];
		}
	}

	e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
	if (e->drawBG)
	{
		e->window->context->gpu->drawRectangle(mgDrawRectangleReason_tableBG, impl,
			&e->transformWorld.buildArea,
			&style->tableBG,
			&style->tableBG, 0, 0);
	}


	if (impl->rows)
	{
		uint32_t index = impl->firstRowIndexForDraw;

		mgPoint pos;
		pos.x = e->transformWorld.buildArea.left;
		pos.y = e->transformWorld.buildArea.top - (int)impl->rowScrollValueWorld;
		pos.y = pos.y + (impl->rowHeight * index);

		if(impl->onBeginGetUserElement)
			impl->onBeginGetUserElement(e);

		impl->hoverRow = 0;
		impl->hoverRowIndex = 0;
		impl->hoverColIndex = 0xFFFFFFFF;
		for (uint32_t i = 0; i < impl->numOfLines; ++i)
		{
			void* rowCurr = impl->rows[index];
		
			mgRect r;
			r.left = pos.x;
			r.top = pos.y;
			r.right = e->transformWorld.clipArea.right;
			r.bottom = r.top + impl->rowHeight;

			mgRect rClip = r;
			if (rClip.top < e->parent->transformWorld.clipArea.top)
				rClip.top = e->parent->transformWorld.clipArea.top;

			if (r.bottom < e->transformWorld.clipArea.top)
				goto end;

			if (r.top > e->transformWorld.clipArea.bottom)
				goto end;

			e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
			if (impl->drawItemBG)
			{
				mgColor* ibgc = &style->tableRowBG2;
				int rea = mgDrawRectangleReason_tableRowBG2;

				if (index)
				{
					if ((index % 2) != 0)
					{
						rea = mgDrawRectangleReason_tableRowBG1;
						ibgc = &style->tableRowBG1;
					}
				}

				e->window->context->gpu->drawRectangle(rea,
					impl,
					&r,
					ibgc,
					ibgc,
					0, 0);
			}


			if (mgPointInRect(&rClip, &ctx->input->mousePosition) && !impl->hoverRow && e->cursorInRect)
			{
				impl->hoverRowIndex = index;
				impl->hoverRow = rowCurr;
				impl->hoverRowClipRect = rClip;
				impl->hoverRowBuildRect = r;

				e->window->context->gpu->drawRectangle(mgDrawRectangleReason_tableHoverItemBG,
					impl,
					&r,
					&style->tableRowHoverBG,
					&style->tableRowHoverBG, 0, 0);
			}

			if (impl->onDrawRow)
			{
				mgPoint pos2 = pos;

				int isRowSelected = 0;
				if (impl->onIsRowSelected)
					isRowSelected = impl->onIsRowSelected(e, rowCurr);

				if (isRowSelected)
				{
					e->window->context->gpu->drawRectangle(
						mgDrawRectangleReason_tableRowSelectedBG,
						impl,
						&r,
						&style->tableRowSelectedBG,
						&style->tableRowSelectedBG,
						0, 0);
				}

				mgUnicodeChar* str = 0;
				uint32_t strLen = 0;
				for (uint32_t i2 = 0; i2 < impl->numCols; ++i2)
				{
					str = 0;
					strLen = 0;

					pos2.x += 2;

					mgRect rctCol, rctColClip;
					rctCol.left = 0;
					rctCol.top = 0;
					rctCol.right = 0;
					rctCol.bottom = 0;

					rctCol.left = pos2.x;
					rctCol.top = pos2.y;
					rctCol.right = rctCol.left + impl->colsSizes[i2];
					rctCol.bottom = rctCol.top + impl->rowHeight;

					rctColClip = rctCol;

					if (rctColClip.top < e->transformWorld.clipArea.top)
						rctColClip.top = e->transformWorld.clipArea.top;
					if (rctColClip.bottom > e->transformWorld.clipArea.bottom)
						rctColClip.bottom = e->transformWorld.clipArea.bottom;
					e->window->context->gpu->setClipRect(&rctColClip);

					e->window->context->gpu->drawRectangle(
						mgDrawRectangleReason_tableCellBG,
						impl,
						&rctCol,
						&style->tableCellBG,
						&style->tableCellBG,
						0, 0);

					if (impl->onDrawRow(e, rowCurr, i2, &str, &strLen))
					{

						if (impl->hoverRow == rowCurr)
						{
							if (mgPointInRect(&rctColClip, &ctx->input->mousePosition))
							{
								impl->hoverColIndex = i2;
								impl->hoverCellClipRect = rctColClip;
							}
						}

						if (str)
						{
							/*e->window->context->gpu->drawText(mgDrawTextReason_table,
								impl,
								&pos2,
								str,
								strLen,
								&style->tableCellText,
								impl->font);*/
							impl->textProcessor->onDrawText(
								mgDrawTextReason_table,
								e,
								impl->textProcessor,
								&pos2,
								str,
								(size_t)strLen,
								&style->tableCellText);
						}

						if (impl->onGetUserElementNum && impl->onGetUserElement && impl->onBeginGetUserElement)
						{
							int userElementNum = impl->onGetUserElementNum(e, rowCurr, index, i2);

							for (int i3 = 0; i3 < userElementNum; ++i3)
							{
								struct mgElement_s* userE = impl->onGetUserElement(e, i3, rowCurr, index, i2);
								if (userE)
								{
									userE->transformWorld = userE->transformLocal;
									userE->transformWorld.buildArea.left += rctCol.left;
									userE->transformWorld.buildArea.right += rctCol.left;
									userE->transformWorld.buildArea.top += rctCol.top;
									userE->transformWorld.buildArea.bottom += rctCol.top;
									userE->transformWorld.clipArea.left += rctColClip.left;
									userE->transformWorld.clipArea.right += rctColClip.left;
									userE->transformWorld.clipArea.top += rctColClip.top;
									userE->transformWorld.clipArea.bottom += rctColClip.top;

									if (userE->transformWorld.clipArea.top < e->transformWorld.clipArea.top)
										userE->transformWorld.clipArea.top = e->transformWorld.clipArea.top;
									if (userE->transformWorld.clipArea.bottom > e->transformWorld.clipArea.bottom)
										userE->transformWorld.clipArea.bottom = e->transformWorld.clipArea.bottom;
									
									userE->onUpdate(userE);
									userE->onDraw(userE);
								}
							}
						}
					}

					pos2.x = pos2.x + (rctCol.right - rctCol.left);
				}
			}

		end:;
			pos.y = pos.y + impl->rowHeight;

			++index;
			if (index >= impl->numRows)
				break;
		}
	}
}

void
miGUI_onRebuild_table(mgElement* e) 
{
}

mgElement* MG_C_DECL
mgCreateTable(struct mgWindow_s* w, 
	mgPoint* position, 
	mgPoint* size, 
	void** rows, 
	uint32_t rowsSz, 
	uint32_t colNum, 
	struct mgTextProcessor_s* tp)
{
	assert(w);
	assert(position);
	assert(size);

	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_TABLE;

	newElement->transformLocal.buildArea.left = position->x;
	newElement->transformLocal.buildArea.top = position->y;
	newElement->transformLocal.buildArea.right = position->x + size->x;
	newElement->transformLocal.buildArea.bottom = position->y + size->y;
	newElement->transformLocal.clipArea = newElement->transformLocal.buildArea;
	newElement->transformLocal.sz = *size;
	newElement->creationRect = newElement->transformLocal.buildArea;

	newElement->window = w;

	mgElementDefaultInit(newElement);
	newElement->onDraw = miGUI_onDraw_table;
	newElement->onUpdate = miGUI_onUpdate_table;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_table;
	newElement->onRebuild = miGUI_onRebuild_table;

	newElement->implementation = calloc(1, sizeof(mgElementTable));
	mgElementTable* impl = (mgElementTable*)newElement->implementation;
	impl->rows = rows;
	impl->numRows = rowsSz;
	impl->numCols = colNum;
	//impl->font = f;
	impl->textProcessor = tp;
	impl->rowHeight = 16;
	impl->drawItemBG = 1;
	impl->scrollSpeed = 10.f;
	impl->colTitleHeight = 20;
	size->x = 20;
	size->y = 20;
	impl->textInput = mgCreateTextInput(w, position, size, tp);
	impl->textInput->userData = newElement;
	impl->textInput->visible = 0;
	((mgElementTextInput*)impl->textInput->implementation)->onActivate = mgElementTable_textinput_onActivate;
	((mgElementTextInput*)impl->textInput->implementation)->onEndEdit = mgElementTable_textinput_onEndEdit;
	((mgElementTextInput*)impl->textInput->implementation)->onCharEnter = mgElementTable_textinput_onCharEnter;
	((mgElementTextInput*)impl->textInput->implementation)->onPaste = mgElementTable_textinput_onPaste;
	mgSetParent(impl->textInput, newElement);

	mgSetParent(newElement, 0);
	return newElement;
}
