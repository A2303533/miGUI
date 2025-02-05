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
#include <stdio.h>

#ifdef MG_PLATFORM_WINDOWS
#include <Windows.h>
#endif

inline double
lerpd(double v0, double v1, double t)
{
	return (1.0 - t) * v0 + t * v1;
}
void miGUI_onUpdateTransform_rectangle(mgElement* e);
void miGUI_onUpdate_rectangle(mgElement* e);
void mgTextInput_updateScrollLimit(mgElement* e);
void mgTextInput_selectAll(struct mgElementTextInput_s* e);
struct mgPopup_s* mgGetDefaultPopupTextInput(mgContext* c, struct mgTextProcessor_s* tp);

extern int g_skipFrame;

#define STRING2(x) #x
#define STRING(x) STRING2(x)

void
mgTextInput_copy(mgElement* e)
{
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	if (!impl->isSelected)
		return;

	uint32_t s1 = impl->selectionStart;
	uint32_t s2 = impl->selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = impl->selectionStart;
	}
	uint32_t num_to_select = s2 - s1;

#ifdef MG_PLATFORM_WINDOWS
	if (!OpenClipboard(0))
		return;

	uint32_t len = num_to_select;
	EmptyClipboard();
	HGLOBAL clipbuffer;
	
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, ((len + len) + 1) * sizeof(WCHAR));

	wchar_t* buffer;
	buffer = (wchar_t*)GlobalLock(clipbuffer);

	memset(buffer,0, (len + len + 1) * sizeof(wchar_t));
	wchar_t* wchar_ptr = buffer;
	//memcpy(buffer, &impl->text[s1], len * sizeof(wchar_t));
	union mgUnicodeUC uc;
	for (size_t i = 0; i < len; ++i)
	{
		mgUnicodeChar c = impl->text[s1 + i];

		if (c >= 0x32000)
			c = '?';

		uc.integer = mgGetUnicodeTable()[c].m_utf16;

		if (uc.shorts[1])
		{
			*wchar_ptr = uc.shorts[1];
			++wchar_ptr;
		}
		if (uc.shorts[0])
		{
			*wchar_ptr = uc.shorts[0];
			++wchar_ptr;
		}
	}
	*wchar_ptr = 0;
	++wchar_ptr;

	buffer[len] = 0;

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_UNICODETEXT, clipbuffer);
	CloseClipboard();
#else
#error Need implementation....
#endif
}
void
mgTextInput_cut(mgElement* e)
{
	mgTextInput_copy(e);
	mgTextInputDeleteSelected(e->implementation);
}
void
mgTextInput_paste(mgElement* e)
{
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	if(impl->isSelected)
		mgTextInputDeleteSelected(e->implementation);

#ifdef MG_PLATFORM_WINDOWS
	if (!OpenClipboard(0))
		return;

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData)
	{
		wchar_t* buffer = (wchar_t*)GlobalLock(hData);
		if (buffer)
		{
			uint32_t len = (uint32_t)wcslen(buffer);
			if (len)
			{
				mgUnicodeChar* utf32 = malloc(len * sizeof(mgUnicodeChar));
				size_t utf32sz = 0;
				mgUnicodeFromUTF16(buffer, utf32, &utf32sz);

				if(impl->onPaste(e, utf32, utf32sz))
					mgTextInputPutText(impl, utf32, utf32sz);
				free(utf32);
			}
		}
		GlobalUnlock(hData);
	}
	CloseClipboard();
#else
#error Need implementation....
#endif
	if (impl->multiline)
	{

	}
	else
	{
		mgPoint tsz;
		//e->window->context->getTextSize(impl->text, impl->onFont, &tsz);
		impl->textProcessor->onGetTextSize(
			mgDrawTextReason_textInput,
			e,
			impl->textProcessor,
			impl->text,
			impl->textLen,
			&tsz);

		impl->textWidth = tsz.x;
	}

}

