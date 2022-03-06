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
#include <string.h>

void miGUI_onUpdate_rectangle(mgElement* e);
void miGUI_onUpdateTransform_rectangle(mgElement* e);

void
miGUI_onUpdateTransform_button(mgElement* e)
{
	miGUI_onUpdateTransform_rectangle(e);
}

void 
miGUI_onUpdate_button(mgElement* e)
{
	mgElementButton* impl = (mgElementButton*)e->implementation;
	if(impl->enabled)
		miGUI_onUpdate_rectangle(e);
}

void 
miGUI_onDraw_button(mgElement* e)
{
	mgElementButton* impl = (mgElementButton*)e->implementation;
	
	mgStyle* style = e->userStyle;
	int iconID = 0;

	if(!e->userStyle)
		style = e->window->context->activeStyle;

	if (impl->enabled)
	{
		if (e->elementState & 0x1)
		{
			impl->colorFinal1 = style->buttonColorHover1;
			impl->colorFinal2 = style->buttonColorHover2;
		}
		else
		{
			impl->colorFinal1 = style->buttonColor1;
			impl->colorFinal2 = style->buttonColor2;
		}

		if (e->elementState & 0x2)
		{
			impl->colorFinal1 = style->buttonColorPress1;
			impl->colorFinal2 = style->buttonColorPress2;
		}
	}
	else
	{
		impl->colorFinal1 = style->buttonColorDisabled1;
		impl->colorFinal2 = style->buttonColorDisabled2;
	}

	if (impl->drawBG)
	{
		e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
		e->window->context->gpu->drawRectangle(mgDrawRectangleReason_buttonBG,
			impl,
			&e->transformWorld.buildArea, &impl->colorFinal1, &impl->colorFinal2, 0, 0);
	}

	if (impl->icons)
	{
		if (!impl->drawBG)
			e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);

		e->window->context->currentIcon.left = impl->icons->iconNodes[iconID].lt.x;
		e->window->context->currentIcon.top = impl->icons->iconNodes[iconID].lt.y;
		e->window->context->currentIcon.right = impl->icons->iconNodes[iconID].sz.x;
		e->window->context->currentIcon.bottom = impl->icons->iconNodes[iconID].sz.y;
		e->window->context->gpu->drawRectangle(mgDrawRectangleReason_buttonIcon,
			e->window,
			&e->transformWorld.buildArea, 
			&e->window->context->whiteColor, 
			&e->window->context->whiteColor,
			impl->icons->texture, 0);
	}

	if (impl->text && impl->textLen)
	{
		if (impl->enabled)
		{
			if (e->elementState & 0x1)
				impl->textColorFinal = style->buttonTextColorHover;
			else
				impl->textColorFinal = style->buttonTextColor;

			if (e->elementState & 0x2)
				impl->textColorFinal = style->buttonTextColorPress;
		}
		else
		{
			impl->textColorFinal = style->buttonTextColorDisabled;
		}

		mgPoint pos;
		pos.x = e->transformWorld.buildArea.left;
		pos.y = e->transformWorld.buildArea.top;
		pos.x += impl->textIndentInternal.x;
		pos.y += impl->textIndentInternal.y;
		e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
		e->window->context->gpu->drawText(mgDrawTextReason_buttonText, impl, 
			&pos, impl->text, impl->textLen, &impl->textColorFinal, impl->font);
	}
}

void
miGUI_onRebuild_button(mgElement* e) 
{
	mgElementButton* impl = (mgElementButton*)e->implementation;
	
	impl->textIndentInternal.x = 0;
	impl->textIndentInternal.y = 0;

	if (impl->text && e->window->context->getTextSize)
	{
		mgPoint p;
		e->window->context->getTextSize(impl->text, impl->font, &p);

		mgPoint buttonCenter;
		buttonCenter.x = (int)((float)(e->transformLocal.buildArea.right - e->transformLocal.buildArea.left) * 0.5f);
		buttonCenter.y = (int)((float)(e->transformLocal.buildArea.bottom - e->transformLocal.buildArea.top) * 0.5f);

		mgPoint textHalfLen;
		textHalfLen.x = (int)((float)p.x * 0.5f);
		textHalfLen.y = (int)((float)p.y * 0.5f);

		impl->textIndentInternal.x += buttonCenter.x - textHalfLen.x;
		impl->textIndentInternal.y += buttonCenter.y - textHalfLen.y;
	}
}

MG_API
mgElement* MG_C_DECL
mgCreateButton_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, const wchar_t* text, mgFont* font)
{
	assert(w);
	assert(position);
	assert(text);
	assert(font);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_BUTTON;

	newElement->transformLocal.buildArea.left = position->x;
	newElement->transformLocal.buildArea.top = position->y;
	newElement->transformLocal.buildArea.right = position->x + size->x;
	newElement->transformLocal.buildArea.bottom = position->y + size->y;
	newElement->transformLocal.clipArea = newElement->transformLocal.buildArea;
	newElement->transformLocal.sz = *size;
	newElement->creationRect = newElement->transformLocal.buildArea;
	
	newElement->window = w;
	newElement->visible = 1;
	newElement->onDraw = miGUI_onDraw_button;
	newElement->onUpdate = miGUI_onUpdate_button;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_button;
	newElement->onRebuild = miGUI_onRebuild_button;

	newElement->implementation = calloc(1, sizeof(mgElementButton));
	mgElementButton* impl = (mgElementButton*)newElement->implementation;
	impl->enabled = 1;
	impl->drawBG = 1;
	
	impl->font = font;
	impl->text = text;
	impl->textLen = wcslen(text);
	
	mgSetParent_f(newElement, 0);

	miGUI_onRebuild_button(newElement);

	return newElement;
}
