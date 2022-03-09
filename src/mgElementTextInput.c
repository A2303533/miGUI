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

inline double
lerpd(double v0, double v1, double t)
{
	return (1.0 - t) * v0 + t * v1;
}
void miGUI_onUpdateTransform_rectangle(mgElement* e);
void miGUI_onUpdate_rectangle(mgElement* e);

void
mgTextInput_updateScrollLimit(mgElement* e)
{
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	impl->h_scrollLimit = 0.0;

	int rectSizeX = e->transformWorld.buildArea.right - e->transformWorld.buildArea.left;

	if ((int)impl->textWidth > rectSizeX)
	{
		impl->h_scrollLimit = impl->textWidth - rectSizeX + 3;
	}
}

void
mgTextInputSetText_reallocate(struct mgElementTextInput_s* e, int newSize)
{
	wchar_t* newBuffer = malloc((newSize + 1) * sizeof(wchar_t));
	for (uint32_t i = 0; i < e->textLen; ++i)
	{
		newBuffer[i] = e->text[i];
	}
	newBuffer[newSize] = 0;
	e->allocated = newSize;
	free(e->text);
	e->text = newBuffer;
}

void
miGUI_onUpdateTransform_textinput(mgElement* e)
{
	miGUI_onUpdateTransform_rectangle(e);
}

void
miGUI_textinput_activate(mgElement* e, int is)
{
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	impl->drawTextCursor = 0;
	impl->textCursorTimer = 0.f;
	if (is)
	{
		e->window->context->activeTextInput = impl;
		if (impl->onActivate)
			impl->onActivate(impl);
	}
	else
	{
		e->window->context->activeTextInput = 0;
		if (impl->onEndEdit)
			impl->onEndEdit(impl);
	}
}

void 
mgTextInputDeselect(struct mgElementTextInput_s* e)
{
	e->selectionStart = 0;
	e->selectionEnd = 0;
	e->isSelected = 0;
}

MG_API 
void MG_C_DECL 
mgTextInputDeleteSelected_f(struct mgElementTextInput_s* e)
{
	mgTextInputDeselect(e);
}

void
mgTextInput_backspace(struct mgElementTextInput_s* e)
{
	if (e->isSelected)
	{
		mgTextInputDeleteSelected_f(e);
		return;
	}
	
	int ok = 0;
	wchar_t* buf = e->text;
	uint32_t str_len = e->textLen;
	for (uint32_t i = e->textCursor; i < str_len; ++i)
	{
		if (i == 0)
			break;
		ok = 1;
		buf[i - 1] = buf[i];
	}
	if (e->textCursor == str_len && !ok)
		ok = 1;
	if (ok)
	{
		if (str_len - 1 >= 0)
		{
			if (e->textCursor)
			{
				--e->textCursor;
				buf[str_len - 1] = 0;
				--e->textLen;
			}
		}
	}
}
void
mgTextInput_delete(struct mgElementTextInput_s* e)
{
	if (e->isSelected)
	{
		mgTextInputDeleteSelected_f(e);
		return;
	}

	if (e->textCursor < e->textLen)
	{
		int ok = 0;
		wchar_t* buf = e->text;
		uint32_t str_len = e->textLen;
		for (size_t i = e->textCursor; i < str_len; ++i)
		{
			ok = 1;
			if (i + 1 == str_len)
				break;
			buf[i] = buf[i + 1];
		}
		if (ok)
		{
			buf[str_len - 1] = 0;
			--e->textLen;
		}
	}
}

MG_API 
int MG_C_DECL
mgTextInputPutText_f(struct mgElementTextInput_s* e, const wchar_t* text, uint32_t len)
{
	int w = 0;
	
	mgPoint pt;
	e->element->window->context->getTextSize(text, e->font, &pt);
	
	w = pt.x;

	uint32_t newLen = e->textLen + len;
	if ((newLen + 1) > e->allocated)
		mgTextInputSetText_reallocate(e, newLen);

	uint32_t i = e->textLen;
	while (i >= e->textCursor)
	{
		uint32_t next = i + len;
		if (next < e->allocated)
			e->text[next] = e->text[i];

		if (i == 0)
			break;
		--i;
	}
	for (uint32_t i2 = 0; i2 < len; ++i2)
	{
		e->text[e->textCursor] = text[i2];
		e->textCursor++;
	}

	e->textLen = newLen;
	e->text[newLen] = 0;
	return w;
}

void
miGUI_textinput_goLeft(mgElementTextInput* impl)
{
	if (impl->textCursor)
		--impl->textCursor;
}

void
miGUI_textinput_goRight(mgElementTextInput* impl)
{
	++impl->textCursor;
	if (impl->textCursor > impl->textLen)
		impl->textCursor = impl->textLen;
}

void
miGUI_textinput_goUp(mgElementTextInput* impl)
{
	mgTextInputDeselect(impl);
	if (impl->multiline)
	{
	}
}

