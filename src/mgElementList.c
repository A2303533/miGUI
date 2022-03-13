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
#include <stdio.h>
#include <wchar.h>

void miGUI_onUpdateTransform_rectangle(mgElement* e);
void miGUI_onUpdate_rectangle(mgElement* e);
void miGUI_onUpdateTransform_textinput(mgElement* e);
float lerp(float v0, float v1, float t);

struct lbData1
{
	const wchar_t* text;
};
struct lbData2
{
	const wchar_t* text;
	uint32_t flags;
};

int mgElementList_textinput_onEndEdit(struct mgElement_s* e, int type)
{
	e->visible = 0;
	e->window->context->activeTextInput = 0;
	
	mgSetCursor_f(e->window->context, e->window->context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);

	mgElementTextInput* ti = e->implementation;
	mgElement* listBox = (mgElement*)e->userData;
	mgElementList* listBoxImpl = (mgElementList*)listBox->implementation;

	if (listBoxImpl->onTextInputEndEdit)
		return listBoxImpl->onTextInputEndEdit(listBox, type, ti->text, listBoxImpl->editItem);

	return 1;
}

wchar_t mgElementList_textinput_onCharEnter(struct mgElement_s* e, wchar_t c)
{
	mgElementTextInput* ti = e->implementation;
	mgElement* listBox = (mgElement*)e->userData;
	mgElementList* listBoxImpl = (mgElementList*)listBox->implementation;
	
	if (listBoxImpl->onTextInputCharEnter)
		return listBoxImpl->onTextInputCharEnter(listBox, c);

	return c;
}
void mgElementList_textinput_onActivate(struct mgElement_s* e)
{
	mgElement* listBox = (mgElement*)e->userData;
	mgElementList* listBoxImpl = (mgElementList*)listBox->implementation;

	mgElementTextInput* ti = e->implementation;
	if(listBoxImpl->hoverItemText)
		mgTextInputSetText_f(ti, listBoxImpl->hoverItemText);


	ti->isSelected = 1;
	ti->selectionStart = 0;
	ti->selectionEnd = ti->textLen;
	ti->textCursor = ti->textLen;
}

void
miGUI_onUpdateTransform_list(mgElement* e)
{
	miGUI_onUpdateTransform_rectangle(e);

	mgElementList* impl = (mgElementList*)e->implementation;
	impl->numOfLines = 0;
	int w = e->transformWorld.buildArea.bottom - e->transformWorld.buildArea.top;
	if (w > 0)
	{
		if (impl->itemHeight < 2)
			impl->itemHeight = 2;

		impl->numOfLines = (w / impl->itemHeight) + 2;
	}

	e->contentHeight = impl->arraySize * impl->itemHeight;
}

