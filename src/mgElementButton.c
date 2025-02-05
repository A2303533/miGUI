﻿/*
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
	if(e->enabled)
		miGUI_onUpdate_rectangle(e);

	if (impl->pushButton)
	{
		/*int inRect = mgPointInRect(&e->transformWorld.clipArea, &e->window->context->input->mousePosition);*/
		
		if ((e->window->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN) && e->cursorInRect)
		{
			if (impl->pushButtonState)
			{
				impl->pushButtonState = 0;
				if (impl->onPushOff)
					impl->onPushOff(e);
			}
			else
			{
				impl->pushButtonState = 1;
				if (impl->onPushOn)
					impl->onPushOn(e);
			}
		}
	}
}

void 
miGUI_onDraw_button(mgElement* e)
{
	mgElementButton* impl = (mgElementButton*)e->implementation;
	
	mgStyle* style = e->userStyle;
	int iconID = impl->iconID_default;

	if(!e->userStyle)
		style = e->window->context->activeStyle;

	if (e->enabled)
	{
		if (impl->pushButton)
		{
			if (impl->pushButtonState)
			{
				impl->colorFinal1 = style->buttonPress1;
				impl->colorFinal2 = style->buttonPress2;
				iconID = impl->iconID_push;
			}
			else
			{
				if (e->elementState & 0x1)
				{
					impl->colorFinal1 = style->buttonHover1;
					impl->colorFinal2 = style->buttonHover2;
					iconID = impl->iconID_hover;
				}
				else
				{
					impl->colorFinal1 = style->button1;
					impl->colorFinal2 = style->button2;
					iconID = impl->iconID_default;
				}
			}
		}
		else
		{
			if (e->elementState & 0x1)
			{
				impl->colorFinal1 = style->buttonHover1;
				impl->colorFinal2 = style->buttonHover2;
				iconID = impl->iconID_hover;
			}
			else
			{
				impl->colorFinal1 = style->button1;
				impl->colorFinal2 = style->button2;
				iconID = impl->iconID_default;
			}

			if (e->elementState & 0x2)
			{
				impl->colorFinal1 = style->buttonPress1;
				impl->colorFinal2 = style->buttonPress2;
				iconID = impl->iconID_push;
			}
		}
	}
	else
	{
		impl->colorFinal1 = style->buttonDisabled1;
		impl->colorFinal2 = style->buttonDisabled2;
		iconID = impl->iconID_disable;
	}

	if (e->drawBG)
	{
		e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
		int reason = mgDrawRectangleReason_buttonBG;
		if (impl->BGtexture)
			reason = mgDrawRectangleReason_buttonBGTexture;

		e->window->context->gpu->drawRectangle(
			reason,
			impl,
			&e->transformWorld.buildArea, 
			&impl->colorFinal1, 
			&impl->colorFinal2, 
			impl->BGtexture,
			0);
	}

	if (impl->icons)
	{
		if (!e->drawBG)
			e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);

		/*e->window->context->currentIcon.left = impl->icons->iconNodes[iconID].lt.x;
		e->window->context->currentIcon.top = impl->icons->iconNodes[iconID].lt.y;
		e->window->context->currentIcon.right = impl->icons->iconNodes[iconID].sz.x;
		e->window->context->currentIcon.bottom = impl->icons->iconNodes[iconID].sz.y;*/
		e->window->context->currentIcon = &impl->icons->iconNodes[iconID];
		e->window->context->gpu->drawRectangle(mgDrawRectangleReason_buttonIcon,
			e->window,
			&e->transformWorld.buildArea, 
			&style->buttonIcon,
			&style->buttonIcon,
			impl->icons->texture, &impl->icons->iconNodes[iconID].uv);
	}

	size_t textLen = 0;
	const mgUnicodeChar* text = impl->onText(e, &textLen);

	if (text && textLen)
	{
		if (e->enabled)
		{
			if (e->elementState & 0x1)
				impl->textColorFinal = style->buttonTextHover;
			else
				impl->textColorFinal = style->buttonText;

			if (e->elementState & 0x2)
				impl->textColorFinal = style->buttonTextPress;
		}
		else
		{
			impl->textColorFinal = style->buttonTextDisabled;
		}

		mgPoint pos;
		pos.x = e->transformWorld.buildArea.left;
		pos.y = e->transformWorld.buildArea.top;
		pos.x += impl->textIndentInternal.x;
		pos.y += impl->textIndentInternal.y;
		e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
		/*e->window->context->gpu->drawText(
			mgDrawTextReason_buttonText, 
			impl, 
			&pos, 
			text, 
			textLen, 
			&impl->textColorFinal, 
			impl->onFont(e));*/
		impl->textProcessor->onDrawText(
			mgDrawTextReason_buttonText,
			e,
			impl->textProcessor,
			&pos,
			text,
			textLen,
			&impl->textColorFinal);
	}
}

void
miGUI_onRebuild_button(mgElement* e) 
{
	mgElementButton* impl = (mgElementButton*)e->implementation;
	
	impl->textIndentInternal.x = 0;
	impl->textIndentInternal.y = 0;

	size_t textLen = 0;
	const mgUnicodeChar* text = impl->onText(e, &textLen);

	if (text)
	{
		mgPoint p;
		//e->window->context->getTextSize(text, impl->onFont(e), &p);
		impl->textProcessor->onGetTextSize(mgDrawTextReason_buttonText, e, impl->textProcessor, text, textLen, &p);

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

mgElement* MG_C_DECL
mgCreateButton(struct mgWindow_s* w, mgPoint* position, mgPoint* size, struct mgTextProcessor_s* tp)
{
	assert(w);
	assert(position);
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
	mgElementDefaultInit(newElement);
	newElement->onDraw = miGUI_onDraw_button;
	newElement->onUpdate = miGUI_onUpdate_button;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_button;
	newElement->onRebuild = miGUI_onRebuild_button;

	newElement->implementation = calloc(1, sizeof(mgElementButton));
	mgElementButton* impl = (mgElementButton*)newElement->implementation;
	impl->textProcessor = tp;

	mgSetParent(newElement, 0);

	// now user must call rebuild function (or just context->needRebuild = 1;) 
	//miGUI_onRebuild_button(newElement);

	return newElement;
}
