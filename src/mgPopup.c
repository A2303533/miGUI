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
#include <stdio.h>

#ifdef MG_PLATFORM_WINDOWS
#include <Windows.h>
LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#else
#endif

struct mgPopupSystemWindow
{
	struct mgSystemWindowOSData systemWindowData;
};

void
mgPopupDestroySystemWindow(mgPopup* p)
{
	struct mgPopupSystemWindow* ptr = p->systemWindowImplementation;
#ifdef MG_PLATFORM_WINDOWS
	if (ptr->systemWindowData.hdcMem)
		DeleteDC(ptr->systemWindowData.hdcMem);
	if (ptr->systemWindowData.hbmMem)
		DeleteObject(ptr->systemWindowData.hbmMem);
#endif
	free(p->systemWindowImplementation);
}

void
mgPopupInitSystemWindow(struct mgContext_s* c, mgPopup* p)
{
	int success = 0;
	struct mgPopupSystemWindow* systemWindow = calloc(1, sizeof(struct mgPopupSystemWindow));

#ifdef MG_PLATFORM_WINDOWS
	static int isClassnameRegistered = 0;
	static WNDCLASSEXW wcex;
	if (!isClassnameRegistered)
	{
		memset(&wcex, 0, sizeof(WNDCLASSEXW));
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
		wcex.lpfnWndProc = PopupWndProc;
		wcex.hInstance = GetModuleHandle(0);
		wcex.hCursor = LoadCursor(0, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszClassName = L"popup_window";
		RegisterClassExW(&wcex);
		isClassnameRegistered = 1;
	}

	systemWindow->systemWindowData.hWnd = CreateWindowExW(
		WS_EX_NOACTIVATE | WS_EX_WINDOWEDGE,
		wcex.lpszClassName, L"pp",
		WS_POPUP | WS_CHILD | WS_BORDER,
		0,0, 10, 10, c->systemWindowData.hWnd, 0, wcex.hInstance, 0);

	//ShowWindow(systemWindow->hwnd, SW_SHOWNOACTIVATE);
	success = 1;
#else
#endif

	if (success)
	{
		//if (c->onCreateWindow) c->onCreateWindow(&systemWindow->systemWindowData);

		p->systemWindowImplementation = systemWindow;
	}
}

void
mgPopupFixPosition(struct mgContext_s* c, mgPopup* p)
{
	if ((p->flags & mgPopupFlags_systemWindow) && p->systemWindowImplementation)
	{
#ifdef MG_PLATFORM_WINDOWS
		RECT desktopRect;
		GetWindowRect(GetDesktopWindow(), &desktopRect);
		
		struct mgPopupSystemWindow* ptr = p->systemWindowImplementation;
		
		RECT popupRect;
		GetWindowRect(ptr->systemWindowData.hWnd, &popupRect);

		if (popupRect.right > desktopRect.right)
		{
			int v = popupRect.right - desktopRect.right;

			MoveWindow(ptr->systemWindowData.hWnd,
				popupRect.left - v,
				popupRect.top,
				popupRect.right - popupRect.left,
				popupRect.bottom - popupRect.top,
				TRUE);

			p->rect.left -= v;
			p->rect.right -= v;
		}

		int iTaskBarHeight = 0;
		int iYScreenSize = 0;
		RECT rWorkArea;
		iYScreenSize = GetSystemMetrics(SM_CYSCREEN);
		SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rWorkArea, 0);
		iTaskBarHeight = iYScreenSize - (rWorkArea.bottom - rWorkArea.top);

		if (popupRect.bottom > (desktopRect.bottom + iTaskBarHeight))
		{
			int v = popupRect.bottom - desktopRect.bottom + iTaskBarHeight;

			MoveWindow(ptr->systemWindowData.hWnd,
				popupRect.left,
				popupRect.top - v,
				popupRect.right - popupRect.left,
				popupRect.bottom - popupRect.top,
				TRUE);

			p->rect.top -= v;
			p->rect.bottom -= v;
		}
#endif
	}
	else
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
			mgRect oldRect = p->rect;

			if ((p->rect.left < p->parent->rect.right)
				&& (p->rect.left > p->parent->rect.left))
			{
				int v = p->rect.left - p->parent->rect.left;
				v += p->rect.right - p->rect.left;
				p->rect.left -= v;
				p->rect.right -= v;

				/*if rect outside of the window from left border*/
				/*move to old rect and move little bit to the left*/
				if (p->rect.left < 0)
				{
					p->rect = oldRect;

					int v = p->rect.right - c->windowSize.x;
					p->rect.left -= v;
					p->rect.right -= v;
				}
			}
		}
	}
}

