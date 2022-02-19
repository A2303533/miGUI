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

#include <wchar.h>

void
mgDrawPopup(struct mgContext_s* c, mgPopup* p)
{
	c->gpu->setClipRect(&p->rect);
	c->gpu->drawRectangle(mgDrawRectangleReason_popupBG, 
		&p->rect,
		&c->activeStyle->popupBG,
		&c->activeStyle->popupBG,
		0, 0, 0);


	mgPoint pt;
	pt.x = p->rect.left + p->indent.x;
	pt.y = p->rect.top + p->indent.y;
	for (int i = 0; i < p->nodesSize; ++i)
	{
		if (&p->nodes[i] == p->nodeUnderCursor)
		{
			c->gpu->drawRectangle(mgDrawRectangleReason_popupHoverElement,
				&p->nodeUnderCursorRect,
				&c->activeStyle->popupHoverElementBG,
				&c->activeStyle->popupHoverElementBG,
				0, 0, 0);
		}

		c->gpu->drawText(mgDrawTextReason_popup, &pt,
			p->nodes[i].text,
			p->nodes[i].textLen,
			&c->activeStyle->popupText,
			p->font);
		pt.y += p->itemHeight;
	}
}

void
mgUpdatePopup(struct mgContext_s* c, mgPopup* p)
{
	if (p->rect.right > c->windowSize.x)
	{
		int v = p->rect.right - c->windowSize.x;
		p->rect.left -= v;
		p->rect.right -= v;
	}

	if (p->rect.bottom > c->windowSize.y)
	{
		int v = p->rect.bottom - c->windowSize.y;
		p->rect.top -= v;
		p->rect.bottom -= v;
	}

	if (mgPointInRect(&p->rect, &c->input->mousePosition))
	{
		c->cursorInPopup = 1;
		p->nodeUnderCursor = 0;

		mgPoint pt;
		pt.x = p->rect.left + p->indent.x;
		pt.y = p->rect.top + p->indent.y;
		mgRect r;
		r.left = pt.x - p->indent.x;
		r.right = p->rect.right;
		for (int i = 0; i < p->nodesSize; ++i)
		{
			r.top = pt.y;
			r.bottom = r.top + p->itemHeight;
			if (mgPointInRect(&r, &c->input->mousePosition))
			{
				p->nodeUnderCursor = &p->nodes[i];
				p->nodeUnderCursorRect = r;
				break;
			}
			pt.y += p->itemHeight;
		}

		if (p->nodeUnderCursor && (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP))
		{
			if (p->nodeUnderCursor->callback)
				p->nodeUnderCursor->callback();
			mgShowPopup_f(c, 0, 0);
		}
	}
}

MG_API
struct mgPopup_s* MG_C_DECL
mgCreatePopup_f(struct mgPopupNode_s* arr, int arrSize, mgFont* fnt)
{
	assert(arr);
	assert(arrSize>0);

	struct mgPopup_s* newPopup = calloc(1, sizeof(struct mgPopup_s));

	newPopup->font = fnt;
	newPopup->nodesSize = arrSize;
	newPopup->nodes = calloc(1, sizeof(struct mgPopupNode_s) * arrSize);
	newPopup->indent.x = 5;
	newPopup->indent.y = 5;

	for (int i = 0; i < arrSize; ++i)
	{
		newPopup->nodes[i] = arr[i];
		newPopup->nodes[i].textLen = wcslen(newPopup->nodes[i].text);
	}

	return newPopup;
}

MG_API
void MG_C_DECL
mgDestroyPopup_f(struct mgPopup_s* p)
{
	assert(p);
	if (p)
	{
		if (p->nodes)
			free(p->nodes);
		free(p);
	}
}

MG_API
void MG_C_DECL
mgShowPopup_f(struct mgContext_s* c, struct mgPopup_s* p, mgPoint* position)
{
	assert(c);
	
	c->activePopup = p;

	if (p)
	{
		p->nodeUnderCursor = 0;
		
		p->rect.left = position->x + p->indent.x;
		p->rect.top = position->y + p->indent.y;
		
		p->itemHeight = 0;
		int maxWidth = 20;

		if (p->nodesSize)
		{
			mgPoint tsz;
			for (int i = 0; i < p->nodesSize; ++i)
			{
				if (p->nodes[i].text)
				{
					c->getTextSize(p->nodes[i].text, p->font, &tsz);
					
					if (tsz.y > p->itemHeight)
						p->itemHeight = tsz.y;
					
					if (tsz.x > maxWidth)
						maxWidth = tsz.x;
				}
			}

			p->rect.right = p->rect.left + maxWidth + p->indent.x + p->indent.x;
			p->rect.bottom = p->rect.top + (p->itemHeight * p->nodesSize) + p->indent.y + p->indent.y;
		}
		else
		{
			p->rect.right = p->rect.left + 20;
			p->rect.bottom = p->rect.top + 20;
		}
	}
}