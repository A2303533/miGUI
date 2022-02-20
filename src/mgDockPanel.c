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
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <Windows.h>
void VSOutput(const char* szFormat, ...)
{
	char szBuff[1024];
	memset(szBuff,0, 1024);
	va_list arg;
	va_start(arg, szFormat);
	vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugStringA(szBuff);
}

extern int g_windowToDockPanelMode;
extern mgRect g_windowToDockRect;

float lerp(float v0, float v1, float t);

int g_windowSize = 0;
float g_lerpTime = 10.f;
int g_blockSplitterMode = 0; /*block resizing when some panel is too small*/
int g_mouseMoveDirBeforeBlock = 0; /*0, 1 = x+, 2= x-, 3 = y+, 4 = y-*/
int g_dockpanel_inSplitterRect = 0;
struct mgDockPanelElement_s* g_dockpanel_splitterModeElement = 0;
struct mgDockPanelWindow_s* g_dockpanel_splitterModePanel = 0;
mgDockPanelWindow* g_pnlWnd_onPopup = 0;

void mgDrawWindow(struct mgWindow_s* w);
void mgUpdateWindow(struct mgWindow_s* w);
void mgUpdateElement(mgElement* e);

void dockPanel_popupCallback_makeFirst();
void dockPanel_popupCallback_unpin();
void dockPanel_popupCallback_close();

void
_mgDockPanelRebuild(struct mgContext_s* c)
{
	c->dockPanel->elements[0].rect = c->dockPanel->rect;
	mgRect mainElRect = c->dockPanel->elements[0].rect;

	if (c->dockPanel->elementsSize > 1)
	{
		for (int i = 1; i < c->dockPanel->elementsSize; ++i)
		{
			int sz = c->dockPanel->elements[i].info.size;

			if (!c->dockPanel->elements[i].panelWindows) sz = 0;

			int addwndSz = 0;
			if (c->dockPanel->elements[i].info.sizeMinimum < 50)
				addwndSz = 50;

			mgRect rect;
			mgRect splitterRect;
			mgRect addWindowRect;
			switch (c->dockPanel->elements[i].info.where)
			{
			default:
			case 0:
				rect.left = mainElRect.left;
				rect.top = mainElRect.top;
				rect.bottom = mainElRect.bottom;
				rect.right = rect.left + sz;
				
				
				addWindowRect = rect;
				addWindowRect.right = rect.left + c->dockPanel->elements[i].info.sizeMinimum + addwndSz;

				mainElRect.left = rect.right;
				splitterRect.left = rect.right - c->dockPanel->splitterWidth;
				splitterRect.right = rect.right + c->dockPanel->splitterWidth;
				splitterRect.top = rect.top;
				splitterRect.bottom = rect.bottom;
				break;
			case 1:
				rect.left = mainElRect.left;
				rect.top = mainElRect.top;
				rect.right = mainElRect.right;
				rect.bottom = rect.top + sz;

				addWindowRect = rect;
				addWindowRect.bottom = rect.top + c->dockPanel->elements[i].info.sizeMinimum + addwndSz;

				mainElRect.top = rect.bottom;
				splitterRect.left = rect.left;
				splitterRect.right = rect.right;
				splitterRect.top = rect.bottom - c->dockPanel->splitterWidth;
				splitterRect.bottom = rect.bottom + c->dockPanel->splitterWidth;
				break;
			case 2:
				rect.top = mainElRect.top;
				rect.bottom = mainElRect.bottom;
				rect.right = mainElRect.right;
				rect.left = rect.right - sz;

				addWindowRect = rect;
				addWindowRect.left = rect.right - c->dockPanel->elements[i].info.sizeMinimum - addwndSz;

				mainElRect.right = rect.left;
				splitterRect.left = rect.left - c->dockPanel->splitterWidth;
				splitterRect.right = rect.left + c->dockPanel->splitterWidth;
				splitterRect.top = rect.top;
				splitterRect.bottom = rect.bottom;
				break;
			case 3:
				rect.bottom = mainElRect.bottom;
				rect.left = mainElRect.left;
				rect.right = mainElRect.right;
				rect.top = rect.bottom - sz;

				addWindowRect = rect;
				addWindowRect.top = rect.bottom - c->dockPanel->elements[i].info.sizeMinimum - addwndSz;

				mainElRect.bottom = rect.top;
				splitterRect.left = rect.left;
				splitterRect.right = rect.right;
				splitterRect.top = rect.top - c->dockPanel->splitterWidth;
				splitterRect.bottom = rect.top + c->dockPanel->splitterWidth;
				break;
			}

			c->dockPanel->elements[i].addWindowRect = addWindowRect;
			c->dockPanel->elements[i].rect = rect;
			c->dockPanel->elements[i].splitterRect = splitterRect;
		}
	}

	mgPoint mainElSz;
	mainElSz.x = abs(mainElRect.right - mainElRect.left);
	mainElSz.y = abs(mainElRect.bottom - mainElRect.top);
	c->dockPanel->mainElementSize = mainElSz;
	c->dockPanel->elements[0].rect = mainElRect;
}

void
mgDockPanelUpdateTabRect(struct mgContext_s* c, mgDockPanelWindow* pw)
{
	pw->windowRect = pw->rect;
	pw->windowRect.top += c->dockPanel->tabHeight;
	pw->tabRect = pw->rect;
	pw->tabRect.bottom = pw->windowRect.top;
}