void
mgPopupHideSub(struct mgPopup_s* p)
{
	p->subVisible = 0;
	if (p->flags & mgPopupFlags_systemWindow)
	{
		if (p->systemWindowImplementation)
		{
#ifdef MG_PLATFORM_WINDOWS
			struct mgPopupSystemWindow* ptr = p->systemWindowImplementation;
			ShowWindow(ptr->systemWindowData.hWnd, SW_HIDE);
#endif
		}
	}

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
						//c->getTextSize(p->items[i].info.text, p->font, &tsz);
						p->textProcessor->onGetTextSize(
							mgDrawTextReason_popup,
							p->textProcessor,
							p->items[i].info.text,
							p->items[i].textLen,
							&tsz);

						if (tsz.y > p->itemHeight)
							p->itemHeight = tsz.y;

						if (tsz.x > p->maxTextWidth)
							p->maxTextWidth = tsz.x;
					}

					if (p->items[i].info.shortcutText)
					{
						//c->getTextSize(p->items[i].info.shortcutText, p->font, &tsz);
						p->textProcessor->onGetTextSize(
							mgDrawTextReason_popupShortcut,
							p->textProcessor,
							p->items[i].info.text,
							p->items[i].textLen,
							&tsz);

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

		if (p->flags & mgPopupFlags_systemWindow)
		{
			if (p->systemWindowImplementation)
			{
				struct mgPopupSystemWindow* ptr = p->systemWindowImplementation;
#ifdef MG_PLATFORM_WINDOWS
				RECT rct;
				GetWindowRect(c->systemWindowData.hWnd, &rct);

				POINT pt;
				pt.x = p->rect.left;
				pt.y = p->rect.top;
				ClientToScreen(c->systemWindowData.hWnd, &pt);

				ptr->systemWindowData.size.x = p->rect.right - p->rect.left;
				ptr->systemWindowData.size.y = p->rect.bottom - p->rect.top;

				MoveWindow(ptr->systemWindowData.hWnd,
					pt.x,
					pt.y,
					ptr->systemWindowData.size.x,
					ptr->systemWindowData.size.y,
					TRUE);
				struct mgSystemWindowOSData* old = c->gpu->setCurrentWindow(&ptr->systemWindowData);
				c->gpu->updateBackBuffer();
				c->gpu->setCurrentWindow(old);
#endif
			}
		}
	}
}