void
miGUI_textinput_goDown(mgElementTextInput* impl)
{
	mgTextInputDeselect(impl);
	if (impl->multiline)
	{
	}
}

void
miGUI_textinput_goHome(mgElementTextInput* impl)
{
	if (impl->multiline)
	{
	}
	else
	{
		impl->textCursor = 0;
	}
}

void
miGUI_textinput_goEnd(mgElementTextInput* impl)
{
	if (impl->multiline)
	{
	}
	else
	{
		impl->textCursor = impl->textLen;
	}
}

void
miGUI_textinput_goPgUp(mgElementTextInput* impl)
{
}

void
miGUI_textinput_goPgDw(mgElementTextInput* impl)
{
}

void
miGUI_textinput_move(mgElement* e, int key)
{
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	switch (key)
	{
	case MG_KEY_LEFT:
		miGUI_textinput_goLeft(impl);
	break;
	case MG_KEY_RIGHT:
		miGUI_textinput_goRight(impl);
	break;
	case MG_KEY_UP:
		miGUI_textinput_goUp(impl);
		break;
	case MG_KEY_DOWN:
		miGUI_textinput_goDown(impl);
		break;
	case MG_KEY_HOME:
		miGUI_textinput_goHome(impl);
		break;
	case MG_KEY_END:
		miGUI_textinput_goEnd(impl);
		break;
	case MG_KEY_PGUP:
		miGUI_textinput_goPgUp(impl);
		break;
	case MG_KEY_PGDOWN:
		miGUI_textinput_goPgDw(impl);
		break;
	}
	impl->drawTextCursor = 1;
	impl->textCursorTimer = 0.f;
	mgTextInput_updateScrollLimit(e);
}

void 
miGUI_onUpdate_textinput(mgElement* e)
{
	mgContext* c = e->window->context;

	int inRect = mgPointInRect(&e->transformWorld.clipArea, &c->input->mousePosition);
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	
	if (inRect)
	{
		mgSetCursor_f(c, c->defaultCursors[mgCursorType_IBeam], mgCursorType_Arrow);

		if (impl->canEdit)
		{
			if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				if(impl != e->window->context->activeTextInput)
					miGUI_textinput_activate(e, 1);
			}
		}
	}
	else
	{
		if (e->elementState & 0x1)
		{
			mgSetCursor_f(e->window->context, e->window->context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		}

		if (impl->canEdit)
		{
			if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				miGUI_textinput_activate(e, 0);
			}
		}
	}

	miGUI_onUpdate_rectangle(e);

	int isActive = impl == e->window->context->activeTextInput;
	if (isActive)
	{
		if (c->input->keyboardModifier == MG_KBMOD_CTRL)
		{
		}
		else
		{
			if (c->input->character)
			{
				switch (c->input->character)
				{
				case MG_KEY_ESCAPE:
				case MG_KEY_ENTER:
					break;
				case MG_KEY_BACKSPACE:
					mgTextInput_backspace(impl);
					break;
				
				default:
				{
					if (impl->onCharEnter)
						c->input->character = impl->onCharEnter(c->input->character);

					if (c->input->character)
					{
						if (impl->isSelected)
							mgTextInputDeleteSelected_f(impl);

						wchar_t t[2] = { c->input->character, 0 };
						int w = mgTextInputPutText_f(impl, t, 1);
						impl->textWidth += w + impl->font->characterSpacing;

						mgTextInput_updateScrollLimit(e);
					}
				}break;
				}
			}
			else
			{
				if (mgIsKeyHit(c->input, MG_KEY_LEFT))
					miGUI_textinput_move(e, MG_KEY_LEFT);
				else if (mgIsKeyHit(c->input, MG_KEY_RIGHT))
					miGUI_textinput_move(e, MG_KEY_RIGHT);
				else if (mgIsKeyHit(c->input, MG_KEY_UP))
					miGUI_textinput_move(e, MG_KEY_UP);
				else if (mgIsKeyHit(c->input, MG_KEY_DOWN))
					miGUI_textinput_move(e, MG_KEY_DOWN);
				else if (mgIsKeyHit(c->input, MG_KEY_HOME))
					miGUI_textinput_move(e, MG_KEY_HOME);
				else if (mgIsKeyHit(c->input, MG_KEY_END))
					miGUI_textinput_move(e, MG_KEY_END);
				else if (mgIsKeyHit(c->input, MG_KEY_PGDOWN))
					miGUI_textinput_move(e, MG_KEY_PGDOWN);
				else if (mgIsKeyHit(c->input, MG_KEY_PGUP))
					miGUI_textinput_move(e, MG_KEY_PGUP);
				else if (mgIsKeyHit(c->input, MG_KEY_DELETE))
					mgTextInput_delete(impl);
			}
		}

		impl->textCursorRect.left = e->transformWorld.buildArea.left;
		impl->textCursorRect.top = e->transformWorld.buildArea.top;
		impl->textCursorRect.right = impl->textCursorRect.left + 2;
		impl->textCursorRect.bottom = impl->textCursorRect.top + impl->font->maxSize.y;
		/*impl->textBeginDrawIndex; for multiline*/

		impl->textCursorRect.left = impl->textCursorRect.left + impl->textCursorPosition.x;
		impl->textCursorRect.right = impl->textCursorRect.right + impl->textCursorPosition.x;

		if (impl->textCursorRect.right > e->transformWorld.buildArea.right)
		{
			int d = impl->textCursorRect.right - e->transformWorld.buildArea.right;
			impl->h_scroll += d;
			if (impl->h_scroll > impl->h_scrollLimit)
				impl->h_scroll = impl->h_scrollLimit;
		}
		else if (impl->textCursorRect.left < e->transformWorld.buildArea.left)
		{
			int d = e->transformWorld.buildArea.left - impl->textCursorRect.left;
			impl->h_scroll -= d;
			if (impl->h_scroll < 0.f)
				impl->h_scroll = 0.f;
		}

	}
}