void
mgDockPanelCheckRects(struct mgContext_s* c)
{
	if (c->dockPanel->elementsSize > 1)
	{
		int difx = c->dockPanel->mainElementSizeMinimum.x - c->dockPanel->mainElementSize.x;
		int dify = c->dockPanel->mainElementSizeMinimum.y - c->dockPanel->mainElementSize.y;
		if (c->dockPanel->mainElementSize.x < c->dockPanel->mainElementSizeMinimum.x)
		{
			for (int i = 1; i < c->dockPanel->elementsSize; ++i)
			{
				switch (c->dockPanel->elements[i].info.where)
				{
				case 0:
				case 2:
					c->dockPanel->elements[i].info.size -= difx;
					if (c->dockPanel->elements[i].info.size < c->dockPanel->elements[i].info.sizeMinimum)
						c->dockPanel->elements[i].info.size = c->dockPanel->elements[i].info.sizeMinimum;
					break;
				}
			}
		}
		else if (g_dockpanel_splitterModeElement && (difx < -10))
		{
			for (int i = 1; i < c->dockPanel->elementsSize; ++i)
			{
				if (&c->dockPanel->elements[i] == g_dockpanel_splitterModeElement)
					continue;

				c->dockPanel->elements[i].info.size =
					(int)lerp((float)c->dockPanel->elements[i].info.size,
						c->dockPanel->elements[i].sizeOnClick, c->deltaTime * g_lerpTime);
			}
		}
		else if (g_windowSize && (difx < -10))
		{
			for (int i = 1; i < c->dockPanel->elementsSize; ++i)
			{
				if (&c->dockPanel->elements[i] == g_dockpanel_splitterModeElement)
					continue;

				c->dockPanel->elements[i].info.size =
					(int)lerp((float)c->dockPanel->elements[i].info.size,
						c->dockPanel->elements[i].sizeOnClick, c->deltaTime * g_lerpTime);
			}
		}

		if (c->dockPanel->mainElementSize.y < c->dockPanel->mainElementSizeMinimum.y)
		{
			for (int i = 1; i < c->dockPanel->elementsSize; ++i)
			{
				switch (c->dockPanel->elements[i].info.where)
				{
				case 1:
				case 3:
					c->dockPanel->elements[i].info.size -= dify;
					if (c->dockPanel->elements[i].info.size < c->dockPanel->elements[i].info.sizeMinimum)
						c->dockPanel->elements[i].info.size = c->dockPanel->elements[i].info.sizeMinimum;
					break;
				}
			}
		}
		else if (g_dockpanel_splitterModeElement && (dify < -10))
		{
			for (int i = 1; i < c->dockPanel->elementsSize; ++i)
			{
				if (&c->dockPanel->elements[i] == g_dockpanel_splitterModeElement)
					continue;

				c->dockPanel->elements[i].info.size =
					(int)lerp((float)c->dockPanel->elements[i].info.size,
						c->dockPanel->elements[i].sizeOnClick, c->deltaTime * g_lerpTime);
			}
		}
		else if (g_windowSize && (dify < -10))
		{
			for (int i = 1; i < c->dockPanel->elementsSize; ++i)
			{
				if (&c->dockPanel->elements[i] == g_dockpanel_splitterModeElement)
					continue;

				c->dockPanel->elements[i].info.size =
					(int)lerp((float)c->dockPanel->elements[i].info.size,
						c->dockPanel->elements[i].sizeOnClick, c->deltaTime * g_lerpTime);
			}
		}
	}

	for (int i = 1; i < c->dockPanel->elementsSize; ++i)
	{
		if (!c->dockPanel->elements[i].panelWindows)
			continue;

		for (int o = 0; o < c->dockPanel->elements[i].panelWindowsSize; ++o)
		{
			mgDockPanelWindow* pw = c->dockPanel->elements[i].panelWindows[o];

			if(o == 0)
				pw->rect = c->dockPanel->elements[i].rect;

			if (pw->parent)
			{
				mgDockPanelWindow* pw2 = pw->parent;
				pw->rect = pw2->rect;
				switch (pw->where)
				{
				default:
					break;
				case 0:
					pw2->rect.left += pw->sz;
					pw->rect.right = pw2->rect.left;

					if (pw2->rect.left >= pw2->rect.right - c->dockPanel->tabHeight)
					{
						pw2->rect.left = pw2->rect.right - c->dockPanel->tabHeight;
						pw->rect.right = pw2->rect.left;
					}
								
					pw->splitterRect.top = pw->rect.top;
					pw->splitterRect.bottom = pw->rect.bottom;
					pw->splitterRect.left = pw->rect.right - c->dockPanel->splitterWidth;
					pw->splitterRect.right = pw->rect.right + c->dockPanel->splitterWidth;
					break;
				case 1:
					pw2->rect.top += pw->sz;
					pw->rect.bottom = pw2->rect.top;
						
					if (pw2->rect.top >= pw2->rect.bottom - c->dockPanel->tabHeight)
					{
						pw2->rect.top = pw2->rect.bottom - c->dockPanel->tabHeight;
						pw->rect.bottom = pw2->rect.top;
					}

					pw->splitterRect.left = pw->rect.left;
					pw->splitterRect.right = pw->rect.right;
					pw->splitterRect.top = pw->rect.bottom - c->dockPanel->splitterWidth;
					pw->splitterRect.bottom = pw->rect.bottom + c->dockPanel->splitterWidth;
					break;
				case 2:
					pw2->rect.right -= pw->sz;
					pw->rect.left = pw2->rect.right;

					if (pw2->rect.right <= pw2->rect.left + c->dockPanel->tabHeight)
					{
						pw2->rect.right = pw2->rect.left + c->dockPanel->tabHeight;
						pw->rect.left = pw2->rect.right;
					}

					pw->splitterRect.top = pw->rect.top;
					pw->splitterRect.bottom = pw->rect.bottom;
					pw->splitterRect.left = pw->rect.left - c->dockPanel->splitterWidth;
					pw->splitterRect.right = pw->rect.left + c->dockPanel->splitterWidth;
					break;
				case 3:
					pw2->rect.bottom -= pw->sz;
					pw->rect.top = pw2->rect.bottom;

					if (pw2->rect.bottom <= pw2->rect.top + c->dockPanel->tabHeight)
					{
						pw2->rect.bottom = pw2->rect.top + c->dockPanel->tabHeight;
						pw->rect.top = pw2->rect.bottom;
					}

					pw->splitterRect.left = pw->rect.left;
					pw->splitterRect.right = pw->rect.right;
					pw->splitterRect.top = pw->rect.top - c->dockPanel->splitterWidth;
					pw->splitterRect.bottom = pw->rect.top + c->dockPanel->splitterWidth;
					break;
				}
				pw2->windowRect = pw2->rect;
				pw2->windowRect.top += c->dockPanel->tabHeight;
				pw2->tabRect = pw2->rect;
				pw2->tabRect.bottom = pw2->windowRect.top;
				mgDockPanelUpdateTabRect(c, pw2);
			}

			mgDockPanelUpdateTabRect(c, pw);

			
		}
	}

	for (int i = 1; i < c->dockPanel->elementsSize; ++i)
	{
		if (!c->dockPanel->elements[i].panelWindows)
			continue;

		for (int o = 0; o < c->dockPanel->elements[i].panelWindowsSize; ++o)
		{
			mgDockPanelWindow* pw = c->dockPanel->elements[i].panelWindows[o];
			mgRect tr = pw->tabRect;
			for (int i2 = 0; i2 < pw->windowsSize; ++i2)
			{
				int tl = 0;
				mgWindow* wnd = pw->windows[i2];
				if (wnd->titlebarText)
				{
					mgPoint pt;
					c->getTextSize(pw->windows[i2]->titlebarText, pw->windows[i2]->titlebarFont, &pt);
					tl = pt.x;
				}

				int w = tl + 6;
				wnd->dockPanelTabRect.left = tr.left;
				wnd->dockPanelTabRect.top = tr.top;
				wnd->dockPanelTabRect.right = wnd->dockPanelTabRect.left + w;

				if (wnd->dockPanelTabRect.right > pw->windowRect.right)
					wnd->dockPanelTabRect.right = pw->windowRect.right;

				wnd->dockPanelTabRect.bottom = wnd->dockPanelTabRect.top + c->dockPanel->tabHeight;

				if (wnd->dockPanelTabRect.left > pw->windowRect.right)
					mgRectSet(&wnd->dockPanelTabRect, 0, 0, 0, 0);

				tr.left += w + 2;
			}
		}
	}
}