void
mgTextInput_defaultPopup_onCut(int id, struct mgPopupItem_s* item)
{
	mgTextInput_cut(item->info.userData);
}
void
mgTextInput_defaultPopup_onCopy(int id, struct mgPopupItem_s* item)
{
	mgTextInput_copy(item->info.userData);
}
void
mgTextInput_defaultPopup_onPaste(int id, struct mgPopupItem_s* item)
{
	mgTextInput_paste(item->info.userData);
}
void
mgTextInput_defaultPopup_onDelete(int id, struct mgPopupItem_s* item)
{
	mgTextInputDeleteSelected(((mgElement*)item->info.userData)->implementation);
}
void
mgTextInput_defaultPopup_onSelectAll(int id, struct mgPopupItem_s* item)
{
	mgTextInput_selectAll(((mgElement*)item->info.userData)->implementation);
}

void
mgTextInput_defaultPopupOnShow(struct mgContext_s* c, struct mgPopup_s* p)
{
	if (!c->textInputActive)
		return;

	p->items[0].info.isEnabled = c->textInputActive->isSelected ? 1 : 0;/*cut*/
	p->items[1].info.isEnabled = c->textInputActive->isSelected ? 1 : 0;/*copy*/
	p->items[3].info.isEnabled = c->textInputActive->isSelected ? 1 : 0;/*delete*/

	p->items[0].info.callback = mgTextInput_defaultPopup_onCut;
	p->items[1].info.callback = mgTextInput_defaultPopup_onCopy;
	p->items[2].info.callback = mgTextInput_defaultPopup_onPaste;
	p->items[3].info.callback = mgTextInput_defaultPopup_onDelete;
	p->items[4].info.callback = mgTextInput_defaultPopup_onSelectAll;
	
	p->items[0].info.userData = c->textInputActive->element;
	p->items[1].info.userData = c->textInputActive->element;
	p->items[2].info.userData = c->textInputActive->element;
	p->items[3].info.userData = c->textInputActive->element;
	p->items[4].info.userData = c->textInputActive->element;
}

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
	mgUnicodeChar* newBuffer = malloc((newSize + 1) * sizeof(mgUnicodeChar));
	for (uint32_t i = 0; i < e->textLen; ++i)
	{
		newBuffer[i] = e->text[i];
	}
	newBuffer[e->textLen] = 0;
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
mgTextInput_selectAll(struct mgElementTextInput_s* e)
{
	if (!e->textLen)
		return;

	e->selectionStart = 0;
	e->selectionEnd = e->textLen;
	e->isSelected = 1;
	e->textCursor = e->textLen;
}

void
miGUI_textinput_activate(mgElement* e, int is)
{
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	impl->drawTextCursor = 0;
	impl->textCursorTimer = 0.f;
	if (is)
	{
		e->window->context->textInputActive = impl;
		if (impl->onActivate)
			impl->onActivate(e);
	}
	else
	{
		e->window->context->textInputActive = 0;
		impl->h_scrollCurr = 0.f;
		impl->h_scroll = 0.f;
	}
//	g_skipFrame = 1; // remove?
}

void MG_C_DECL 
mgTextInputActivate(struct mgContext_s* c, struct mgElementTextInput_s* e, int isActive, int deactivateCode)
{
	assert(c);
	assert(e);
	if (isActive)
	{
		if(c->textInputActive)
			miGUI_textinput_activate(e->element, 0);

		miGUI_textinput_activate(e->element, 1);
	}
	else
	{
		if (e->onEndEdit)
		{
			if (e->onEndEdit(e->element, deactivateCode))
				miGUI_textinput_activate(e->element, 0);
		}
	}
}

void 
mgTextInputDeselect(struct mgElementTextInput_s* e)
{
	e->selectionStart = 0;
	e->selectionEnd = 0;
	e->isSelected = 0;
}

