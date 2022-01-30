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

void
miGUI_onUpdateTransform_rectangle(mgElement* e)
{
	e->transformWorld = e->transformLocal;

	e->transformWorld.buildArea.left += e->parent->transformWorld.buildArea.left;
	e->transformWorld.buildArea.top += e->parent->transformWorld.buildArea.top;
	e->transformWorld.buildArea.right += e->parent->transformWorld.buildArea.left;
	e->transformWorld.buildArea.bottom += e->parent->transformWorld.buildArea.top;

	e->transformWorld.clipArea.left += e->parent->transformWorld.clipArea.left;
	e->transformWorld.clipArea.top += e->parent->transformWorld.clipArea.top;
	e->transformWorld.clipArea.right += e->parent->transformWorld.clipArea.left;
	e->transformWorld.clipArea.bottom += e->parent->transformWorld.clipArea.top;

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
miGUI_onUpdate_rectangle(mgElement* e)
{
	int inRect = mgPointInRect(&e->transformWorld.buildArea, &e->window->context->input->mousePosition);

	if (inRect)
	{
		if (!e->elementState & 0x1)
		{
			if (e->onMouseEnter)
				e->onMouseEnter(e);
		}

		if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
		{
			e->elementState |= 0x2;

			if (e->onClickLMB)
				e->onClickLMB(e);
		}
		else if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			if (e->onReleaseLMB && e->elementState & 0x2)
				e->onReleaseLMB(e);
		}

		if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
		{
			e->elementState |= 0x4;

			if (e->onClickRMB)
				e->onClickRMB(e);
		}
		else if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_RMBUP)
		{
			if (e->onReleaseRMB && e->elementState & 0x4)
				e->onReleaseRMB(e);
		}

		if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN)
		{
			e->elementState |= 0x8;

			if (e->onClickMMB)
				e->onClickMMB(e);
		}
		else if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_MMBUP)
		{
			if (e->onReleaseMMB && e->elementState & 0x8)
				e->onReleaseMMB(e);
		}

		if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_X1MBDOWN)
		{
			e->elementState |= 0x10;

			if (e->onClickX1MB)
				e->onClickX1MB(e);
		}
		else if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_X1MBUP)
		{
			if (e->onReleaseX1MB && e->elementState & 0x10)
				e->onReleaseX1MB(e);
		}

		if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_X2MBDOWN)
		{
			e->elementState |= 0x20;

			if (e->onClickX2MB)
				e->onClickX2MB(e);
		}
		else if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_X2MBUP)
		{
			if (e->onReleaseX2MB && e->elementState & 0x20)
				e->onReleaseX2MB(e);
		}
	}
	else
	{
		if (e->elementState & 0x1)
		{
			if (e->onMouseLeave)
				e->onMouseLeave(e);
		}
	}

	if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
	{
		if (e->elementState & 0x2)
			e->elementState ^= 0x2;
	}

	if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_RMBUP)
	{
		if (e->elementState & 0x4)
			e->elementState ^= 0x4;
	}

	if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_MMBUP)
	{
		if (e->elementState & 0x8)
			e->elementState ^= 0x8;
	}

	if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_X1MBUP)
	{
		if (e->elementState & 0x10)
			e->elementState ^= 0x10;
	}

	if (e->window->context->input->mouseButtonFlags1 & MG_MBFL_X2MBUP)
	{
		if (e->elementState & 0x20)
			e->elementState ^= 0x20;
	}

	if (inRect)
		e->elementState |= 0x1;
	else if(e->elementState & 0x1)
		e->elementState ^= 0x1;
}

void 
miGUI_onDraw_rectangle(mgElement* e)
{
	mgPoint pos;
	pos.x = e->transformWorld.buildArea.left;
	pos.y = e->transformWorld.buildArea.top;

	mgPoint sz;
	sz.x = e->transformWorld.buildArea.right - e->transformWorld.buildArea.left;
	sz.y = e->transformWorld.buildArea.bottom - e->transformWorld.buildArea.top;

	mgElementRectangle* impl = (mgElementRectangle*)e->implementation;

	e->window->context->gpu->setClipRect(&e->transformWorld.clipArea);
	e->window->context->gpu->drawRectangle(mgDrawRectangleReason_rectangle, &pos, &sz, &impl->color1, &impl->color2, 0, 0, 0);
}

void
miGUI_onRebuild_rectangle(mgElement* e) 
{}

MG_API
mgElement* MG_C_DECL
mgCreateRectangle_f(struct mgWindow_s* w, mgPoint* position, mgPoint* size, mgColor* color1, mgColor* color2)
{
	assert(w);
	assert(position);
	assert(size);
	assert(color1);
	assert(color2);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_RECTANGLE;

	newElement->transformLocal.buildArea.left = position->x;
	newElement->transformLocal.buildArea.top = position->y;
	newElement->transformLocal.buildArea.right = position->x + size->x;
	newElement->transformLocal.buildArea.bottom = position->y + size->y;
	newElement->transformLocal.clipArea = newElement->transformLocal.buildArea;
	newElement->transformLocal.sz = *size;
	newElement->window = w;

	newElement->visible = 1;
	newElement->onDraw = miGUI_onDraw_rectangle;
	newElement->onUpdate = miGUI_onUpdate_rectangle;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_rectangle;
	newElement->onRebuild = miGUI_onRebuild_rectangle;

	newElement->implementation = calloc(1, sizeof(mgElementRectangle));
	mgElementRectangle* impl = (mgElementRectangle*)newElement->implementation;
	impl->color1 = *color1;
	impl->color2 = *color2;

	mgSetParent_f(newElement, 0);

	return newElement;
}