void
mgDockPanelRebuild(struct mgContext_s* c)
{
	_mgDockPanelRebuild(c);
	if (c->dockPanel->elementsSize > 1)
	{
		for (int i = 1; i < c->dockPanel->elementsSize; ++i)
		{
			if (!c->dockPanel->elements[i].panelWindows)
				continue;

			for (int o = 0; o < c->dockPanel->elements[i].panelWindowsSize; ++o)
			{
				mgDockPanelWindow* pw = c->dockPanel->elements[i].panelWindows[o];

				if (o == 0)
					pw->rect = c->dockPanel->elements[i].rect;

				if (pw->parent)
				{
					mgDockPanelWindow* pw2 = pw->parent;
					pw->rect = pw2->rect;
					switch (pw->where)
					{
					default:
						break;
					case 0:
						pw2->rect.left += pw->sz;
						pw->rect.right = pw2->rect.left;

						pw->splitterRect.top = pw->rect.top;
						pw->splitterRect.bottom = pw->rect.bottom;
						pw->splitterRect.left = pw->rect.right - c->dockPanel->splitterWidth;
						pw->splitterRect.right = pw->rect.right + c->dockPanel->splitterWidth;

						break;
					case 1:
						pw2->rect.top += pw->sz;
						pw->rect.bottom = pw2->rect.top;
						pw->splitterRect.left = pw->rect.left;
						pw->splitterRect.right = pw->rect.right;
						pw->splitterRect.top = pw->rect.bottom - c->dockPanel->splitterWidth;
						pw->splitterRect.bottom = pw->rect.bottom + c->dockPanel->splitterWidth;
						break;
					case 2:
						pw2->rect.right -= pw->sz;
						pw->rect.left = pw2->rect.right;
						pw->splitterRect.top = pw->rect.top;
						pw->splitterRect.bottom = pw->rect.bottom;
						pw->splitterRect.left = pw->rect.left - c->dockPanel->splitterWidth;
						pw->splitterRect.right = pw->rect.left + c->dockPanel->splitterWidth;
						break;
					case 3:
						pw2->rect.bottom -= pw->sz;
						pw->rect.top = pw2->rect.bottom;
						pw->splitterRect.left = pw->rect.left;
						pw->splitterRect.right = pw->rect.right;
						pw->splitterRect.top = pw->rect.top - c->dockPanel->splitterWidth;
						pw->splitterRect.bottom = pw->rect.top + c->dockPanel->splitterWidth;
						break;
					}
					mgDockPanelUpdateTabRect(c, pw2);
				}

				mgDockPanelUpdateTabRect(c, pw);
			}
		}
		
		mgDockPanelCheckRects(c);

		for (int i = 1; i < c->dockPanel->elementsSize; ++i)
		{
			if (!c->dockPanel->elements[i].panelWindows)
				continue;
			for (int o = 0; o < c->dockPanel->elements[i].panelWindowsSize; ++o)
			{
				mgDockPanelWindow* pw = c->dockPanel->elements[i].panelWindows[o];
				mgDockPanelUpdateTabRect(c, pw);
			}
		}
	}

}

void
mgDockPanelUpdateWindow(struct mgContext_s* c)
{
	if (c->dockPanel->arrayWindows && c->dockPanel->arrayWindowsSize)
	{
		for (int i = 0; i < c->dockPanel->arrayWindowsSize; ++i)
		{
			mgUpdateWindow(c->dockPanel->arrayWindows[i]->activeWindow);
		}
	}
}