void 
miGUI_onDraw_textinput(mgElement* e)
{
	mgStyle* style = e->userStyle;
	if (!style)
		style = e->window->context->activeStyle;

	mgContext* ctx = e->window->context;
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	int isActive = impl == e->window->context->activeTextInput;

	if (isActive)
	{
		double t = 10.f * ctx->deltaTime;
		if (t > 1.f)
			t = 1.f;
		if (impl->h_scrollCurr != impl->h_scroll)
			impl->h_scrollCurr = lerpd(impl->h_scrollCurr, impl->h_scroll, t);

		/*if (impl->v_scrollCurr != impl->v_scroll)
			impl->v_scrollCurr = lerp(impl->v_scrollCurr, impl->v_scroll, t);*/

		impl->textCursorTimer += ctx->deltaTime;
		if (impl->textCursorTimer > impl->textCursorTimerLimit)
		{
			impl->textCursorTimer = 0.f;
			impl->drawTextCursor = impl->drawTextCursor ? 0 : 1;
		}
	}

	ctx->gpu->setClipRect(&e->transformWorld.clipArea);

	if (impl->drawBG)
	{
		mgColor* c = &style->textInputBGNotActive;

		if (ctx->activeTextInput == impl)
			c = &style->textInputBGActive;

		ctx->gpu->drawRectangle(mgDrawRectangleReason_textInputBG,
			impl, &e->transformWorld.buildArea, c, c, 0, 0);
	}


	if (impl->text && impl->textLen)
	{
		if (impl->multiline)
		{

		}
		else
		{
			impl->textWidth = 0;
			mgPoint pos;
			pos.x = e->transformWorld.buildArea.left - (int)impl->h_scrollCurr;
			pos.y = e->transformWorld.buildArea.top;
			for (uint32_t i = 0; i < impl->textLen; ++i)
			{
				e->window->context->gpu->drawText(
					mgDrawTextReason_textInputDefaultText,
					impl,
					&pos,
					&impl->text[i],
					1,
					&style->textInputDefaultText,
					impl->font);

				if (i == impl->textCursor)
					impl->textCursorPosition = pos;

				if (impl->monospace)
				{
					int v = impl->font->maxSize.x + impl->font->characterSpacing;
					pos.x += v;
					impl->textWidth += v;
				}
				else
				{
					mgPoint p;
					wchar_t t[2] = { impl->text[i], 0 };
					e->window->context->getTextSize(t, impl->font, &p);

					int v = p.x + impl->font->characterSpacing;
					pos.x += p.x + impl->font->characterSpacing;
					impl->textWidth += v;
				}
			}

			if(impl->textCursor == impl->textLen)
				impl->textCursorPosition = pos;
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
		else
		{
			impl->textCursorPosition.x = e->transformWorld.buildArea.left;
			impl->textCursorPosition.y = e->transformWorld.buildArea.top;
		}
	}

	if (impl->drawTextCursor && isActive)
	{
		ctx->gpu->drawRectangle(mgDrawRectangleReason_textInputCursor,
			impl, &impl->textCursorRect,
			&style->textInputCursor,
			&style->textInputCursor,
			0, 0);
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
	impl->canEdit = 1;
	impl->textCursorTimerLimit = 0.5f;
	impl->element = newElement;

	mgTextInputClear_f(impl, 1);

	mgSetParent_f(newElement, 0);
	miGUI_onRebuild_textinput(newElement);

	return newElement;
}

MG_API 
void MG_C_DECL 
mgTextInputSetText_f(struct mgElementTextInput_s* e, const wchar_t* text)
{
	assert(e);

	if (text)
	{
		uint32_t len = wcslen(text);
		if (len)
		{
			if (len > e->allocated)
				mgTextInputSetText_reallocate(e, len);
			mgTextInputClear_f(e, 0);
			for (uint32_t i = 0; i < len; ++i)
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

