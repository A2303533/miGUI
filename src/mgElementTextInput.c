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

void miGUI_onUpdateTransform_rectangle(mgElement* e);

void
miGUI_onUpdateTransform_textinput(mgElement* e)
{
	miGUI_onUpdateTransform_rectangle(e);
}

void 
miGUI_onUpdate_textinput(mgElement* e)
{
}

void 
miGUI_onDraw_textinput(mgElement* e)
{
	mgStyle* style = e->userStyle;
	if (!style)
		style = e->window->context->activeStyle;

	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	int isActive = impl == e->window->context->activeTextInput;

	e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);

	if (impl->drawBG)
	{
		mgColor* c = &style->textInputBGNotActive;

		if (e->window->context->activeTextInput == impl)
			c = &style->textInputBGActive;

		e->window->context->gpu->drawRectangle(mgDrawRectangleReason_textInputBG,
			impl, &e->transformWorld.buildArea, c, c, 0, 0);
	}


	if (impl->text && impl->textLen)
	{
		if (impl->multiline)
		{

		}
		else
		{
			mgPoint pos;
			pos.x = e->transformWorld.buildArea.left;
			pos.y = e->transformWorld.buildArea.top;

			for (int i = 0; i < impl->textLen; ++i)
			{
				e->window->context->gpu->drawText(
					mgDrawTextReason_textInputDefaultText,
					impl,
					&pos,
					&impl->text[i],
					1,
					&style->textInputDefaultText,
					impl->font);

				if (impl->monospace)
				{
					pos.x += impl->font->maxSize.x + impl->font->characterSpacing;
				}
				else
				{
					mgPoint p;
					wchar_t t[2] = { impl->text[i], 0 };
					e->window->context->getTextSize(t, impl->font, &p);

					pos.x += p.x + impl->font->characterSpacing;
				}

			}
		}
	}
	else
	{
		if (impl->defaultText && !isActive)
		{
			mgPoint pos;
			pos.x = e->transformWorld.buildArea.left;
			pos.y = e->transformWorld.buildArea.top;

			e->window->context->gpu->drawText(
				mgDrawTextReason_textInputDefaultText,
				impl, 
				&pos,
				impl->defaultText, 
				impl->defaultTextLen,
				&style->textInputDefaultText,
				impl->font);
		}
	}
}

void
miGUI_onRebuild_textinput(mgElement* e) {
	mgElementText* impl = (mgElementText*)e->implementation;
	if (impl->text && e->window->context->getTextSize)
	{
		/*mgPoint p;
		e->window->context->getTextSize(impl->text, impl->font, &p);
		e->transformLocal.sz = p;
		miGUI_onUpdateTransform_rectangle(e);*/
	}
}

MG_API
mgElement* MG_C_DECL
mgCreateTextInput_f(struct mgWindow_s* w, mgRect* r, mgFont* font)
{
	assert(w);
	assert(r);
	assert(font);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_TEXTINPUT;
	
	newElement->transformLocal.buildArea = *r;
	newElement->transformLocal.clipArea = newElement->transformLocal.buildArea;
	newElement->creationRect = newElement->transformLocal.buildArea;
	
	newElement->window = w;
	newElement->visible = 1;
	newElement->onDraw = miGUI_onDraw_textinput;
	newElement->onUpdate = miGUI_onUpdate_textinput;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_textinput;
	newElement->onRebuild = miGUI_onRebuild_textinput;

	newElement->implementation = calloc(1, sizeof(mgElementTextInput));
	mgElementTextInput* impl = (mgElementTextInput*)newElement->implementation;
	impl->font = font;
	impl->drawBG = 1;
	impl->defaultText = L"Click...";
	impl->defaultTextLen = 9;

	mgTextInputClear_f(impl, 1);

	mgSetParent_f(newElement, 0);
	miGUI_onRebuild_textinput(newElement);

	return newElement;
}

void
mgTextInputSetText_reallocate(struct mgElementTextInput_s* e, int newSize)
{
	wchar_t* newBuffer = malloc((newSize + 1) * sizeof(wchar_t));
	for (int i = 0; i < e->textLen; ++i)
	{
		newBuffer[i] = e->text[i];
	}
	newBuffer[newSize] = 0;
	e->allocated = newSize;
	free(e->text);
	e->text = newBuffer;
}

MG_API 
void MG_C_DECL 
mgTextInputSetText_f(struct mgElementTextInput_s* e, const wchar_t* text)
{
	assert(e);

	if (text)
	{
		int len = wcslen(text);
		if (len)
		{
			if (len > e->allocated)
				mgTextInputSetText_reallocate(e, len);
			mgTextInputClear_f(e, 0);
			for (int i = 0; i < len; ++i)
			{
				e->text[i] = text[i];
			}
			e->textLen = len;
			e->text[len] = 0;
		}
		else
		{
			e->textLen = 0;
			e->text[0] = 0;
		}
	}
	else
	{
		e->textLen = 0;
		e->text[0] = 0;
	}
}

MG_API 
void MG_C_DECL 
mgTextInputClear_f(struct mgElementTextInput_s* e, int freeMemory)
{
	if (freeMemory)
	{
		if (e->text)
			free(e->text);

		e->allocated = 10;
		e->text = malloc((e->allocated + 1) * sizeof(wchar_t));
	}

	e->textLen = 0;
	e->text[0] = 0;
}