void 
miGUI_onUpdate_list(mgElement* e)
{
	miGUI_onUpdate_rectangle(e);

	mgContext* c = e->window->context;
	mgElementList* impl = (mgElementList*)e->implementation;

	if (e->contentHeight > e->clientHeight)
	{
		if (mgPointInRect(&e->transformWorld.clipArea, &c->input->mousePosition))
		{
			if (c->input->mouseWheelDelta < 0.f)
			{
				impl->itemScrollValueTarget += 10.f;
				if (impl->itemScrollValueTarget > (e->contentHeight - e->clientHeight))
					impl->itemScrollValueTarget = (float)(e->contentHeight - e->clientHeight);
			}
			else if (c->input->mouseWheelDelta > 0.f)
			{
				impl->itemScrollValueTarget -= 10.f;

				if (impl->itemScrollValueTarget < 0.f)
					impl->itemScrollValueTarget = 0.f;
			}
		}

		impl->itemScrollValue = lerp(impl->itemScrollValue, impl->itemScrollValueTarget, 0.1f);

		impl->itemScrollValueWorld = impl->itemScrollValue;/*parent must be somewhere*/
	}

	if (impl->arraySize && (impl->arraySize > impl->numOfLines))
	{
		mgPoint pos;
		pos.x = e->transformWorld.buildArea.left;
		pos.y = e->transformWorld.buildArea.top;
		
		pos.y = pos.y + (impl->itemHeight * impl->firstItemIndexForDraw) - (int)impl->itemScrollValueWorld;

		if (pos.y < (e->transformWorld.buildArea.top - impl->itemHeight))
		{
			++impl->firstItemIndexForDraw;

			uint32_t lastIndex = impl->arraySize - impl->numOfLines;
			if (impl->firstItemIndexForDraw > lastIndex)
			{
				impl->firstItemIndexForDraw = lastIndex;
			}
		}
		else if (pos.y > e->transformWorld.buildArea.top - impl->itemHeight)
		{
			if (impl->firstItemIndexForDraw)
				--impl->firstItemIndexForDraw;
		}
	}

	if (impl->hoverItem)
	{
		if ((e->lmbClickCount == 2) && (impl->editText))
		{
			mgElementTextInput* ti = (mgElementTextInput*)impl->textInput->implementation;
			impl->textInput->visible = 1;

			mgTextInputActivate_f(c, ti, 1, 0);
			mgRect r;
			r.left = 0;
			r.right = e->transformWorld.buildArea.right - e->transformWorld.buildArea.left;
			r.top = impl->hoverItemBuildRect.top - e->transformWorld.buildArea.top;
			r.bottom = r.top + impl->itemHeight;

			impl->textInput->transformLocal.buildArea = r;
			impl->textInput->transformLocal.clipArea = r;

			impl->editItem = impl->hoverItem;

			miGUI_onUpdateTransform_textinput(impl->textInput);
		}
		else
		{
			if (impl->multiselect)
			{
				struct lbData2* dptr = (struct lbData2*)impl->hoverItem;
				if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				{
					if (dptr->flags & 0x1)
						dptr->flags ^= 0x1;
					else
					{
						if (impl->onSelect)
						{
							if (impl->onSelect(e))
								dptr->flags |= 0x1;
						}
						else
						{
							dptr->flags |= 0x1;
						}
					}
				}
			}
			else
			{
				if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				{
					if (impl->isSelected)
					{
						uint32_t index = impl->hoverItem - (uint8_t*)impl->array;
						index /= impl->dataTypeSizeOf;

						if (index != impl->curSel)
						{
							if (impl->onSelect)
							{
								if (impl->onSelect(e))
									impl->curSel = index;
							}
							else
							{
								impl->curSel = index;
							}
						}
						else
						{
							impl->isSelected = 0;
							impl->curSel = 0;
						}
					}
					else
					{
						if (impl->onSelect)
						{
							if (impl->onSelect(e))
							{
								impl->isSelected = 1;
								impl->curSel = impl->hoverItem - (uint8_t*)impl->array;
								impl->curSel /= impl->dataTypeSizeOf;
							}
						}
						else
						{
							impl->isSelected = 1;
							impl->curSel = impl->hoverItem - (uint8_t*)impl->array;
							impl->curSel /= impl->dataTypeSizeOf;
						}
					}
				}
			}
		}
	}
}