void
mgDockPanelOnSize(struct mgContext_s* c)
{
	static int lmbdown = 0;
	static int lmbdownOld = 0;

	if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN && !lmbdown)
	{
		lmbdown = 1;
		g_windowSize = 1;
	}

	if (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
	{
		lmbdown = 0;
		g_windowSize = 0;
	}

	if (lmbdown && !lmbdownOld)
	{
		for (int i2 = 0; i2 < c->dockPanel->elementsSize; ++i2)
		{
			c->dockPanel->elements[i2].sizeOnClick = c->dockPanel->elements[i2].info.size;
		}
	}

	c->dockPanel->rect.left = c->dockPanel->indent.left;
	c->dockPanel->rect.top = c->dockPanel->indent.top;
	c->dockPanel->rect.right = c->windowSize.x - c->dockPanel->indent.right;
	c->dockPanel->rect.bottom = c->windowSize.y - c->dockPanel->indent.bottom;
	c->dockPanel->size.x = c->dockPanel->rect.right - c->dockPanel->rect.left;
	c->dockPanel->size.y = c->dockPanel->rect.bottom - c->dockPanel->rect.top;
	mgDockPanelRebuild(c);
	mgDockPanelUpdateWindow(c);
	
	if (c->dockPanel->elementsSize > 1)
	{

	}
	lmbdownOld = lmbdown;
}

void
mgDrawDockPanel(struct mgContext_s* c)
{
	if (c->dockPanel->flags & mgDockPanelFlag_drawBG)
	{
		c->gpu->setClipRect(&c->dockPanel->rect);
		c->gpu->drawRectangle(mgDrawRectangleReason_dockBG, &c->dockPanel->rect, 
			&c->activeStyle->dockpanelBGColor, 
			&c->activeStyle->dockpanelBGColor, 
			0, 0, 0);
	}

	for (int i = 0; i < c->dockPanel->elementsSize; ++i)
	{
		c->gpu->setClipRect(&c->dockPanel->elements[i].rect);

		if (c->dockPanel->elements[i].flags & mgDockPanelElementFlag_drawBG)
		{
			c->gpu->drawRectangle(mgDrawRectangleReason_dockElementBG, &c->dockPanel->elements[i].rect,
				&c->dockPanel->elements[i].colorBG, 
				&c->dockPanel->elements[i].colorBG, 
				0, 0, 0);
		}
	}

	if (c->dockPanel->arrayWindows && c->dockPanel->arrayWindowsSize)
	{
		for (int i = 0; i < c->dockPanel->arrayWindowsSize; ++i)
		{
			c->gpu->setClipRect(&c->dockPanel->arrayWindows[i]->tabRect);
			c->gpu->drawRectangle(mgDrawRectangleReason_dockTabBG, &c->dockPanel->arrayWindows[i]->tabRect,
				&c->activeStyle->windowBGColor,
				&c->activeStyle->windowBGColor,
				0, 0, 0);
			
			/*mgPoint p;
			mgPointSet(&p, c->dockPanel->arrayWindows[i]->tabRect.left, c->dockPanel->arrayWindows[i]->tabRect.top);
			wchar_t t[30];
			swprintf(t, 30, L"W:%i", c->dockPanel->arrayWindows[i]->windowsSize);
			c->gpu->drawText(0, &p, t, wcslen(t), &c->activeStyle->windowTitlebarTextColor, c->dockPanel->arrayWindows[i]->activeWindow->titlebarFont);*/

			c->gpu->setClipRect(&c->dockPanel->arrayWindows[i]->windowRect);
			mgDrawWindow(c->dockPanel->arrayWindows[i]->activeWindow);			
		}

		for (int i = 0; i < c->dockPanel->elementsSize; ++i)
		{
			for (int i2 = 0; i2 < c->dockPanel->elements[i].panelWindowsSize; ++i2)
			{
				for (int i3 = 0; i3 < c->dockPanel->elements[i].panelWindows[i2]->windowsSize; ++i3)
				{
					mgWindow* wnd = c->dockPanel->elements[i].panelWindows[i2]->windows[i3];
					
					if (wnd->dockPanelTabRect.left == wnd->dockPanelTabRect.right)
						continue;

					c->gpu->setClipRect(&wnd->dockPanelTabRect);

					if (c->dockPanel->elements[i].panelWindows[i2]->activeWindow == wnd)
					{
						c->gpu->drawRectangle(mgDrawRectangleReason_dockTabWindowTitle,
							&wnd->dockPanelTabRect,
							&c->activeStyle->dockpanelTabActiveWindowTitleBG,
							&c->activeStyle->dockpanelTabActiveWindowTitleBG,
							0, 0, 0);
					}
					else
					{
						c->gpu->drawRectangle(mgDrawRectangleReason_dockTabWindowTitle,
							&wnd->dockPanelTabRect,
							&c->activeStyle->dockpanelTabWindowTitleBG,
							&c->activeStyle->dockpanelTabWindowTitleBG,
							0, 0, 0);
					}
					if (wnd->titlebarText)
					{
						mgPoint p;
						p.x = wnd->dockPanelTabRect.left+3;
						p.y = wnd->dockPanelTabRect.top;
						c->gpu->drawText(0, &p, 
							wnd->titlebarText, 
							wnd->titlebarTextLen, 
							&c->activeStyle->windowTitlebarTextColor, 
							wnd->titlebarFont);
					}
				}
				
			}
		}


		int reason = mgDrawRectangleReason_dockPanelSplitterBGHor;
		for (int i = 0; i < c->dockPanel->arrayWindowsSize; ++i)
		{
			c->gpu->setClipRect(&c->dockPanel->arrayWindows[i]->splitterRect);
			switch (c->dockPanel->arrayWindows[i]->where)
			{
			case 0:
			case 2:
				reason = mgDrawRectangleReason_dockPanelSplitterBGVert;
				break;
			}
			c->gpu->drawRectangle(reason, &c->dockPanel->arrayWindows[i]->splitterRect,
				&c->activeStyle->dockpanelPanelSplitterBGColor,
				&c->activeStyle->dockpanelPanelSplitterBGColor,
				0, 0, 0);
		}
	}

	if (c->dockPanel->flags & mgDockPanelFlag_drawSplitterBG)
	{
		int reason = mgDrawRectangleReason_dockSplitterBGHor;
		for (int i = 0; i < c->dockPanel->elementsSize; ++i)
		{
			c->gpu->setClipRect(&c->dockPanel->elements[i].splitterRect);
			switch (c->dockPanel->elements[i].info.where)
			{
			case 0:
			case 2:
				reason = mgDrawRectangleReason_dockSplitterBGVert;
				break;
			}
			c->gpu->drawRectangle(reason, &c->dockPanel->elements[i].splitterRect,
				&c->activeStyle->dockpanelSplitterBGColor,
				&c->activeStyle->dockpanelSplitterBGColor,
				0, 0, 0);
		}
	}

	if (g_windowToDockPanelMode)
	{
		c->gpu->setClipRect(&g_windowToDockRect);
		c->gpu->drawRectangle(mgDrawRectangleReason_dockWindowToDock, &g_windowToDockRect,
			&c->activeStyle->dockpanelWindowToDockColor,
			&c->activeStyle->dockpanelWindowToDockColor,
			0, 0, 0);
	}
}

