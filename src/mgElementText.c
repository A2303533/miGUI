/*
  Copyright (C) 2021 Basov Artyom
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
miGUI_onUpdateTransform_text(mgElement* e)
{
	miGUI_onUpdateTransform_rectangle(e);
	e->transformLocal.clipArea = e->transformLocal.buildArea;
	e->transformLocal.clipArea.right = e->transformLocal.clipArea.left + e->transformLocal.sz.x;
	e->transformLocal.clipArea.bottom = e->transformLocal.clipArea.top + e->transformLocal.sz.y;
	
	e->transformWorld.clipArea.right = e->transformWorld.clipArea.left + e->transformLocal.sz.x;
	e->transformWorld.clipArea.bottom = e->transformWorld.clipArea.top + e->transformLocal.sz.y;

	if (e->transformWorld.clipArea.left < e->parent->transformWorld.clipArea.left)
		e->transformWorld.clipArea.left = e->parent->transformWorld.clipArea.left;
	if (e->transformWorld.clipArea.top < e->parent->transformWorld.clipArea.top)
		e->transformWorld.clipArea.top = e->parent->transformWorld.clipArea.top;
	if (e->transformWorld.clipArea.right > e->parent->transformWorld.clipArea.right)
		e->transformWorld.clipArea.right = e->parent->transformWorld.clipArea.right;
	if (e->transformWorld.clipArea.bottom > e->parent->transformWorld.clipArea.bottom)
		e->transformWorld.clipArea.bottom = e->parent->transformWorld.clipArea.bottom;
}

void 
miGUI_onUpdate_text(mgElement* e)
{
}

void 
miGUI_onDraw_text(mgElement* e)
{
	mgPoint pos;
	pos.x = e->transformWorld.buildArea.left;
	pos.y = e->transformWorld.buildArea.top;

	mgElementText* impl = (mgElementText*)e->implementation;

	size_t textLen = 0;
	const mgUnicodeChar* text = impl->onText(e, &textLen);
	if (text && textLen)
	{
		e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);

		if (impl->isMultiline)
		{
			mgUnicodeChar* b = &text[0];
			mgUnicodeChar* e2 = b + textLen;
			mgPoint ts;
			size_t tl = 0;
			while (1)
			{
				mgUnicodeChar* t2 = b;
				while (1)
				{
					++t2;
					++tl;

					if (*t2 == 0x20)
					{
						break;
					}

					if (*t2 == 0)
						break;
				}

				impl->textProcessor->onGetTextSize(mgDrawTextReason_text,
					impl->textProcessor,
					b,
					tl,
					&ts);

				mgPoint pos2 = pos;
				pos2.x += ts.x;
				if (pos2.x > e->transformWorld.clipArea.right)
				{
					pos.y += ts.y;
					pos.x = e->transformWorld.buildArea.left;

					e->transformLocal.sz.y += ts.y;
					miGUI_onUpdateTransform_text(e);
				}

				impl->textProcessor->onDrawText(
					mgDrawTextReason_text,
					impl->textProcessor,
					&pos,
					b,
					tl,
					0);

				pos.x += ts.x;

				b += tl;

				tl = 0;
				if (b >= e2)
					break;
			}
		}
		else
		{
			impl->textProcessor->onDrawText(
				mgDrawTextReason_text,
				impl->textProcessor,
				&pos,
				text,
				textLen,
				0);
		}
	}
}

void
miGUI_onRebuild_text(mgElement* e) {
	mgElementText* impl = (mgElementText*)e->implementation;
	size_t textLen = 0;
	const mgUnicodeChar* text = impl->onText(e, &textLen);
	if (text && textLen)
	{
		mgPoint p;
		//e->window->context->getTextSize(text, impl->onFont, &p);
		impl->textProcessor->onGetTextSize(
			mgDrawTextReason_text,
			impl->textProcessor,
			text,
			textLen,
			&p);
		e->transformLocal.sz = p;

		miGUI_onUpdateTransform_rectangle(e);
	}
}

mgElement* MG_C_DECL
mgCreateText(struct mgWindow_s* w, mgPoint* position, struct mgTextProcessor_s* tp)
{
	assert(w);
	assert(position);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_TEXT;
	
	newElement->transformLocal.buildArea.left = position->x;
	newElement->transformLocal.buildArea.top = position->y;
	newElement->transformLocal.buildArea.right = position->x;
	newElement->transformLocal.buildArea.bottom = position->y;
	newElement->transformLocal.clipArea = newElement->transformLocal.buildArea;
	newElement->creationRect = newElement->transformLocal.buildArea;
	
	newElement->window = w;
	mgElementDefaultInit(newElement);
	newElement->onDraw = miGUI_onDraw_text;
	newElement->onUpdate = miGUI_onUpdate_text;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_text;
	newElement->onRebuild = miGUI_onRebuild_text;

	newElement->implementation = calloc(1, sizeof(mgElementText));
	mgElementText* impl = (mgElementText*)newElement->implementation;
	impl->textProcessor = tp;
	mgSetParent(newElement, 0);

	// now user must call rebuild function (or just context->needRebuild = 1;)
	//miGUI_onRebuild_text(newElement);

	return newElement;
}