void 
miGUI_onDraw_list(mgElement* e)
{
	mgStyle* style = e->userStyle;
	if (!style)
		style = e->window->context->activeStyle;

	mgContext* ctx = e->window->context;

	mgElementList* impl = (mgElementList*)e->implementation;

	e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
	if (e->drawBG)
	{
		e->window->context->gpu->drawRectangle(mgDrawRectangleReason_listBG, impl,
			&e->transformWorld.buildArea,
			&style->listBG,
			&style->listBG, 0, 0);
	}

	if (impl->arraySize && impl->font)
	{
		uint32_t index = impl->firstItemIndexForDraw;
		mgPoint pos;
		pos.x = e->transformWorld.buildArea.left;
		pos.y = e->transformWorld.buildArea.top - (int)impl->itemScrollValueWorld;
		pos.y = pos.y + (impl->itemHeight * index);

		uint8_t* u8ptr = (uint8_t*)impl->array;

		impl->hoverItem = 0;
		for (uint32_t i = 0; i < impl->numOfLines; ++i)
		{
			const wchar_t* str = 0;
			uint8_t* u8ptr_curr = u8ptr + (index * impl->dataTypeSizeOf);
			
			mgRect r;
			r.left = pos.x;
			r.top = pos.y;
			r.right = e->transformWorld.clipArea.right;
			r.bottom = r.top + impl->itemHeight;

			mgRect rClip = r;
			if (rClip.top < e->parent->transformWorld.clipArea.top)
				rClip.top = e->parent->transformWorld.clipArea.top;

			if (mgPointInRect(&rClip, &ctx->input->mousePosition) && !impl->hoverItem)
			{
				impl->hoverItem = u8ptr_curr;
				impl->hoverItemClipRect = rClip;
				impl->hoverItemBuildRect = r;

				e->window->context->gpu->drawRectangle(mgDrawRectangleReason_listHoverItemBG, 
					impl,
					&r,
					&style->listItemHoverBG,
					&style->listItemHoverBG, 0, 0);
			}

			if (impl->multiselect)
			{
				struct lbData2* dptr = (struct lbData2*)u8ptr_curr;
				str = dptr->text;
				if (dptr->flags & 0x1)
				{
					e->window->context->gpu->drawRectangle(mgDrawRectangleReason_listHoverItemBG,
						impl,
						&r,
						&style->listItemSelectedBG,
						&style->listItemSelectedBG, 0, 0);
				}
			}
			else
			{
				struct lbData1* dptr = (struct lbData1*)u8ptr_curr;
				str = dptr->text;

				if (impl->isSelected && (impl->curSel == index))
				{
					e->window->context->gpu->drawRectangle(mgDrawRectangleReason_listHoverItemBG,
						impl,
						&r,
						&style->listItemSelectedBG,
						&style->listItemSelectedBG, 0, 0);
				}
			}

			if(impl->hoverItem == u8ptr_curr)
				impl->hoverItemText = str;

			e->window->context->gpu->drawText(mgDrawTextReason_text, impl, 
				&pos,
				str,
				wcslen(str),
				&style->listItemText,
				impl->font);

			pos.y = pos.y + impl->itemHeight;

			++index;
			if (index >= impl->arraySize)
				break;
		}
	}
}

void
miGUI_onRebuild_list(mgElement* e) 
{
}

MG_API
mgElement* MG_C_DECL
mgCreateListBox_f(
	struct mgWindow_s* w, 
	mgPoint* position, 
	mgPoint* size, 
	void* array, 
	uint32_t arraySize, 
	uint32_t dataTypeSizeOf,
	mgFont* f)
{
	assert(w);
	assert(position);
	assert(size);
	assert(array);
	assert(arraySize > 0);

	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_LIST;

	newElement->transformLocal.buildArea.left = position->x;
	newElement->transformLocal.buildArea.top = position->y;
	newElement->transformLocal.buildArea.right = position->x + size->x;
	newElement->transformLocal.buildArea.bottom = position->y + size->y;
	newElement->transformLocal.clipArea = newElement->transformLocal.buildArea;
	newElement->transformLocal.sz = *size;
	newElement->creationRect = newElement->transformLocal.buildArea;

	newElement->window = w;

	mgElementDefaultInit(newElement);
	newElement->onDraw = miGUI_onDraw_list;
	newElement->onUpdate = miGUI_onUpdate_list;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_list;
	newElement->onRebuild = miGUI_onRebuild_list;

	newElement->implementation = calloc(1, sizeof(mgElementList));
	mgElementList* impl = (mgElementList*)newElement->implementation;
	impl->array = array;
	impl->dataTypeSizeOf = dataTypeSizeOf;
	impl->arraySize = arraySize;
	impl->isSelected = 0;
	impl->multiselect = 0;
	impl->curSel = 0;
	impl->itemHeight = 12;
	impl->font = f;
	size->x = 20;
	size->y = 20;
	impl->textInput = mgCreateTextInput_f(w, position, size, f);
	impl->textInput->userData = newElement;
	impl->textInput->visible = 0;
	((mgElementTextInput*)impl->textInput->implementation)->onActivate = mgElementList_textinput_onActivate;
	((mgElementTextInput*)impl->textInput->implementation)->onEndEdit = mgElementList_textinput_onEndEdit;
	((mgElementTextInput*)impl->textInput->implementation)->onCharEnter = mgElementList_textinput_onCharEnter;
	mgSetParent_f(impl->textInput, newElement);

	mgSetParent_f(newElement, 0);
	return newElement;
}