void
mgDockPanelUpdate(struct mgContext_s* c)
{
	int needRebuild = 0;

	static mgPoint firstClick;
	//static int sizeOnClick = 0;
	static float splitterLerpTarget = 0.f;

	if(!g_dockpanel_splitterModeElement)
		g_dockpanel_inSplitterRect = 0;

	if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
	{
		firstClick = c->input->mousePosition;
		g_blockSplitterMode = 0;

		for (int i2 = 0; i2 < c->dockPanel->elementsSize; ++i2)
		{
			c->dockPanel->elements[i2].sizeOnClick = c->dockPanel->elements[i2].info.size;
		}
	}

	for (int i = 0; i < c->dockPanel->elementsSize; ++i)
	{
		if (!c->dockPanel->elements[i].panelWindows)
			continue;

		if (mgPointInRect(&c->dockPanel->elements[i].splitterRect, &c->input->mousePosition))
		{
			g_dockpanel_inSplitterRect = 1;
			switch (c->dockPanel->elements[i].info.where)
			{
			case 0:
			case 2:
				if (!g_dockpanel_splitterModeElement)
					mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeWE], mgCursorType_Arrow);
				break;
			default:
				if (!g_dockpanel_splitterModeElement)
					mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeNS], mgCursorType_Arrow);
				break;
			}
			c->dockPanel->flags |= mgDockPanelFlag_cursorChanged;

			if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				if (!g_dockpanel_splitterModeElement)
				{
					g_dockpanel_splitterModeElement = &c->dockPanel->elements[i];
					c->dockPanel->flags |= mgDockPanelFlag_onSplitter;

					//sizeOnClick = g_dockpanel_splitterModeElement->info.size;
					splitterLerpTarget = (float)g_dockpanel_splitterModeElement->sizeOnClick;
					break;
				}
			}
		}
		else if (mgPointInRect(&c->dockPanel->elements[i].rect, &c->input->mousePosition))
		{
			for (int i2 = 0; i2 < c->dockPanel->elements[i].panelWindowsSize; ++i2)
			{
				if (mgPointInRect(&c->dockPanel->elements[i].panelWindows[i2]->rect, &c->input->mousePosition))
				{
					for (int i3 = 0; i3 < c->dockPanel->elements[i].panelWindows[i2]->windowsSize; ++i3)
					{
						mgWindow* wnd = c->dockPanel->elements[i].panelWindows[i2]->windows[i3];
						if (mgPointInRect(&wnd->dockPanelTabRect, &c->input->mousePosition))
						{
							if (c->input->mouseButtonFlags1 & MG_MBFL_RMBUP)
							{
								c->dockPanel->elements[i].panelWindows[i2]->activeWindow = wnd;
								needRebuild = 1;
								if (c->dockPanel->windowTabPopup)
								{
									mgPoint pp = c->input->mousePosition;
									g_pnlWnd_onPopup = c->dockPanel->elements[i].panelWindows[i2];
									mgShowPopup_f(c, c->dockPanel->windowTabPopup, &pp);
									return;
								}
							}

							if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
							{
								//VSOutput("Window");
								c->dockPanel->elements[i].panelWindows[i2]->activeWindow = wnd;
								needRebuild = 1;
							}
						}
					}
				}
			}
		}
	}

	if (!g_dockpanel_inSplitterRect)
	{
		for (int i = 0; i < c->dockPanel->arrayWindowsSize; ++i)
		{
			if (mgPointInRect(&c->dockPanel->arrayWindows[i]->splitterRect, &c->input->mousePosition))
			{
				g_dockpanel_inSplitterRect = 1;
				switch (c->dockPanel->arrayWindows[i]->where)
				{
				case 0:
				case 2:
					if (!g_dockpanel_splitterModePanel)
						mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeWE], mgCursorType_Arrow);
					break;
				default:
					if (!g_dockpanel_splitterModePanel)
						mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeNS], mgCursorType_Arrow);
					break;
				}
				c->dockPanel->flags |= mgDockPanelFlag_cursorChanged;
				if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				{
					if (!g_dockpanel_splitterModePanel)
					{
						g_dockpanel_splitterModePanel = c->dockPanel->arrayWindows[i];
						g_dockpanel_splitterModePanel->sizeOnClick = g_dockpanel_splitterModePanel->sz;
						c->dockPanel->flags |= mgDockPanelFlag_onSplitter;
						break;
					}
				}
			}
		}
	}

	if (!g_dockpanel_inSplitterRect && (c->dockPanel->flags & mgDockPanelFlag_cursorChanged)
		&& !g_dockpanel_splitterModeElement
		&& !g_dockpanel_splitterModePanel)
	{
		mgSetCursor_f(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		c->dockPanel->flags ^= mgDockPanelFlag_cursorChanged;
	}

	if (g_dockpanel_splitterModePanel)
	{
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			g_dockpanel_splitterModePanel = 0;
			if (c->dockPanel->flags & mgDockPanelFlag_onSplitter)
				c->dockPanel->flags ^= mgDockPanelFlag_onSplitter;
			mgSetCursor_f(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		}
	}
	static int panelSz = 0;
	if (g_dockpanel_splitterModePanel && !g_blockSplitterMode)
	{
		int px = c->input->mousePosition.x - firstClick.x;
		int py = c->input->mousePosition.y - firstClick.y;

		switch (g_dockpanel_splitterModePanel->where)
		{
		case 0:
			splitterLerpTarget = (float)(g_dockpanel_splitterModePanel->sizeOnClick + px);
			break;
		case 1:
			splitterLerpTarget = (float)(g_dockpanel_splitterModePanel->sizeOnClick + py);
			break;
		case 2:
			splitterLerpTarget = (float)(g_dockpanel_splitterModePanel->sizeOnClick - px);
			break;
		case 3:
			splitterLerpTarget = (float)(g_dockpanel_splitterModePanel->sizeOnClick - py);
			break;
		}
		panelSz = g_dockpanel_splitterModePanel->sz;
		g_dockpanel_splitterModePanel->sz = (int)lerp((float)g_dockpanel_splitterModePanel->sz, splitterLerpTarget, c->deltaTime * 30.f);
		if (g_dockpanel_splitterModePanel->sz < c->dockPanel->tabHeight)
			g_dockpanel_splitterModePanel->sz = c->dockPanel->tabHeight;

		needRebuild = 1;
	}
	/*not else*/if (g_dockpanel_splitterModePanel && g_blockSplitterMode)
	{
		g_dockpanel_splitterModePanel->sz = panelSz;

		switch (g_mouseMoveDirBeforeBlock)
		{
		case 1:
			if (c->input->mouseMoveDelta.x > 0)
				g_blockSplitterMode = 0;
			break;
		case 2:
			if (c->input->mouseMoveDelta.x < 0)
				g_blockSplitterMode = 0;
			break;
		case 3:
			if (c->input->mouseMoveDelta.y < 0)
				g_blockSplitterMode = 0;
			break;
		case 4:
			if (c->input->mouseMoveDelta.y > 0)
				g_blockSplitterMode = 0;
			break;
		}
	}

	if (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
	{
		g_windowSize = 0;
	}

	if (g_dockpanel_splitterModeElement)
	{
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			g_dockpanel_splitterModeElement = 0;
			if (c->dockPanel->flags & mgDockPanelFlag_onSplitter)
				c->dockPanel->flags ^= mgDockPanelFlag_onSplitter;
			mgSetCursor_f(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		}
	}

	if (g_dockpanel_splitterModeElement)
	{
		int px = c->input->mousePosition.x - firstClick.x;
		int py = c->input->mousePosition.y - firstClick.y;

		switch (g_dockpanel_splitterModeElement->info.where)
		{
		case 0:
			splitterLerpTarget = (float)(g_dockpanel_splitterModeElement->sizeOnClick + px);
			break;
		case 1:
			splitterLerpTarget = (float)(g_dockpanel_splitterModeElement->sizeOnClick + py);
			break;
		case 2:
			splitterLerpTarget = (float)(g_dockpanel_splitterModeElement->sizeOnClick - px);
			break;
		case 3:
			splitterLerpTarget = (float)(g_dockpanel_splitterModeElement->sizeOnClick - py);
			break;
		}
		int sizeOld = g_dockpanel_splitterModeElement->info.size;
		g_dockpanel_splitterModeElement->info.size = (int)lerp((float)g_dockpanel_splitterModeElement->info.size, splitterLerpTarget, c->deltaTime * 30.f);

		if (g_dockpanel_splitterModeElement->info.size > g_dockpanel_splitterModeElement->info.sizeMaximum)
			g_dockpanel_splitterModeElement->info.size = g_dockpanel_splitterModeElement->info.sizeMaximum;
		if (g_dockpanel_splitterModeElement->info.size < g_dockpanel_splitterModeElement->info.sizeMinimum)
			g_dockpanel_splitterModeElement->info.size = g_dockpanel_splitterModeElement->info.sizeMinimum;
		needRebuild = 1;
	}
	//mgDockPanelUpdateWindow(c);
	
	if (!g_dockpanel_splitterModeElement)
	{
		if (c->dockPanel->arrayWindows && c->dockPanel->arrayWindowsSize)
		{
			for (int i = 0; i < c->dockPanel->arrayWindowsSize; ++i)
			{
				mgUpdateElement(c->dockPanel->arrayWindows[i]->activeWindow->rootElement);

				//mgUpdateWindow(c->dockPanel->arrayWindows[i]);
			}
		}
	}

	if(needRebuild)
	{ 
		mgDockPanelRebuild(c);
		mgDockPanelUpdateWindow(c);
	}
}

mgColor colors[] = {
	{1.f, 0.f, 0.f, 1.f},
	{0.f, 1.f, 0.f, 1.f},
	{0.f, 0.f, 1.f, 1.f},

	{1.f, 1.f, 0.f, 1.f},
	{0.f, 1.f, 1.f, 1.f},
	{1.f, 0.f, 1.f, 1.f},
};

MG_API
void MG_C_DECL
mgInitDockPanel_f(
	struct mgContext_s* c, 
	int indentLeft, 
	int indentTop, 
	int indentRight, 
	int indentBottom, 
	mgDockPanelElementCreationInfo* elements, 
	int elementsSize)
{
	assert(c);
	assert(!c->dockPanel);
	assert(elementsSize > 0);
	assert(indentLeft >= 0);
	assert(indentTop >= 0);
	assert(indentRight >= 0);
	assert(indentBottom >= 0);

	c->dockPanel = calloc(1, sizeof(struct mgDockPanel_s));
	c->dockPanel->indent.left = indentLeft;
	c->dockPanel->indent.top = indentTop;
	c->dockPanel->indent.right = indentRight;
	c->dockPanel->indent.bottom = indentBottom;
	c->dockPanel->flags = mgDockPanelFlag_drawSplitterBG;
	c->dockPanel->splitterWidth = 2;
	c->dockPanel->mainElementSizeMinimum.x = 300;
	c->dockPanel->mainElementSizeMinimum.y = 300;
	c->dockPanel->tabHeight = 25;

	if (c->defaultPopupFont)
	{
		struct mgPopupNode_s popupNodes[] =
		{
			{L"Make first", 0, 0, dockPanel_popupCallback_makeFirst},
			{L"Unpin\\remove", 0, 0, dockPanel_popupCallback_unpin},
			{L"Close\\hide", 0, 0, dockPanel_popupCallback_close},
		};
		c->dockPanel->windowTabPopup = mgCreatePopup_f(popupNodes, 3, c->defaultPopupFont);
	}

	c->dockPanel->elementsSize = elementsSize + 1;
	c->dockPanel->elements = calloc(1, sizeof(mgDockPanelElement) * c->dockPanel->elementsSize);
	mgColorSetAsIntegerRGB(&c->dockPanel->elements[0].colorBG, 0xFFFFFF);
	static int colorCount = 0;
	for (int i = 0; i < elementsSize; ++i)
	{
		c->dockPanel->elements[i + 1].info = elements[i];
		c->dockPanel->elements[i + 1].colorBG = colors[colorCount];

		colorCount++;
		if (colorCount > 5)
			colorCount = 0;
	}

	mgDockPanelOnSize(c);
}

void
mgDockUpdateArrayWindows(struct mgContext_s* c)
{
	if (!c->dockPanel)
		return;
	
	if (c->dockPanel->arrayWindows)
		free(c->dockPanel->arrayWindows);

	c->dockPanel->arrayWindows = 0;
	c->dockPanel->arrayWindowsSize = 0;

	if (c->dockPanel->elementsSize < 2)
		return;

	for (int i = 1; i < c->dockPanel->elementsSize; ++i)
	{
		mgDockPanelElement * el = &c->dockPanel->elements[i];
		if (!el->panelWindows)
			continue;

		for (int o = 0; o < el->panelWindowsSize; ++o)
		{
			if (el->panelWindows[o]->activeWindow)
				++c->dockPanel->arrayWindowsSize;
		}
	}

	if (c->dockPanel->arrayWindowsSize)
	{
		c->dockPanel->arrayWindows = malloc(c->dockPanel->arrayWindowsSize * sizeof(mgDockPanelWindow*));
		mgDockPanelWindow** wptr = c->dockPanel->arrayWindows;

		for (int i = 1; i < c->dockPanel->elementsSize; ++i)
		{
			mgDockPanelElement* el = &c->dockPanel->elements[i];
			if (!el->panelWindows)
				continue;

			for (int o = 0; o < el->panelWindowsSize; ++o)
			{
				if (el->panelWindows[o]->activeWindow)
				{
					*wptr = el->panelWindows[o];
					++wptr;
				}
			}
		}
	}

}

void
put_new_window_to_panel(mgDockPanelWindow* pnlWnd, struct mgWindow_s* w, int onFirst)
{
	pnlWnd->windowsSize++;
	struct mgWindow_s** new_windows = calloc(1, sizeof(struct mgWindow_s*) * pnlWnd->windowsSize);
	if (pnlWnd->windowsSize > 1)
		memcpy(new_windows, pnlWnd->windows, sizeof(struct mgWindow_s*) * (pnlWnd->windowsSize - 1));
	new_windows[pnlWnd->windowsSize - 1] = w;
	
	if (onFirst)
	{
		for (int i = pnlWnd->windowsSize; i > 1; i--)
		{
			new_windows[i - 1] = new_windows[i - 2];
		}
		new_windows[0] = w;
	}

	if (pnlWnd->windows)
		free(pnlWnd->windows);
	pnlWnd->windows = new_windows;
}

mgDockPanelWindow*
add_new_dock_panel_window(mgDockPanelElement* dckEl, struct mgDockPanelWindow_s* parent, struct mgWindow_s* w)
{
	mgDockPanelWindow* pnlWnd = calloc(1, sizeof(mgDockPanelWindow));
	pnlWnd->dockElement = dckEl;
	pnlWnd->parent = parent;
	
	put_new_window_to_panel(pnlWnd, w, 0);

	pnlWnd->activeWindow = w;
	pnlWnd->where = 4;

	dckEl->panelWindowsSize++;
	mgDockPanelWindow** pnlWindows = calloc(1, sizeof(mgDockPanelElement*) * dckEl->panelWindowsSize);
	memcpy(pnlWindows, dckEl->panelWindows, sizeof(mgDockPanelElement*) * (dckEl->panelWindowsSize-1));
	pnlWindows[dckEl->panelWindowsSize - 1] = pnlWnd;
	if (dckEl->panelWindows)
		free(dckEl->panelWindows);
	dckEl->panelWindows = pnlWindows;
	return pnlWnd;
}

MG_API
struct mgDockPanelWindow_s* MG_C_DECL
mgDockAddWindow_f(struct mgWindow_s* w, struct mgDockPanelWindow_s* dw, int id)
{
	assert(w);
	assert(w->context->dockPanel);
	assert(w->flags & mgWindowFlag_canDock);
	
	mgDockPanel* dck = w->context->dockPanel;

	mgDockPanelWindow* pnlWnd = 0;	

	//static int pnlid = 1;

	if (dw)
	{
		/*id == 4 - add new window in mgDockPanelWindow::windows*/
		switch (id)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		{
			mgDockPanelElement* dckEl = dw->dockElement;
			pnlWnd = add_new_dock_panel_window(dckEl, dw, w);
			pnlWnd->where = id;
			w->dockPanelWindow = pnlWnd;
		//	++pnlid;
			switch (id)
			{
			case 0:
			case 2:
				//pnlWnd->sz = g_windowToDockRect.right - g_windowToDockRect.left;
				pnlWnd->sz = dw->rect.right - dw->rect.left;
				break;
			case 1:
			case 3:
				//pnlWnd->sz = g_windowToDockRect.bottom - g_windowToDockRect.top;
				pnlWnd->sz = dw->rect.bottom - dw->rect.top;
				break;
			}
			if (pnlWnd->sz > 3)
				pnlWnd->sz /= 2;
		}break;
		case 4:
		default:
			pnlWnd = dw;
			put_new_window_to_panel(pnlWnd, w, 1);
			w->dockPanelWindow = pnlWnd;
			pnlWnd->activeWindow = w;
			break;
		}
	}
	else
	{
		id++;
		if ((id > 0) && (id < dck->elementsSize))
		{
			mgDockPanelElement* dckEl = &dck->elements[id];			
			pnlWnd = add_new_dock_panel_window(dckEl, 0, w);
			
			w->dockPanelWindow = pnlWnd;

			switch (dckEl->info.where)
			{
			case 0:
			case 2:
				if(dckEl->info.size < w->size.x)
					dckEl->info.size = w->size.x;
				break;
			case 1:
			case 3:
				if (dckEl->info.size < w->size.y)
					dckEl->info.size = w->size.y;
				break;
			}

		}
		else
		{
		}
	}

	if (pnlWnd)
	{
		/*pnlWnd->windowsSize++;
		struct mgWindow_s** new_windows = calloc(1, sizeof(struct mgWindow_s*) * pnlWnd->windowsSize);
		memcpy(new_windows, pnlWnd->windows, sizeof(struct mgWindow_s*) * (pnlWnd->windowsSize - 1));
		new_windows[pnlWnd->windowsSize - 1] = w;
		free(pnlWnd->windows);
		pnlWnd->windows = new_windows;*/

		mgDockPanelRebuild(w->context);
	}
	
	mgDockUpdateArrayWindows(w->context);

	if (pnlWnd)
		mgDockPanelUpdateWindow(w->context);

	return pnlWnd;
}