void MG_C_DECL 
mgTextInputDeleteSelected(struct mgElementTextInput_s* e)
{
	if (!e->isSelected)
		return;

	uint32_t s1 = e->selectionStart;
	uint32_t s2 = e->selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = e->selectionStart;
	}

	uint32_t num_to_delete = s2 - s1;
	uint32_t str_len = e->textLen;
	mgUnicodeChar * buf = e->text;
	for (size_t i = s2, i2 = s1; i < str_len; ++i, ++i2)
	{
		buf[i2] = buf[i];
	}
	buf[str_len - num_to_delete] = 0;
	e->textCursor = s1;
	e->textLen -= num_to_delete;
	mgTextInputDeselect(e);
}

void
mgTextInput_backspace(struct mgElementTextInput_s* e)
{
	if (e->isSelected)
	{
		mgTextInputDeleteSelected(e);
		return;
	}
	
	int ok = 0;
	mgUnicodeChar* buf = e->text;
	size_t str_len = e->textLen;
	for (size_t i = e->textCursor; i < str_len; ++i)
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
		mgTextInputDeleteSelected(e);
		return;
	}

	if (e->textCursor < e->textLen)
	{
		int ok = 0;
		mgUnicodeChar* buf = e->text;
		size_t str_len = e->textLen;
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

int MG_C_DECL
mgTextInputPutText(struct mgElementTextInput_s* e, const mgUnicodeChar* text, size_t len)
{
	int w = 0;
	
	mgPoint pt;
	//e->element->window->context->getTextSize(text, e->onFont, &pt);
	e->textProcessor->onGetTextSize(
		mgDrawTextReason_textInput,
		e->element,
		e->textProcessor,
		text,
		len,
		&pt);
	
	w = pt.x;

	size_t newLen = e->textLen + len;

	if (e->charLimit)
	{
		if (newLen > e->charLimit)
		{
			size_t dif = newLen - e->charLimit;
			len -= dif;
			newLen = e->charLimit;

			if (!len)
				return w;
		}
	}

	if (newLen > e->allocated)
		mgTextInputSetText_reallocate(e, newLen);

	size_t i = e->textLen;
	while (i >= e->textCursor)
	{
		size_t next = i + len;
		if (next < e->allocated)
			e->text[next] = e->text[i];

		if (i == 0)
			break;
		--i;
	}
	for (size_t i2 = 0; i2 < len; ++i2)
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
	mgContext* ctx = impl->element->window->context;
	if (impl->textCursor)
	{
		if (ctx->input->keyboardModifier == MG_KBMOD_SHIFT)
		{
			if ((impl->selectionStart == 0) && (impl->selectionEnd == 0))
			{
				impl->isSelected = 1;
				impl->selectionStart = impl->textCursor;
			}
			impl->selectionEnd = impl->textCursor - 1;
		}
		else
		{
			if (impl->isSelected)
			{
				if (impl->selectionStart < impl->selectionEnd)
					impl->textCursor = impl->selectionStart + 1;
				else
					++impl->textCursor;
				mgTextInputDeselect(impl);
			}
		}
		--impl->textCursor;
	}
	else
	{
		if(impl->isSelected && ctx->input->keyboardModifier != MG_KBMOD_SHIFT)
			mgTextInputDeselect(impl);
	}
}

void
miGUI_textinput_goRight(mgElementTextInput* impl)
{
	mgContext* ctx = impl->element->window->context;
	if (impl->textCursor < impl->textLen)
	{
		if (ctx->input->keyboardModifier == MG_KBMOD_SHIFT)
		{
			if ((impl->selectionStart == 0) && (impl->selectionEnd == 0))
			{
				impl->isSelected = 1;
				impl->selectionStart = impl->textCursor;
			}
			impl->selectionEnd = impl->textCursor + 1;
		}
		else
		{
			if (impl->isSelected)
			{
				if (impl->selectionStart > impl->selectionEnd)
					impl->textCursor = impl->selectionStart - 1;
				else
					--impl->textCursor;
				mgTextInputDeselect(impl);
			}
		}
		++impl->textCursor;
		if (impl->textCursor > impl->textLen)
			impl->textCursor = impl->textLen;
	}
	else
	{
		if (impl->isSelected && ctx->input->keyboardModifier != MG_KBMOD_SHIFT)
			mgTextInputDeselect(impl);
	}
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
miGUI_textinput_goHomeOfText(mgElementTextInput* impl)
{
	mgContext* ctx = impl->element->window->context;
	if (ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
	{
		if ((impl->selectionStart == 0) && (impl->selectionEnd == 0))
		{
			impl->isSelected = 1;
			impl->selectionStart = impl->textCursor;
		}
	}
	else
	{
		if (impl->isSelected)
			mgTextInputDeselect(impl);
	}
	impl->textCursor = 0;

	if (ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
		impl->selectionEnd = impl->textCursor;
}

void
miGUI_textinput_goHome(mgElementTextInput* impl)
{
	mgContext* ctx = impl->element->window->context;
	if (ctx->input->keyboardModifier == MG_KBMOD_CTRL
		|| ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
	{
		miGUI_textinput_goHomeOfText(impl);
		return;
	}
	if (ctx->input->keyboardModifier == MG_KBMOD_SHIFT
		|| ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
	{
		if ((impl->selectionStart == 0) && (impl->selectionEnd == 0))
		{
			impl->isSelected = 1;
			impl->selectionStart = impl->textCursor;
		}
	}
	else 
	{
		if (impl->isSelected)
			mgTextInputDeselect(impl);
	}

	if (impl->multiline)
	{
	}
	else
	{
		impl->textCursor = 0;
	}

	if (ctx->input->keyboardModifier == MG_KBMOD_SHIFT
		|| ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
		impl->selectionEnd = impl->textCursor;
}

void
miGUI_textinput_goEndOfText(mgElementTextInput* impl)
{
	mgContext* ctx = impl->element->window->context;
	if (ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
	{
		if ((impl->selectionStart == 0) && (impl->selectionEnd == 0))
		{
			impl->isSelected = 1;
			impl->selectionStart = impl->textCursor;
		}
	}
	else
	{
		if (impl->isSelected)
			mgTextInputDeselect(impl);
	}

	impl->textCursor = impl->textLen;

	if (ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
		impl->selectionEnd = impl->textCursor;
}

void
miGUI_textinput_goEnd(mgElementTextInput* impl)
{
	mgContext* ctx = impl->element->window->context;
	if (ctx->input->keyboardModifier == MG_KBMOD_CTRL
		|| ctx->input->keyboardModifier == MG_KBMOD_CTRLSHIFT)
	{
		miGUI_textinput_goEndOfText(impl);
		return;
	}
	
	if (ctx->input->keyboardModifier == MG_KBMOD_SHIFT)
	{
		if ((impl->selectionStart == 0) && (impl->selectionEnd == 0))
		{
			impl->isSelected = 1;
			impl->selectionStart = impl->textCursor;
		}
	}
	else
	{
		if (impl->isSelected)
			mgTextInputDeselect(impl);
	}

	if (impl->multiline)
	{
	}
	else
	{
		impl->textCursor = impl->textLen;
	}
	if (ctx->input->keyboardModifier == MG_KBMOD_SHIFT)
		impl->selectionEnd = impl->textCursor;
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
}

void 
miGUI_onUpdate_textinput(mgElement* e)
{
	mgContext* c = e->window->context;

	e->cursorInRect = mgPointInRect(&e->transformWorld.clipArea, &c->input->mousePosition);
	mgElementTextInput* impl = (mgElementTextInput*)e->implementation;
	
	if (e->cursorInRect)
	{
		c->textInputUnderCursor = impl;

		mgSetCursor(c, c->defaultCursors[mgCursorType_IBeam], mgCursorType_Arrow);

		if (e->enabled)
		{
			if ((c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				|| (c->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
				|| (c->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN))
			{
				if(impl != e->window->context->textInputActive)
					miGUI_textinput_activate(e, 1);
			}
			
		}
	}
	else
	{
		if (e->elementState & 0x1)
			mgSetCursor(e->window->context, e->window->context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);

		/*if (e->enabled)
		{
			if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				if (impl->onEndEdit)
				{
					if(impl->onEndEdit(e, 2))
						miGUI_textinput_activate(e, 0);
				}
				else
				{
					miGUI_textinput_activate(e, 0);
				}
			}
		}*/
	}

	miGUI_onUpdate_rectangle(e);

	int isActive = impl == e->window->context->textInputActive;
	if (isActive)
	{
		mgTextInput_updateScrollLimit(e);
		
		//mgFont* fnt = impl->onFont(c->input->character, 0);
		mgFont* fnt = impl->textProcessor->onFont(
			mgDrawTextReason_textInput, 
			e,
			impl->textProcessor, c->input->character);

		if (c->input->character)
		{
			if (c->input->keyboardModifier != MG_KBMOD_CTRL)
			{
				switch (c->input->character)
				{
				case MG_KEY_ESCAPE:
				{
					if (!impl->multiline)
					{
						if (impl->isSelected)
						{
							mgTextInputDeselect(impl);
						}
						else
						{
							if (impl->onEndEdit)
							{
								if(impl->onEndEdit(e, 3))
									miGUI_textinput_activate(e, 0);
							}
							else
							{
								miGUI_textinput_activate(e, 0);
							}
						}
						return;
					}
				}break;
				case MG_KEY_ENTER:
				{
					if (!impl->multiline)
					{
						if (impl->onEndEdit)
						{
							if (impl->onEndEdit(e, 1))
								miGUI_textinput_activate(e, 0);
						}
						else
						{
							miGUI_textinput_activate(e, 0);
						}
						return;
					}
				}break;
				case MG_KEY_BACKSPACE:
					mgTextInput_backspace(impl);
					break;

				default:
				{
					if (impl->onCharEnter)
						c->input->character = impl->onCharEnter(e, c->input->character);

					if (c->input->character)
					{
						if (impl->isSelected)
							mgTextInputDeleteSelected(impl);

						mgUnicodeChar t[2] = { c->input->character, 0 };
						int w = mgTextInputPutText(impl, t, 1);
						
						impl->textWidth += w + fnt->characterSpacing;
					}
				}break;
				}
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

		if (c->input->keyboardModifier == MG_KBMOD_CTRL)
		{
			if (mgIsKeyHit(c->input, MG_KEY_A))
				mgTextInput_selectAll(impl);
			else if (mgIsKeyHit(c->input, MG_KEY_C))
				mgTextInput_copy(e);
			else if (mgIsKeyHit(c->input, MG_KEY_INSERT))
				mgTextInput_copy(e);
			else if (mgIsKeyHit(c->input, MG_KEY_X))
				mgTextInput_cut(e);
			else if (mgIsKeyHit(c->input, MG_KEY_V))
				mgTextInput_paste(e);
		}
		
		if (c->input->keyboardModifier == MG_KBMOD_SHIFT)
		{
			if (mgIsKeyHit(c->input, MG_KEY_INSERT))
				mgTextInput_paste(e);
			else if (mgIsKeyHit(c->input, MG_KEY_DELETE))
				mgTextInput_cut(e);
		}

		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
		{
			impl->textCursor = impl->charIndexUnderCursor;
			impl->textCursorTimer = 0.f;
			impl->drawTextCursor = 1;
			if (c->input->keyboardModifier != MG_KBMOD_SHIFT)
			{
				mgTextInputDeselect(impl);
				impl->selectionStart = impl->textCursor;
				impl->selectionEnd = impl->textCursor;
			}
			else
			{
				impl->selectionEnd = impl->textCursor;
				if (impl->selectionEnd != impl->selectionStart)
					impl->isSelected = 1;
				else
					impl->isSelected = 0;
			}
		}

		if (c->input->mouseButtonFlags2 & MG_MBFL_LMBHOLD)
		{
			if (c->input->mouseMoveDelta.x || c->input->mouseMoveDelta.y)
			{
				impl->textCursor = impl->charIndexUnderCursor;
				impl->selectionEnd = impl->textCursor;
				impl->isSelected = 1;
			}
		}

		if (e->lmbClickCount == 2)
			mgTextInput_selectAll(impl);
		

		impl->textCursorRect.left = 0;
		impl->textCursorRect.top = e->transformWorld.buildArea.top;
		impl->textCursorRect.right = impl->textCursorRect.left + 2;
		impl->textCursorRect.bottom = impl->textCursorRect.top + fnt->maxSize.y;
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
		
		if (c->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
		{
			if (impl->rightClickPopup)
			{
				if (!impl->rightClickPopup->onShow)
					impl->rightClickPopup->onShow = mgTextInput_defaultPopupOnShow;
				mgShowPopup(c, impl->rightClickPopup, &c->input->mousePosition);
			}
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
	int isActive = impl == e->window->context->textInputActive;

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

	if (e->drawBG)
	{
		mgColor* c = &style->textInputBGNotActive;

		if (ctx->textInputActive == impl)
			c = &style->textInputBGActive;

		ctx->gpu->drawRectangle(mgDrawRectangleReason_textInputBG,
			impl, &e->transformWorld.buildArea, c, c, 0, 0);
	}

	impl->charIndexUnderCursor = 0;
	
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
			mgRect rect;
			mgColor* textColor = 0;
			for (uint32_t i = 0; i < impl->textLen; ++i)
			{
				mgUnicodeChar t[2] = { impl->text[i], 0 };

				//mgFont* fnt = impl->onFont(ch1, ch2);
				mgFont* fnt = impl->textProcessor->onFont(mgDrawTextReason_textInput, e, impl->textProcessor, impl->text[i]);

				rect.left = pos.x;
				rect.top = pos.y;				

				mgPoint pos2 = pos;

				if (impl->monospace)
				{
					int v = fnt->maxSize.x + fnt->characterSpacing;
					pos.x += v;
					impl->textWidth += v;
				}
				else
				{
					mgPoint p;
					//e->window->context->getTextSize(t, impl->font, &p);
					impl->textProcessor->onGetTextSize(
						mgDrawTextReason_textInput,
						e,
						impl->textProcessor,
						t,
						1,
						&p);

					int v = p.x + fnt->characterSpacing;
					pos.x += p.x + fnt->characterSpacing;
					impl->textWidth += v;
				}
				rect.right = pos.x;
				rect.bottom = pos.y + fnt->maxSize.y;

				textColor = &style->textInputText;
				if (impl->isSelected)
				{
					uint32_t start = impl->selectionStart;
					uint32_t end = impl->selectionEnd;
					if (start > end)
					{
						end = impl->selectionStart;
						start = impl->selectionEnd;
					}

					if ((i >= start) && (i < end))
					{
						mgColor* selcol = &style->textInputSelectionBGNotActive;
						if (isActive)
							selcol = &style->textInputSelectionBGActive;

						e->window->context->gpu->drawRectangle(
							mgDrawRectangleReason_textInputSelectionBG,
							impl, &rect, selcol, selcol, 0, 0);
						textColor = &style->textInputTextSelected;
					}
				}

				/*e->window->context->gpu->drawText(
					mgDrawTextReason_textInputDefaultText,
					impl,
					&pos2,
					&impl->text[i],
					1,
					impl->onColor,
					impl->onFont);*/
					/*textColor,
					impl->font);*/
				impl->textProcessor->onDrawText(
					mgDrawTextReason_textInputDefaultText,
					e,
					impl->textProcessor,
					&pos2,
					&impl->text[i],
					1,
					0);

				{
					uint32_t rw = rect.right - rect.left;
					uint32_t halfw = rw / 2;
					mgRect r1, r2;
					r1 = rect;
					r2 = rect;
					r1.right = r1.right - halfw;
					r2.left = r2.left + halfw;
					
					if (e->window->context->input->mousePosition.x >= r1.left
						&& e->window->context->input->mousePosition.x < r1.right)
						impl->charIndexUnderCursor = i;
					else if (e->window->context->input->mousePosition.x >= r2.left
						&& e->window->context->input->mousePosition.x < r2.right)
						impl->charIndexUnderCursor = i+1;
				}

				if (i == impl->textCursor)
					impl->textCursorPosition = pos2;
			}

			if (!impl->charIndexUnderCursor)
			{
				if (e->window->context->input->mousePosition.x > pos.x)
					impl->charIndexUnderCursor = impl->textLen;
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

			/*e->window->context->gpu->drawText(
				mgDrawTextReason_textInputDefaultText,
				impl, 
				&pos,
				impl->defaultText, 
				impl->defaultTextLen,
				&style->textInputDefaultText,
				impl->font);*/
			impl->textProcessor->onDrawText(
				mgDrawTextReason_textInputDefaultText,
				e,
				impl->textProcessor,
				&pos,
				impl->defaultText,
				impl->defaultTextLen,
				&style->textInputDefaultText);
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
	//mgElementText* impl = (mgElementText*)e->implementation;
	//if (impl->text && e->window->context->getTextSize)
	//{
		/*mgPoint p;
		e->window->context->getTextSize(impl->text, impl->font, &p);
		e->transformLocal.sz = p;
		miGUI_onUpdateTransform_rectangle(e);*/
	//}
}

mgElement* MG_C_DECL
mgCreateTextInput(struct mgWindow_s* w, mgPoint* position, mgPoint* size, struct mgTextProcessor_s* tp)
{
	assert(w);
	assert(position);
	assert(size);
	//assert(font);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_TEXTINPUT;
	
	newElement->transformLocal.buildArea.left = position->x;
	newElement->transformLocal.buildArea.top = position->y;
	newElement->transformLocal.buildArea.right = position->x + size->x;
	newElement->transformLocal.buildArea.bottom = position->y + size->y;
	newElement->transformLocal.clipArea = newElement->transformLocal.buildArea;
	newElement->creationRect = newElement->transformLocal.buildArea;
	
	mgElementDefaultInit(newElement);

	newElement->window = w;
	newElement->onDraw = miGUI_onDraw_textinput;
	newElement->onUpdate = miGUI_onUpdate_textinput;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_textinput;
	newElement->onRebuild = miGUI_onRebuild_textinput;

	newElement->implementation = calloc(1, sizeof(mgElementTextInput));
	mgElementTextInput* impl = (mgElementTextInput*)newElement->implementation;
	//impl->font = font;
	impl->textProcessor = tp;
	impl->defaultText = U"Click...";
	impl->defaultTextLen = 9;
	impl->canSelect = 1;
	impl->textCursorTimerLimit = 0.5f;
	impl->element = newElement;
	impl->rightClickPopup = mgGetDefaultPopupTextInput(w->context, tp);
	
	mgTextInputClear(impl, 1);

	mgSetParent(newElement, 0);
	miGUI_onRebuild_textinput(newElement);

	return newElement;
}

void MG_C_DECL 
mgTextInputSetText(struct mgElementTextInput_s* e, const mgUnicodeChar* text)
{
	assert(e);

	if (text)
	{
		//uint32_t len = (uint32_t)wcslen(text);
		size_t len = mgUnicodeStrlen(text);
		if (len)
		{
			if (len > e->allocated)
				mgTextInputSetText_reallocate(e, len);
			mgTextInputClear(e, 0);
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

void MG_C_DECL 
mgTextInputClear(struct mgElementTextInput_s* e, int freeMemory)
{
	if (freeMemory)
	{
		if (e->text)
			free(e->text);

		e->allocated = 10;
		e->text = malloc((e->allocated + 1) * sizeof(mgUnicodeChar));
	}

	e->textLen = 0;
	e->text[0] = 0;
}

void
mgTextInputFree(struct mgElementTextInput_s* e)
{
	if (e->text)
		free(e->text);
	e->allocated = 0;
	e->text = 0;
	e->textLen = 0;
}
