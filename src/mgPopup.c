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
mgPopupFixPosition(struct mgContext_s* c, mgPopup* p)
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

	if (p->parent)
	{
		if ((p->rect.left < p->parent->rect.right)
			&& (p->rect.left > p->parent->rect.left))
		{
			int v = p->rect.left - p->parent->rect.left;
			v += p->rect.right - p->rect.left;
			p->rect.left -= v;
			p->rect.right -= v;
		}
	}
}

void
mgPopupHideSub(struct mgPopup_s* p)
{
	p->subVisible = 0;

	for (int i = 0; i < p->itemsSize; ++i)
	{
		if (p->items[i].info.subMenu)
			mgPopupHideSub(p->items[i].info.subMenu);
	}
}

void
mgPopupSetPosition(struct mgContext_s* c, mgPopup* p, mgPoint* position)
{
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
				if (p->items[i].info.type == mgPopupItemType_separator)
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

			p->rect.right = p->rect.left
				+ p->maxTextWidth
				+ p->maxShortcutTextWidth
				+ p->textShortcutTextIndent
				+ p->iconLeftWidth
				+ p->indent.x
				+ p->indent.x
				+ p->iconRightWidth;

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
	pt.x = p->rect.left + p->indent.x + p->iconLeftWidth;
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

			if (p->items[i].info.subMenu)
			{
				mgIconGroup* iconGroup = c->defaultIconGroup;
				int iconID = iconGroup->popupNext;
				c->currentIcon.left =  iconGroup->icons->iconNodes[iconID].lt.x;
				c->currentIcon.top = iconGroup->icons->iconNodes[iconID].lt.y;
				c->currentIcon.right = iconGroup->icons->iconNodes[iconID].sz.x;
				c->currentIcon.bottom = iconGroup->icons->iconNodes[iconID].sz.y;

				mgColor wh;
				mgColorSet(&wh, 1.f, 1.f, 1.f, 1.f);


				mgRect r;
				r.left = p->rect.right - iconGroup->icons->iconNodes[iconID].sz.x;
				r.right = p->rect.right;
				r.top = pt.y;
				r.bottom = pt.y + 11;
				
				if (p->itemHeight && iconGroup->icons->iconNodes[iconID].sz.x)
				{
					int ihHalf = p->itemHeight / 2;
					int szHalf = iconGroup->icons->iconNodes[iconID].sz.x / 2;

					r.top += ihHalf - szHalf;
					r.bottom += ihHalf - szHalf;
				}

				c->gpu->drawRectangle(mgDrawRectangleReason_popupNextIcon,
					&r, &wh, &wh, 0, iconGroup->icons->texture, 0);
			}

			pt.y += p->itemHeight;
		}
	}

	for (int i = 0; i < p->itemsSize; ++i)
	{
		if (/*(p->items[i].info.type != mgPopupItemType_separator)
			&& */(p->items[i].info.subMenu))
		{
			if (p->items[i].info.subMenu->subVisible)
				mgDrawPopup(c, p->items[i].info.subMenu);
		}
	}
}

void
_mgUpdatePopup(struct mgContext_s* c, mgPopup* p)
{
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

		//mgPopup* subPopup = 0;

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
				
				/*if (p->items[i].info.subMenu)
					p->items[i].info.subMenu->subVisible = 0;*/

				r.bottom = r.top + p->itemHeight;
				if (mgPointInRect(&r, &c->input->mousePosition))
				{
					//if (p->items[i].info.subMenu)
					//	subPopup = p->items[i].info.subMenu;

					p->nodeUnderCursor = &p->items[i];
					p->nodeUnderCursorRect = r;

					if (p->items[i].info.subMenu)
					{
						mgPoint pt2;
						pt2.x = r.right - p->indent.x;
						pt2.y = r.top - p->indent.y;
						mgPopupSetPosition(c, p->items[i].info.subMenu, &pt2);

						//if(!p->items[i].info.subMenu->subVisible)
						//	_mgUpdatePopup(c, p->items[i].info.subMenu);

						mgPopupFixPosition(c, p->items[i].info.subMenu);
						p->items[i].info.subMenu->subVisible = 1;
					}
					else
					{

					}
					break;
				}
				pt.y += p->itemHeight;
			}
		}

		/*if (!subPopup)
		{
			mgPopupHideSub(p);
		}*/

		if (p->nodeUnderCursor && (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP))
		{
			if (!p->nodeUnderCursor->info.subMenu)
			{
				if (p->nodeUnderCursor->info.callback)
					p->nodeUnderCursor->info.callback();
				mgShowPopup_f(c, 0, 0);
			}
		}
	}
}

void
mgUpdatePopup_internal(struct mgContext_s* c, mgPopup* p)
{
	for (int i = 0; i < p->itemsSize; ++i)
	{
		if (p->items[i].info.subMenu)
		{
			p->items[i].info.subMenu->parent = p;

			if (p->items[i].info.subMenu->subVisible)
				mgUpdatePopup_internal(c, p->items[i].info.subMenu);
		}
	}
	mgPopupFixPosition(c, p);


	if (mgPointInRect(&p->rect, &c->input->mousePosition))
	{
		if (!c->popupUnderCursor)
		{
			c->popupUnderCursor = p;
			_mgUpdatePopup(c, p);
		}
	}
}

void
mgUpdatePopup(struct mgContext_s* c, mgPopup* p)
{
	mgUpdatePopup_internal(c, p);
	if (c->popupUnderCursor)
	{
		for (int i = 0; i < c->popupUnderCursor->itemsSize; ++i)
		{
			if (c->popupUnderCursor->nodeUnderCursor != &c->popupUnderCursor->items[i])
			{
				if (c->popupUnderCursor->items[i].info.subMenu)
					mgPopupHideSub(c->popupUnderCursor->items[i].info.subMenu);
			}
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
	newPopup->iconRightWidth = 16;
	newPopup->iconLeftWidth = 18;

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

	if (!p)
		return;

	for (int i = 0; i < p->itemsSize; ++i)
	{
		if (p->items[i].info.subMenu)
			mgPopupHideSub(p->items[i].info.subMenu);
	}

	mgPopupSetPosition(c, p, position);
}