void dockPanel_popupCallback_makeFirst()
{
	mgWindow* aw = 0;
	for (int i = g_pnlWnd_onPopup->windowsSize; i > 1; i--)
	{
		if (g_pnlWnd_onPopup->activeWindow == g_pnlWnd_onPopup->windows[i - 1])
			aw = g_pnlWnd_onPopup->activeWindow;

		if (aw)
		{
			g_pnlWnd_onPopup->windows[i - 1] = g_pnlWnd_onPopup->windows[i - 2];
		}
	}

	if (aw)
	{
		g_pnlWnd_onPopup->windows[0] = aw;
		mgDockPanelRebuild(aw->context);
	}
}

void dockPanel_popupCallback_unpin()
{
	mgWindow * w = g_pnlWnd_onPopup->activeWindow;
	
	w->position.x = g_pnlWnd_onPopup->rect.left;
	w->position.y = g_pnlWnd_onPopup->rect.top;

	if (g_pnlWnd_onPopup->windowsSize > 1)
	{
		struct mgWindow_s** newWindows = malloc(sizeof(struct mgWindow_s*) * (g_pnlWnd_onPopup->windowsSize - 1));
		for (int i = 0, i2 = 0; i < g_pnlWnd_onPopup->windowsSize; ++i)
		{
			if (g_pnlWnd_onPopup->windows[i] == w)
				continue;

			newWindows[i2] = g_pnlWnd_onPopup->windows[i];
			i2++;
		}

		g_pnlWnd_onPopup->activeWindow = newWindows[0];

		free(g_pnlWnd_onPopup->windows);
		g_pnlWnd_onPopup->windows = newWindows;

		g_pnlWnd_onPopup->windowsSize--;
	}
	else
	{
		g_pnlWnd_onPopup->windowsSize = 0;

		free(g_pnlWnd_onPopup->windows);

		g_pnlWnd_onPopup->windows = 0;
		g_pnlWnd_onPopup->activeWindow = 0;

		struct mgDockPanelElement_s* de = g_pnlWnd_onPopup->dockElement;
		if (de->panelWindowsSize > 1)
		{
			/*change parents*/
			struct mgDockPanelWindow_s* grndPrnt = g_pnlWnd_onPopup->parent;
			for (int i = 0; i < de->panelWindowsSize; ++i)
			{
				if (de->panelWindows[i] == g_pnlWnd_onPopup)
					continue;

				if (de->panelWindows[i]->parent == g_pnlWnd_onPopup)
				{
					de->panelWindows[i]->sz = g_pnlWnd_onPopup->sz;
					de->panelWindows[i]->where = g_pnlWnd_onPopup->where;
					if (de->panelWindows[i]->where == 4)
						mgRectSet(&de->panelWindows[i]->splitterRect, 0, 0, 0, 0);
					de->panelWindows[i]->parent = grndPrnt;
				}
			}

			/*remove g_pnlWnd_onPopup from dockElement*/
			mgDockPanelWindow** newPanels = malloc(sizeof(mgDockPanelWindow*) * (de->panelWindowsSize - 1));
			for (int i = 0, i2 = 0; i < de->panelWindowsSize; ++i)
			{
				if (de->panelWindows[i] == g_pnlWnd_onPopup)
					continue;

				newPanels[i2] = de->panelWindows[i];
				i2++;
			}
			
			free(g_pnlWnd_onPopup);
			g_pnlWnd_onPopup = 0;

			free(de->panelWindows);
			de->panelWindows = newPanels;
			de->panelWindowsSize--;
		}
		else
		{
			de->panelWindowsSize = 0;
			free(de->panelWindows);
			de->panelWindows = 0;
		}
	}
	
	w->dockPanelWindow = 0;
	mgDockPanelRebuild(w->context);
	mgDockUpdateArrayWindows(w->context);
	mgDockPanelUpdateWindow(w->context);
}

void dockPanel_popupCallback_close()
{
	mgWindow* w = g_pnlWnd_onPopup->activeWindow;
	dockPanel_popupCallback_unpin();
	mgShowWindow_f(w, 0);
}