void MG_C_DECL
mgDrawPopup(struct mgContext_s* c, mgPopup* p)
{
	mgRect popupRect = p->rect;
	struct mgSystemWindowOSData* oldWindow = 0;

	int isSystemPopup = 0;

	if ((p->flags & mgPopupFlags_systemWindow) && p->systemWindowImplementation)
	{
		struct mgPopupSystemWindow* ptr = p->systemWindowImplementation;
		oldWindow = c->gpu->setCurrentWindow(&ptr->systemWindowData);
		isSystemPopup = 1;

		popupRect.left = 0;
		popupRect.top = 0;
		popupRect.right = ptr->systemWindowData.size.x;
		popupRect.bottom = ptr->systemWindowData.size.y;

		c->gpu->setClipRect(&popupRect);
		c->gpu->beginDraw(mgBeginDrawReason_popupWindow);
	}


	mgStyle* style = p->userStyle ? p->userStyle : c->activeStyle;
	
	c->gpu->setClipRect(&popupRect);
	c->gpu->drawRectangle(mgDrawRectangleReason_popupBG, 
		p,
		&popupRect,
		&style->popupBG,
		&style->popupBG,
		0, 0);

	mgPoint pt;
	mgPoint pt2;
	pt.y = popupRect.top + p->indent.y;
	for (int i = 0; i < p->itemsSize; ++i)
	{
		pt.x = popupRect.left + p->indent.x + p->iconLeftWidth;

		if (p->items[i].info.type == mgPopupItemType_separator)
		{
			mgRect r;
			r.left = popupRect.left;
			r.top = pt.y;
			r.right = popupRect.right;
			r.bottom = r.top + 5;
			c->gpu->drawRectangle(mgDrawRectangleReason_popupSeparator,
				p,
				&r,
				&style->popupSeparator,
				&style->popupSeparator,
				0, 0);
			pt.y += 5;
		}
		else
		{
			if (p->onIsItemEnabled)
			{
				p->items[i].info.isEnabled = p->onIsItemEnabled(c, p, &p->items[i]);
			}

			if (&p->items[i] == p->nodeUnderCursor)
			{
				mgRect nodeRect = p->nodeUnderCursorRect;
				if (isSystemPopup)
				{
					nodeRect.right -= p->rect.left;
					nodeRect.left -= p->rect.left;
					nodeRect.bottom -= p->rect.top;
					nodeRect.top -= p->rect.top;

					/*nodeRect.left = popupRect.left;
					nodeRect.right = popupRect.right;
					nodeRect.top = */
				}

				c->gpu->drawRectangle(mgDrawRectangleReason_popupHoverElement,
					p,
					&nodeRect,
					&style->popupHoverElementBG,
					&style->popupHoverElementBG,
					0, 0);
			}

			mgColor* textColor = &style->popupText;
			if(!p->items[i].info.isEnabled)
				textColor = &style->popupTextDisabled;

			/*c->gpu->drawText(mgDrawTextReason_popup, 
				p,
				&pt,
				p->items[i].info.text,
				p->items[i].textLen,
				textColor,
				p->font);*/
			p->textProcessor->onDrawText(
				mgDrawTextReason_popup,
				p->textProcessor,
				&pt,
				p->items[i].info.text,
				p->items[i].textLen,
				textColor);

			if (p->items[i].info.shortcutText)
			{
				pt2 = pt;
				pt2.x += p->items[i].indentForShortcutText;

				textColor = &style->popupTextShortcut;
				if (!p->items[i].info.isEnabled)
					textColor = &style->popupTextShortcutDisabled;

				/*c->gpu->drawText(mgDrawTextReason_popupShortcut, 
					p,
					&pt2,
					p->items[i].info.shortcutText,
					p->items[i].shortcutTextLen,
					textColor,
					p->font);*/
				p->textProcessor->onDrawText(
					mgDrawTextReason_popupShortcut,
					p->textProcessor,
					&pt,
					p->items[i].info.shortcutText,
					p->items[i].shortcutTextLen,
					textColor);
			}

			if (p->items[i].info.subMenu)
			{
				mgIconGroup* iconGroup = c->defaultIconGroup;
				int iconID = iconGroup->popupNext;
				c->currentIcon = &iconGroup->icons->iconNodes[iconID];

				mgRect r;
				r.left = popupRect.right - iconGroup->icons->iconNodes[iconID].sz.x;
				r.right = popupRect.right;
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
					p,
					&r, 
					&style->popupIconNext,
					&style->popupIconNext,
					iconGroup->icons->texture, 0);
			}
			else
			{
				int isRadio = 0;
				if (p->onIsItemRadio)
					isRadio = p->onIsItemRadio(c, p, &p->items[i]);

				if(p->onIsItemChecked)
					p->items[i].info.isChecked = p->onIsItemChecked(c, p, &p->items[i]);

				if (p->items[i].info.isChecked)
				{
					mgIconGroup* iconGroup = c->defaultIconGroup;
					mgIcons* icons = iconGroup->icons;

					mgColor iconColor = style->popupIconCheck;

					int iconID = isRadio == 1 ? iconGroup->popupCheckRadio : iconGroup->popupCheck;

					if (p->onIcon)
					{
						struct mgIcons_s* user_icons = 0;
						int user_iconID = 0;
						mgColor user_iconColor;
						if (p->onIcon(c, p, &p->items[i], &user_icons, &user_iconID, &user_iconColor))
						{
							if (user_icons)
							{
								icons = user_icons;
								iconID = user_iconID;
								iconColor = user_iconColor;
							}
						}
					}
					
					c->currentIcon = &icons->iconNodes[iconID];

					mgRect r;
					r.left = popupRect.left + p->indent.x;
					r.right = r.left + icons->iconNodes[iconID].sz.x;
					r.top = pt.y;
					r.bottom = r.top + icons->iconNodes[iconID].sz.y;

					c->gpu->drawRectangle(mgDrawRectangleReason_popupCheckIcon,
						p,
						&r, 
						&iconColor,
						&iconColor,
						icons->texture, 0);
				}
			}

			pt.y += p->itemHeight;
		}
	}

	if (oldWindow)
	{
		c->gpu->endDraw();
		c->gpu->setCurrentWindow(oldWindow);
	}

	for (int i = 0; i < p->itemsSize; ++i)
	{
		if (p->items[i].info.subMenu)
		{
			if (p->items[i].info.subMenu->subVisible)
			{
				p->items[i].info.subMenu->userStyle = p->userStyle;
				mgDrawPopup(c, p->items[i].info.subMenu);
			}
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
					if (p->items[i].info.isEnabled)
					{
						p->nodeUnderCursor = &p->items[i];
						p->nodeUnderCursorRect = r;

						if (p->items[i].info.subMenu)
						{
							mgPoint pt2;
							pt2.x = r.right - p->indent.x;
							pt2.y = r.top - p->indent.y;

							mgPopupSetPosition(c, p->items[i].info.subMenu, &pt2);

							mgPopupFixPosition(c, p->items[i].info.subMenu);
							p->items[i].info.subMenu->subVisible = 1;

							if (p->items[i].info.subMenu->flags & mgPopupFlags_systemWindow)
							{
								struct mgPopupSystemWindow* ptr = p->items[i].info.subMenu->systemWindowImplementation;
								ShowWindow(ptr->systemWindowData.hWnd, SW_SHOWNOACTIVATE);
							}
						}
						else
						{

						}
						break;
					}
				}
				pt.y += p->itemHeight;
			}
		}
		if (p->nodeUnderCursor && (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP))
		{
			if (!p->nodeUnderCursor->info.subMenu)
			{
				if (p->nodeUnderCursor->info.callback)
					p->nodeUnderCursor->info.callback(p->nodeUnderCursor->info.id, p->nodeUnderCursor);
				mgShowPopup(c, 0, 0);
				c->cursorInPopup = 0;

				if (c->activeMenu)
				{
					c->activeMenu->activeItem = 0;
					c->activeMenu = 0;
				}
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

struct mgPopup_s* MG_C_DECL
mgCreatePopup(struct mgContext_s* c, struct mgPopupItemInfo_s* arr, int arrSize, 
	int flags, 
	struct mgTextProcessor_s* tp)
{
	assert(tp);
	assert(arr);
	assert(arrSize>0);

	struct mgPopup_s* newPopup = calloc(1, sizeof(struct mgPopup_s));
	newPopup->textProcessor = tp;
	//newPopup->font = fnt;
	newPopup->itemsSize = arrSize;
	newPopup->items = calloc(1, sizeof(struct mgPopupItem_s) * arrSize);
	newPopup->indent.x = 5;
	newPopup->indent.y = 5;
	newPopup->textShortcutTextIndent = 20;
	newPopup->iconRightWidth = 16;
	newPopup->iconLeftWidth = 18;
	newPopup->flags = flags;
	
	for (int i = 0; i < arrSize; ++i)
	{
		newPopup->items[i].info = arr[i];
		if(newPopup->items[i].info.text)
			newPopup->items[i].textLen = mgUnicodeStrlen(newPopup->items[i].info.text);

		if (newPopup->items[i].info.shortcutText)
			newPopup->items[i].shortcutTextLen = mgUnicodeStrlen(newPopup->items[i].info.shortcutText);
	}

	if (newPopup->flags & mgPopupFlags_systemWindow)
	{
		mgPopupInitSystemWindow(c, newPopup);
		for (int i = 0; i < newPopup->itemsSize; ++i)
		{
			if (newPopup->items[i].info.subMenu && newPopup->systemWindowImplementation)
				mgPopupInitSystemWindow(c, newPopup->items[i].info.subMenu);
		}
	}

	return newPopup;
}

void MG_C_DECL
mgDestroyPopup(struct mgPopup_s* p)
{
	assert(p);
	if (p)
	{
		if (p->systemWindowImplementation)
			mgPopupDestroySystemWindow(p);

		if (p->items)
			free(p->items);
		free(p);
	}
}

void MG_C_DECL
mgShowPopup(struct mgContext_s* c, struct mgPopup_s* p, mgPoint* position)
{
	assert(c);
	
	if (!p && c->activePopup)
	{
#ifdef MG_PLATFORM_WINDOWS
		if ((c->activePopup->flags & mgPopupFlags_systemWindow) && c->activePopup->systemWindowImplementation)
		{
			struct mgPopupSystemWindow* ptr = c->activePopup->systemWindowImplementation;
			ShowWindow(ptr->systemWindowData.hWnd, SW_HIDE);
		}

		for (int i = 0; i < c->activePopup->itemsSize; ++i)
		{
			if (c->activePopup->items[i].info.subMenu)
				mgPopupHideSub(c->activePopup->items[i].info.subMenu);
		}
#endif
	}

	c->activePopup = p;

	if (!p)
	{
		return;
	}

	if (p->onShow)
		p->onShow(c, p);

	if (p->flags & mgPopupFlags_systemWindow)
	{
#ifdef MG_PLATFORM_WINDOWS
		struct mgPopupSystemWindow* ptr = p->systemWindowImplementation;
		ShowWindow(ptr->systemWindowData.hWnd, SW_SHOWNOACTIVATE);
#endif
	}

	for (int i = 0; i < p->itemsSize; ++i)
	{
		if (p->items[i].info.subMenu)
			mgPopupHideSub(p->items[i].info.subMenu);
	}

	mgPopupSetPosition(c, p, position);
	mgSetCursor(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
}