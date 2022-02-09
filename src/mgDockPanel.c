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

#include <Windows.h>

extern int g_windowToDockPanelMode;
extern mgRect g_windowToDockRect;

float lerp(float v0, float v1, float t);

int g_dockpanel_inSplitterRect = 0;
struct mgDockPanelElement_s* g_dockpanel_splitterMode = 0;

void mgDrawWindow(struct mgWindow_s* w);
void mgUpdateWindow(struct mgWindow_s* w);
void mgUpdateElement(mgElement* e);

void
_mgDockPanelRebuild(struct mgContext_s* c)
{
	c->dockPanel->elements[0].rect = c->dockPanel->rect;
	mgRect mainElRect = c->dockPanel->elements[0].rect;

	if (c->dockPanel->elementsNum > 1)
	{
		for (int i = 1; i < c->dockPanel->elementsNum; ++i)
		{
			int sz = c->dockPanel->elements[i].info.size;

			if (!c->dockPanel->elements[i].firstWindow) sz = 0;

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
mgDockPanelRebuild(struct mgContext_s* c)
{
	_mgDockPanelRebuild(c);
	if (c->dockPanel->elementsNum > 1)
	{
		if (c->dockPanel->mainElementSize.x < c->dockPanel->mainElementSizeMinimum.x)
		{
			int dif = c->dockPanel->mainElementSizeMinimum.x - c->dockPanel->mainElementSize.x;
		
			for (int i = 1; i < c->dockPanel->elementsNum; ++i)
			{
				switch (c->dockPanel->elements[i].info.where)
				{
				case 0:
				case 2:
					c->dockPanel->elements[i].info.size -= dif;
					if (c->dockPanel->elements[i].info.size < c->dockPanel->elements[i].info.sizeMinimum)
						c->dockPanel->elements[i].info.size = c->dockPanel->elements[i].info.sizeMinimum;
					break;
				}
			}
			_mgDockPanelRebuild(c);
		}

		if (c->dockPanel->mainElementSize.y < c->dockPanel->mainElementSizeMinimum.y)
		{
			int dif = c->dockPanel->mainElementSizeMinimum.y - c->dockPanel->mainElementSize.y;

			for (int i = 1; i < c->dockPanel->elementsNum; ++i)
			{
				switch (c->dockPanel->elements[i].info.where)
				{
				case 1:
				case 3:
					c->dockPanel->elements[i].info.size -= dif;
					if (c->dockPanel->elements[i].info.size < c->dockPanel->elements[i].info.sizeMinimum)
						c->dockPanel->elements[i].info.size = c->dockPanel->elements[i].info.sizeMinimum;
					break;
				}
			}
			_mgDockPanelRebuild(c);
		}

		for (int i = 1; i < c->dockPanel->elementsNum; ++i)
		{
			if (!c->dockPanel->elements[i].firstWindow)
				continue;

			mgDockPanelWindow* cw = c->dockPanel->elements[i].firstWindow;
			mgDockPanelWindow* lw = cw->left;
			while (1)
			{
				cw->rect = c->dockPanel->elements[i].rect;
				cw->windowRect = cw->rect;

				if (cw == lw)
					break;
				cw = cw->right;
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
			mgUpdateWindow(c->dockPanel->arrayWindows[i]);
		}
	}
}

void
mgDockPanelOnSize(struct mgContext_s* c)
{
	c->dockPanel->rect.left = c->dockPanel->indent.left;
	c->dockPanel->rect.top = c->dockPanel->indent.top;
	c->dockPanel->rect.right = c->windowSize.x - c->dockPanel->indent.right;
	c->dockPanel->rect.bottom = c->windowSize.y - c->dockPanel->indent.bottom;
	c->dockPanel->size.x = c->dockPanel->rect.right - c->dockPanel->rect.left;
	c->dockPanel->size.y = c->dockPanel->rect.bottom - c->dockPanel->rect.top;
	mgDockPanelRebuild(c);
	mgDockPanelUpdateWindow(c);
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

	for (int i = 0; i < c->dockPanel->elementsNum; ++i)
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
			mgDrawWindow(c->dockPanel->arrayWindows[i]);
		}
	}

	if (c->dockPanel->flags & mgDockPanelFlag_drawSplitterBG)
	{
		for (int i = 0; i < c->dockPanel->elementsNum; ++i)
		{
			c->gpu->setClipRect(&c->dockPanel->elements[i].splitterRect);
			c->gpu->drawRectangle(mgDrawRectangleReason_dockSplitterBG, &c->dockPanel->elements[i].splitterRect,
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
	static mgPoint firstClick;
	static int sizeOnClick = 0;
	static float splitterLerpTarget = 0.f;

	if(!g_dockpanel_splitterMode)
		g_dockpanel_inSplitterRect = 0;

	if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
	{
		firstClick = c->input->mousePosition;
	}

	for (int i = 0; i < c->dockPanel->elementsNum; ++i)
	{
		if (!c->dockPanel->elements[i].firstWindow)
			continue;

		if (mgPointInRect(&c->dockPanel->elements[i].splitterRect, &c->input->mousePosition))
		{
			g_dockpanel_inSplitterRect = 1;
			switch (c->dockPanel->elements[i].info.where)
			{
			case 0:
			case 2:
				if (!g_dockpanel_splitterMode)
					mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeWE], mgCursorType_Arrow);
				break;
			default:
				if (!g_dockpanel_splitterMode)
					mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeNS], mgCursorType_Arrow);
				break;
			}
			c->dockPanel->flags |= mgDockPanelFlag_cursorChanged;

			if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				if (!g_dockpanel_splitterMode)
				{
					g_dockpanel_splitterMode = &c->dockPanel->elements[i];
					c->dockPanel->flags |= mgDockPanelFlag_onSplitter;
					sizeOnClick = g_dockpanel_splitterMode->info.size;
					splitterLerpTarget = (float)sizeOnClick;
				}
			}
		}
	}
	if (!g_dockpanel_inSplitterRect && (c->dockPanel->flags & mgDockPanelFlag_cursorChanged)
		&& !g_dockpanel_splitterMode)
	{
		mgSetCursor_f(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		c->dockPanel->flags ^= mgDockPanelFlag_cursorChanged;
	}

	if (g_dockpanel_splitterMode)
	{
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			g_dockpanel_splitterMode = 0;
			if (c->dockPanel->flags & mgDockPanelFlag_onSplitter)
				c->dockPanel->flags ^= mgDockPanelFlag_onSplitter;
			mgSetCursor_f(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		}
	}

	if (g_dockpanel_splitterMode)
	{
		int px = c->input->mousePosition.x - firstClick.x;
		int py = c->input->mousePosition.y - firstClick.y;

		switch (g_dockpanel_splitterMode->info.where)
		{
		case 0:
			splitterLerpTarget = (float)(sizeOnClick + px);
			break;
		case 1:
			splitterLerpTarget = (float)(sizeOnClick + py);
			break;
		case 2:
			splitterLerpTarget = (float)(sizeOnClick - px);
			break;
		case 3:
			splitterLerpTarget = (float)(sizeOnClick - py);
			break;
		}
		int sizeOld = g_dockpanel_splitterMode->info.size;
		g_dockpanel_splitterMode->info.size = (int)lerp((float)g_dockpanel_splitterMode->info.size, splitterLerpTarget, c->deltaTime * 30.f);

		if (g_dockpanel_splitterMode->info.size > g_dockpanel_splitterMode->info.sizeMaximum)
			g_dockpanel_splitterMode->info.size = g_dockpanel_splitterMode->info.sizeMaximum;
		if (g_dockpanel_splitterMode->info.size < g_dockpanel_splitterMode->info.sizeMinimum)
			g_dockpanel_splitterMode->info.size = g_dockpanel_splitterMode->info.sizeMinimum;
		mgDockPanelRebuild(c);
		mgDockPanelUpdateWindow(c);
	}
	//mgDockPanelUpdateWindow(c);
	
	if (!g_dockpanel_splitterMode)
	{
		if (c->dockPanel->arrayWindows && c->dockPanel->arrayWindowsSize)
		{
			for (int i = 0; i < c->dockPanel->arrayWindowsSize; ++i)
			{
				mgUpdateElement(c->dockPanel->arrayWindows[i]->rootElement);

				//mgUpdateWindow(c->dockPanel->arrayWindows[i]);
			}
		}
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
	c->dockPanel->splitterWidth = 3;
	c->dockPanel->mainElementSizeMinimum.x = 300;
	c->dockPanel->mainElementSizeMinimum.y = 300;

	c->dockPanel->elementsNum = elementsSize + 1;
	c->dockPanel->elements = calloc(1, sizeof(mgDockPanelElement) * c->dockPanel->elementsNum);
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

	if (c->dockPanel->elementsNum < 2)
		return;

	for (int i = 1; i < c->dockPanel->elementsNum; ++i)
	{
		mgDockPanelElement * el = &c->dockPanel->elements[i];
		if (!el->firstWindow)
			continue;

		mgDockPanelWindow* cw = el->firstWindow;
		mgDockPanelWindow* lw = cw->left;
		while (1)
		{
			if (cw->activeWindow)
				++c->dockPanel->arrayWindowsSize;

			if (cw == lw)
				break;
			cw = cw->right;
		}

		if (!c->dockPanel->arrayWindowsSize)
			continue;

		c->dockPanel->arrayWindows = malloc(c->dockPanel->arrayWindowsSize * sizeof(struct mgWindow_s*));
		mgWindow** wptr = c->dockPanel->arrayWindows;

		cw = el->firstWindow;
		lw = cw->left;
		while (1)
		{
			if (cw->activeWindow)
			{
				*wptr = cw->activeWindow;
				++wptr;
			}

			if (cw == lw)
				break;
			cw = cw->right;
		}
	}
}

MG_API
struct mgDockPanelWindow_s* MG_C_DECL
mgDockAddWindow_f(struct mgWindow_s* w, struct mgDockPanelWindow_s* dw, int id)
{
	assert(w);
	assert(w->context->dockPanel);
	assert(w->flags & mgWindowFlag_canDock);
	
	mgDockPanel* dck = w->context->dockPanel;

	mgDockPanelWindow* dckWnd = 0;	

	if (dw)
	{

	}
	else
	{
		id++;
		if ((id > 0) && (id < dck->elementsNum))
		{
			mgDockPanelElement* dckEl = &dck->elements[id];

			dckWnd = calloc(1, sizeof(mgDockPanelWindow));
			
			dckWnd->firstWindow = calloc(1, sizeof(mgDockPanelWindowNode));
			dckWnd->firstWindow->window = w;
			dckWnd->firstWindow->left = dckWnd->firstWindow;
			dckWnd->firstWindow->right = dckWnd->firstWindow;

			dckWnd->left = dckWnd;
			dckWnd->right = dckWnd;
			
			dckEl->firstWindow = dckWnd;
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

			dckEl->firstWindow->activeWindow = w;
			w->dockPanelWindow = dckWnd;
		}
	}

	if (dckWnd)
		mgDockPanelRebuild(w->context);
	
	mgDockUpdateArrayWindows(w->context);

	if (dckWnd)
	{
		mgDockPanelUpdateWindow(w->context);
	}


	return dckWnd;
}
