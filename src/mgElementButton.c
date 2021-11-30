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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

MG_API
void MG_C_DECL
mgSetParent_f(mgElement* object, mgElement* parent);

void
miGUI_onUpdateTransform_button(mgElement* e)
{
	e->buildAreaFinal = e->buildArea;
	e->clipAreaFinal = e->clipArea;
}

void 
miGUI_onUpdate_button(mgElement* e)
{
}

void 
miGUI_onDraw_button(mgElement* e)
{
	mgPoint pos;
	pos.x = e->buildAreaFinal.left;
	pos.y = e->buildAreaFinal.top;

	mgPoint sz;
	sz.x = e->buildAreaFinal.right - e->buildAreaFinal.left;
	sz.y = e->buildAreaFinal.bottom - e->buildAreaFinal.top;

	mgElementButton* impl = (mgElementButton*)e->implementation;
	e->context->gpu->setClipRect(&e->clipAreaFinal);
	e->context->gpu->drawRectangle(e, &pos, &sz, &impl->color1, &impl->color2, 0, 0);

	/*if (impl->text && impl->textLen)
	{
		e->context->gpu->setClipRect(&e->clipAreaFinal);
		e->context->gpu->drawText(&pos, impl->text, impl->textLen, &impl->color, impl->font);
	}*/
}

void
miGUI_onRebuild_button(mgElement* e) {}

MG_API
mgElement* MG_C_DECL
mgCreateButton_f(struct mgContext_s* c, mgPoint* position, mgPoint* size, const wchar_t* text, mgFont* font)
{
	assert(c);
	assert(position);
	assert(text);
	assert(font);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_BUTTON;
	newElement->buildArea.left = position->x;
	newElement->buildArea.top = position->y;
	newElement->buildArea.right = position->x + size->x;
	newElement->buildArea.bottom = position->y + size->y;
	newElement->clipArea = newElement->buildArea;
	newElement->context = c;
	newElement->visible = 1;
	newElement->onDraw = miGUI_onDraw_button;
	newElement->onUpdate = miGUI_onUpdate_button;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_button;
	newElement->onRebuild = miGUI_onRebuild_button;

	newElement->implementation = calloc(1, sizeof(mgElementButton));
	mgElementButton* impl = (mgElementButton*)newElement->implementation;
	mgColorSetAsIntegerRGB(&impl->color1, 0x999999);
	mgColorSetAsIntegerRGB(&impl->color2, 0x666666);

	mgSetParent_f(newElement, 0);

	return newElement;
}
