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
	mgPoint pt2;
	pt.x = p->rect.left + p->indent.x;
	pt.y = p->rect.top + p->indent.y;
	for (int i = 0; i < p->itemsSize; ++i)
	{
		if (p->items[i].info.type == mgPopupItemType_separator)
		{
			mgRect r;
			r.left = p->rect.left;
			r.top = pt.y;
			r.right = p->rect.right;
			r.bottom = r.top + 5;
			c->gpu->drawRectangle(mgDrawRectangleReason_popupSeparator,
				&r,
				&c->activeStyle->popupSeparator,
				&c->activeStyle->popupSeparator,
				0, 0, 0);
			pt.y += 5;
		}
		else
		{

			if (&p->items[i] == p->nodeUnderCursor)
			{
				c->gpu->drawRectangle(mgDrawRectangleReason_popupHoverElement,
					&p->nodeUnderCursorRect,
					&c->activeStyle->popupHoverElementBG,
					&c->activeStyle->popupHoverElementBG,
					0, 0, 0);
			}

			c->gpu->drawText(mgDrawTextReason_popup, &pt,
				p->items[i].info.text,
				p->items[i].textLen,
				&c->activeStyle->popupText,
				p->font);

			if (p->items[i].info.shortcutText)
			{
				pt2 = pt;
				pt2.x += p->items[i].indentForShortcutText;
				c->gpu->drawText(mgDrawTextReason_popupShortcut, &pt2,
					p->items[i].info.shortcutText,
					p->items[i].shortcutTextLen,
					&c->activeStyle->popupTextShortcut,
					p->font);
			}

			pt.y += p->itemHeight;
		}
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
		for (int i = 0; i < p->itemsSize; ++i)
		{
			r.top = pt.y;
			
			if (p->items[i].info.type == mgPopupItemType_separator)
			{
				r.bottom = r.top + 5;
				pt.y += 5;
			}
			else
			{
				r.bottom = r.top + p->itemHeight;
				if (mgPointInRect(&r, &c->input->mousePosition))
				{
					p->nodeUnderCursor = &p->items[i];
					p->nodeUnderCursorRect = r;
					break;
				}
				pt.y += p->itemHeight;
			}
		}

		if (p->nodeUnderCursor && (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP))
		{
			if (p->nodeUnderCursor->info.callback)
				p->nodeUnderCursor->info.callback();
			mgShowPopup_f(c, 0, 0);
		}
	}
}

MG_API
struct mgPopup_s* MG_C_DECL
mgCreatePopup_f(struct mgPopupItemInfo_s* arr, int arrSize, mgFont* fnt)
{
	assert(arr);
	assert(arrSize>0);

	struct mgPopup_s* newPopup = calloc(1, sizeof(struct mgPopup_s));

	newPopup->font = fnt;
	newPopup->itemsSize = arrSize;
	newPopup->items = calloc(1, sizeof(struct mgPopupItem_s) * arrSize);
	newPopup->indent.x = 5;
	newPopup->indent.y = 5;
	newPopup->textShortcutTextIndent = 20;

	for (int i = 0; i < arrSize; ++i)
	{
		newPopup->items[i].info = arr[i];
		if(newPopup->items[i].info.text)
			newPopup->items[i].textLen = wcslen(newPopup->items[i].info.text);

		if (newPopup->items[i].info.shortcutText)
			newPopup->items[i].shortcutTextLen = wcslen(newPopup->items[i].info.shortcutText);
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
		if (p->items)
			free(p->items);
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
		p->maxTextWidth = 20;
		p->maxShortcutTextWidth = 0;

		int notSeparatorItems = 0;
		int separatorItems = 0;

		if (p->itemsSize)
		{
			mgPoint tsz;
			for (int i = 0; i < p->itemsSize; ++i)
			{			
				if(p->items[i].info.type == mgPopupItemType_separator)
				{
					separatorItems++;
				}
				else 
				{
					notSeparatorItems++;
					if (p->items[i].info.text)
					{
						c->getTextSize(p->items[i].info.text, p->font, &tsz);

						if (tsz.y > p->itemHeight)
							p->itemHeight = tsz.y;

						if (tsz.x > p->maxTextWidth)
							p->maxTextWidth = tsz.x;
					}

					if (p->items[i].info.shortcutText)
					{
						c->getTextSize(p->items[i].info.shortcutText, p->font, &tsz);
						if (tsz.x > p->maxShortcutTextWidth)
							p->maxShortcutTextWidth = tsz.x;
						p->items[i].shortcutTextWidth = tsz.x;
					}
				}
			}
			for (int i = 0; i < p->itemsSize; ++i)
			{
				if (p->items[i].info.type != mgPopupItemType_separator)
				{
					p->items[i].indentForShortcutText = p->maxTextWidth + p->textShortcutTextIndent;
					if (p->items[i].shortcutTextWidth < p->maxShortcutTextWidth)
					{
						p->items[i].indentForShortcutText += p->maxShortcutTextWidth - p->items[i].shortcutTextWidth;
					}
				}
			}

			p->rect.right = p->rect.left + p->maxTextWidth + p->maxShortcutTextWidth + p->textShortcutTextIndent + p->indent.x + p->indent.x;
			p->rect.bottom = p->rect.top + (p->itemHeight * notSeparatorItems) + p->indent.y + p->indent.y;
			p->rect.bottom += 5 * separatorItems;
		}
		else
		{
			p->rect.right = p->rect.left + 20;
			p->rect.bottom = p->rect.top + 20;
		}
	}
}