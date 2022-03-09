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

extern struct mgDockPanelElement_s* g_dockpanel_splitterModeElement;
extern struct mgDockPanelWindow_s* g_dockpanel_splitterModePanel;

mgDockPanelWindow* g_dockPanelWindow = 0;
int g_dockElIdOrWhere = 0;
int g_windowToDockPanelMode = 0;
mgRect g_windowToDockRect;

void mgDestroyElement_f(mgElement* e);
void mgrootobject_cb(mgElement*e) {}
void mgDrawElement(mgElement* e);
void mgUpdateTransformElement(mgElement* e);
void mgDockPanelUpdateWindow(struct mgContext_s* c);

inline float 
lerp(float v0, float v1, float t) 
{
	return (1.f - t) * v0 + t * v1;
}



void
mgWindowUpdateContentHeight(mgWindow* w, mgElement* e)
{
	if (e != w->rootElement)
	{
		int h = 0;

		mgRect r;
		if (w->dockPanelWindow)
		{
			r = w->dockPanelWindow->rect;
			r.top = w->dockPanelWindow->tabRect.bottom;
			h = e->transformWorld.buildArea.bottom - r.top + (int)e->scrollValueWorld;
		}
		else
		{
			r = w->rect;

			if (w->flags & mgWindowFlag_withTitlebar)
				h = e->transformWorld.buildArea.bottom - (r.top + w->titlebarHeight) + (int)e->scrollValueWorld;
			else
				h = e->transformWorld.buildArea.bottom - r.top + (int)e->scrollValueWorld;
		}

		if (h > w->contentHeight)
			w->contentHeight = h;
	}
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgWindowUpdateContentHeight(w, e->children[i].pointer);
	}
}

MG_API
struct mgWindow_s*
MG_C_DECL
mgCreateWindow_f(struct mgContext_s* ctx, int px, int py, int sx, int sy)
{
	assert(ctx);
	mgWindow* newWindow = calloc(1, sizeof(mgWindow));
	newWindow->position.x = px;
	newWindow->position.y = py;
	newWindow->size.x = sx;
	newWindow->size.y = sy;
	newWindow->context = ctx;
	
	newWindow->flags 
		= mgWindowFlag_withTitlebar 
		| mgWindowFlag_canMove 
		| mgWindowFlag_drawBG 
		| mgWindowFlag_closeButton
		| mgWindowFlag_canResize
		| mgWindowFlag_collapseButton;
	
	newWindow->id = -1;

	newWindow->scrollbarWidth = 12;
	newWindow->titlebarHeight = 17;
	newWindow->flagsInternal
		= mgWindowFlag_internal_visible
		| mgWindowFlag_internal_isExpand;
	
	newWindow->left = newWindow;
	newWindow->right = newWindow;

	newWindow->sizeMinimum.x = 100;
	newWindow->sizeMinimum.y = 50;

	newWindow->rect.left = newWindow->position.x;
	newWindow->rect.top = newWindow->position.y;
	newWindow->rect.right = newWindow->rect.left + newWindow->size.x;
	newWindow->rect.bottom = newWindow->rect.top + newWindow->size.y;


	newWindow->rootElement = calloc(1, sizeof(mgElement));
	newWindow->rootElement->window = newWindow;
	newWindow->rootElement->visible = 1;
	newWindow->rootElement->onDraw = mgrootobject_cb;
	newWindow->rootElement->onUpdate = mgrootobject_cb;
	newWindow->rootElement->onUpdateTransform = mgrootobject_cb;
	newWindow->rootElement->onRebuild = mgrootobject_cb;

	newWindow->rootElement->transformLocal.buildArea = newWindow->rect;
	newWindow->rootElement->transformLocal.clipArea = newWindow->rect;
	newWindow->rootElement->transformWorld = newWindow->rootElement->transformLocal;
	newWindow->rootElement->creationRect = newWindow->rootElement->transformLocal.buildArea;
	newWindow->rootElement->transformLocal.clipArea.top += newWindow->titlebarHeight;

	if (ctx->firstWindow)
	{
		newWindow->right = ctx->firstWindow;
		newWindow->left = ctx->firstWindow->left;
		ctx->firstWindow->left->right = newWindow;
		ctx->firstWindow->left = newWindow;
	}
	else
	{
		ctx->firstWindow = newWindow;
	}

	return newWindow;
}

MG_API
void 
MG_C_DECL
mgDestroyWindow_f(struct mgWindow_s* w)
{
	assert(w);

	if (w->rootElement)
		mgDestroyElement_f(w->rootElement);

	free(w);
}

MG_API
void
MG_C_DECL
mgBringWindowToTop_f(struct mgWindow_s* w)
{
	if (w->flags & mgWindowFlag_noToTop)
		return;

	struct mgContext_s* c = w->context;

	mgWindow* r = w->right;
	mgWindow* l = w->left;
	r->left = l;
	l->right = r;

	if (c->firstWindow == w)
		c->firstWindow = c->firstWindow->right;
	if (c->firstWindow == w)
		c->firstWindow = 0;

	if (c->firstWindow)
	{
		w->right = c->firstWindow;
		w->left = c->firstWindow->left;
		c->firstWindow->left->right = w;
		c->firstWindow->left = w;
	}
	else
	{
		c->firstWindow = w;
		w->right = w;
		w->left = w;
	}
}

void
mgDrawWindow(struct mgWindow_s* w)
{
	assert(w);
	mgIconGroup* iconGroup = w->context->defaultIconGroup;
	mgStyle* style = w->userStyle ? w->userStyle : w->context->activeStyle;

	mgColor clrbg = w->context->firstWindow->left == w ? style->windowBGTopWindow : style->windowBG;
	mgColor clrttl = w->context->firstWindow->left == w ? style->windowTitlebarTopWindow : style->windowTitlebar;

	w->context->gpu->setClipRect(&w->rect);
	
	if ((w->flags & mgWindowFlag_canDock) && w->dockPanelWindow)
	{

	}
	else
	{
		if ((w->flags & mgWindowFlag_drawBG)
			&& (w->flagsInternal & mgWindowFlag_internal_isExpand))
			w->context->gpu->drawRectangle(mgDrawRectangleReason_windowBG, w, &w->rect, &clrbg, &clrbg, 0, 0);

		if ((w->flags & mgWindowFlag_withTitlebar) && !w->dockPanelWindow)
		{
			float tbhalfsz = (float)w->titlebarHeight * 0.5f;

			w->rectTitlebar.left = w->position.x;
			w->rectTitlebar.top = w->position.y;
			w->rectTitlebar.right = w->position.x + w->size.x;
			w->rectTitlebar.bottom = w->position.y + w->titlebarHeight;

			w->context->gpu->drawRectangle(mgDrawRectangleReason_windowTitlebar, w,
				&w->rectTitlebar, &clrttl, &clrttl, 0, 0);

			int text_move = 0;
			if (w->flags & mgWindowFlag_collapseButton)
			{
				if (iconGroup)
				{
					mgPoint pos;
					pos.x = w->position.x + 3;
					pos.y = w->position.y;
					int iconID = iconGroup->windowExpandButton;

					if (w->flagsInternal & mgWindowFlag_internal_isExpand)
						iconID = iconGroup->windowCollapseButton;

					mgPoint sz = iconGroup->icons->iconNodes[iconID].sz;


					text_move = sz.x + 3;

					{
						float ichalfsz = (float)sz.y * 0.5f;
						pos.y += (int)(tbhalfsz - ichalfsz);
					}

					w->collapseButtonRect.left = pos.x;
					w->collapseButtonRect.top = pos.y;
					w->collapseButtonRect.right = w->collapseButtonRect.left + sz.x;
					w->collapseButtonRect.bottom = w->collapseButtonRect.top + sz.y;

					w->context->currentIcon.left = iconGroup->icons->iconNodes[iconID].lt.x;
					w->context->currentIcon.top = iconGroup->icons->iconNodes[iconID].lt.y;
					w->context->currentIcon.right = iconGroup->icons->iconNodes[iconID].sz.x;
					w->context->currentIcon.bottom = iconGroup->icons->iconNodes[iconID].sz.y;
					w->context->gpu->drawRectangle(mgDrawRectangleReason_windowCollapseButton, w,
						&w->collapseButtonRect, &w->context->whiteColor, &w->context->whiteColor, 
						iconGroup->icons->texture, 0);
				}
			}

			if (w->titlebarFont && w->titlebarText)
			{
				mgPoint pos;
				pos.x = w->position.x + text_move + 3;
				pos.y = w->position.y;

				w->context->gpu->setClipRect(&w->rect);
				w->context->gpu->drawText(mgDrawTextReason_windowTitlebar, w,
					&pos,
					w->titlebarText,
					w->titlebarTextLen,
					&style->windowTitlebarText,
					w->titlebarFont);
			}

			if (w->flags & mgWindowFlag_closeButton)
			{
				if (iconGroup)
				{
					mgColor wh;
					mgColorSet(&wh, 1.f, 1.f, 1.f, 1.f);
					mgPoint sz = iconGroup->icons->iconNodes[iconGroup->windowCloseButton].sz;
					mgPoint pos;
					pos.x = w->position.x + w->size.x - sz.x - 3;
					pos.y = w->position.y;

					{
						float ichalfsz = (float)sz.y * 0.5f;
						pos.y += (int)(tbhalfsz - ichalfsz);
					}

					int iconID = iconGroup->windowCloseButton;

					w->closeButtonRect.left = pos.x;
					w->closeButtonRect.top = pos.y;
					w->closeButtonRect.right = w->closeButtonRect.left + sz.x;
					w->closeButtonRect.bottom = w->closeButtonRect.top + sz.y;

					if (w->cursorInfo == mgWindowCursorInfo_closeButton)
					{
						iconID = iconGroup->windowCloseButtonMouseHover;

					}
					if (w->flagsInternal & mgWindowFlag_internal_isCloseButton)
						iconID = iconGroup->windowCloseButtonPress;

					w->context->currentIcon.left = iconGroup->icons->iconNodes[iconID].lt.x;
					w->context->currentIcon.top = iconGroup->icons->iconNodes[iconID].lt.y;
					w->context->currentIcon.right = iconGroup->icons->iconNodes[iconID].sz.x;
					w->context->currentIcon.bottom = iconGroup->icons->iconNodes[iconID].sz.y;
					w->context->gpu->drawRectangle(mgDrawRectangleReason_windowCloseButton, w,
						&w->closeButtonRect, &wh, &wh, 
						iconGroup->icons->texture, 0);
				}
			}
		}
	}

	if (w->flagsInternal & mgWindowFlag_internal_isExpand)
	{
		mgDrawElement(w->rootElement);
		if (w->flagsInternal & mgWindowFlag_internal_canScroll)
		{
			w->context->gpu->setClipRect(&w->scrollbarBGRect);
			w->context->gpu->drawRectangle(mgDrawRectangleReason_windowScrollbarBG, w,
				&w->scrollbarBGRect,
				&style->windowScrollbarBG,
				&style->windowScrollbarBG,
				0, 0);
			
			w->context->gpu->drawRectangle(mgDrawRectangleReason_windowScrollbarElement, w,
				&w->scrollbarElementRect,
				&style->windowScrollbarElement,
				&style->windowScrollbarElement,
				0, 0);
		}

		if (w->menu)
		{
			w->context->gpu->setClipRect(&w->menuRect);
			w->context->gpu->drawRectangle(mgDrawRectangleReason_windowMenuBG, w,
				&w->menuRect,
				&style->windowMenuBG,
				&style->windowMenuBG,
				0, 0);
			mgPoint pt;
			for (int i = 0; i < w->menu->itemsSize; ++i)
			{
				if (w->menu->activeItem == &w->menu->items[i])
				{
					w->context->gpu->drawRectangle(mgDrawRectangleReason_windowMenuActiveItemBG, w,
						&w->menu->activeItem->rect,
						&style->windowMenuActiveItemBG,
						&style->windowMenuActiveItemBG,
						0, 0);
				}
				else if (w->menu->hoverItem == &w->menu->items[i])
				{
					w->context->gpu->drawRectangle(mgDrawRectangleReason_windowMenuHoverItemBG, w,
						&w->menu->hoverItem->rect,
						&style->windowMenuHoverItemBG,
						&style->windowMenuHoverItemBG,
						0, 0);
				}

				if (!w->menu->items[i].info.text)
					continue;

				pt.x = w->menu->items[i].rect.left + w->menu->textIndent;
				pt.y = w->menu->items[i].rect.top;

				w->context->gpu->drawText(mgDrawTextReason_windowMenu, w,
					&pt,
					w->menu->items[i].info.text,
					w->menu->items[i].textLen,
					&style->windowTitlebarText,
					w->menu->font);
			}
		}
	}
}

void
mgWindowRebuildMenu(struct mgWindow_s* w)
{
	mgRect r = w->rect;
	if (w->dockPanelWindow)
		r = w->dockPanelWindow->windowRect;

	w->menu->currentHeight = w->menu->height;

	w->menuRect.left = r.left;
	w->menuRect.top = r.top;

	if (w->flags & mgWindowFlag_withTitlebar)
	{
		if(!w->dockPanelWindow)
			w->menuRect.top += w->titlebarHeight;
	}

	w->menuRect.right = r.right;

	int X = w->menuRect.left + w->menu->indent;
	int Y = w->menuRect.top;
	for (int i = 0; i < w->menu->itemsSize; ++i)
	{
		mgRect itemRect;
		itemRect.left = 0;
		itemRect.top = 0;
		itemRect.right = w->menu->items[i].width;
		itemRect.bottom = w->menu->height;

		itemRect.top += Y;
		itemRect.bottom += Y;
		itemRect.left += X;
		itemRect.right += X;

		X += w->menu->items[i].width + w->menu->textIndent;
		
		if (X > w->menuRect.right)
		{
			Y += w->menu->height+1;
			X = w->menuRect.left;
			w->menu->currentHeight += w->menu->height;

			itemRect.left = 0;
			itemRect.top = 0;
			itemRect.right = w->menu->items[i].width;
			itemRect.bottom = w->menu->height;

			itemRect.top += Y;
			itemRect.bottom += Y;
			itemRect.left += X;
			itemRect.right += X;
			X += w->menu->items[i].width + w->menu->textIndent;
		}

		itemRect.right += w->menu->textIndent + w->menu->textIndent;
		X += w->menu->textIndent + 1;

		w->menu->items[i].rect = itemRect;
	}

	w->menuRect.bottom = w->menuRect.top + w->menu->currentHeight;
}

void
mgUpdateWindow(struct mgWindow_s* w)
{
	assert(w);

	int topIndent = 0;

	if ((w->flags & mgWindowFlag_withTitlebar) && !w->dockPanelWindow)
	{
		topIndent += w->titlebarHeight;
	}

	if (w->menu)
	{
		mgWindowRebuildMenu(w);
		topIndent += w->menu->currentHeight;
	}

	static float posX = 0;
	static float posXlerp = 0;
	static float posY = 0;
	static float posYlerp = 0;
	static float scrollOnClick = 0.f;

	static mgPoint firstClick;
	static mgPoint saveSize;

	w->cursorInfo = mgWindowCursorInfo_out;
	int windowBtm = w->position.y + w->size.y;

	int needUpdateTransform = 0;
	{
	begin:;
		needUpdateTransform = 1;
	}

	if (w->flags & mgWindowFlag_collapseButton)
	{
		if ((w->flagsInternal & mgWindowFlag_internal_isExpand) == 0)
			windowBtm = w->position.y + topIndent;
	}

	if (w->dockPanelWindow && (w->flags & mgWindowFlag_canDock))
	{
		if (mgPointInRect(&w->dockPanelWindow->windowRect, &w->context->input->mousePosition))
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				firstClick = w->context->input->mousePosition;
		}

		w->rootElement->transformLocal.buildArea = w->dockPanelWindow->windowRect;
		w->rootElement->transformLocal.clipArea = w->dockPanelWindow->windowRect;

		w->rootElement->transformLocal.buildArea.top += topIndent;
		w->rootElement->transformLocal.clipArea.top += topIndent;

		if (w->contentHeight > w->clientHeight)
		{
			w->rootElement->transformLocal.buildArea.right -= w->scrollbarWidth;
			w->rootElement->transformLocal.clipArea.right -= w->scrollbarWidth;
		}

		w->rootElement->transformWorld = w->rootElement->transformLocal;
	}
	else
	{
		if (mgPointInRect(&w->rect, &w->context->input->mousePosition))
		{
			w->context->windowUnderCursor = w;

			if (w->cursorInfoOld == mgWindowCursorInfo_out)
				mgSetCursor_f(w->context, w->context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);

			w->cursorInfo = mgWindowCursorInfo_client;

			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				firstClick = w->context->input->mousePosition;
				saveSize = w->size;
			}

			if ((w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				|| (w->context->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
				|| (w->context->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN))
				mgBringWindowToTop_f(w);

			if (w->flags & mgWindowFlag_withTitlebar)
			{
				if (w->context->input->mousePosition.y < (w->position.y + w->titlebarHeight))
				{
					w->cursorInfo = mgWindowCursorInfo_titlebar;

					if (mgPointInRect(&w->closeButtonRect, &w->context->input->mousePosition))
					{
						w->cursorInfo = mgWindowCursorInfo_closeButton;
						/*w->context->tooltipText = L"Close";*/
					}
					else if (mgPointInRect(&w->collapseButtonRect, &w->context->input->mousePosition))
						w->cursorInfo = mgWindowCursorInfo_collapseButton;
				}
			}

			if ((w->flags & mgWindowFlag_canResize) && (w->flagsInternal & mgWindowFlag_internal_isExpand))
			{
				w->resizeRBRect.right = w->rect.right;
				w->resizeRBRect.bottom = w->rect.bottom;
				w->resizeRBRect.left = w->resizeRBRect.right - 20;
				w->resizeRBRect.top = w->resizeRBRect.bottom - 20;
				if (mgPointInRect(&w->resizeRBRect, &w->context->input->mousePosition))
				{
					w->cursorInfo = mgWindowCursorInfo_resizeRB;
				}
			}
		}

		if (w->cursorInfo == mgWindowCursorInfo_titlebar)
		{
			if (!(w->flagsInternal & mgWindowFlag_internal_isMove))
			{
				if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				{
					w->flagsInternal |= mgWindowFlag_internal_isMove;
					posX = (float)w->position.x;
					posY = (float)w->position.y;
					posXlerp = posX;
					posYlerp = posY;
				}
			}
		}
		else if (w->cursorInfo == mgWindowCursorInfo_closeButton)
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				w->flagsInternal |= mgWindowFlag_internal_isCloseButton;
			}
			else if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
			{
				if (w->flagsInternal & mgWindowFlag_internal_isCloseButton)
				{
					if (w->onClose)
					{
						if (w->onClose(w))
						{
							if(w->flagsInternal & mgWindowFlag_internal_visible)
								w->flagsInternal ^= mgWindowFlag_internal_visible;
						}
					}
					else
					{
						if (w->flagsInternal & mgWindowFlag_internal_visible)
							w->flagsInternal ^= mgWindowFlag_internal_visible;
					}
				}
			}
		}
		else if (w->cursorInfo == mgWindowCursorInfo_collapseButton)
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				if (w->flagsInternal & mgWindowFlag_internal_isExpand)
					w->flagsInternal ^= mgWindowFlag_internal_isExpand;
				else
				{
					w->flagsInternal |= mgWindowFlag_internal_isExpand;
					windowBtm = w->position.y + w->size.y;
				}
				needUpdateTransform = 1;
			}
		}
		else if (w->cursorInfo == mgWindowCursorInfo_resizeRB)
		{
			mgSetCursor_f(w->context, w->context->defaultCursors[mgCursorType_SizeNWSE], mgCursorType_Arrow);
			if (!(w->flagsInternal & mgWindowFlag_internal_isResizeRB))
			{
				if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
				{
					w->flagsInternal |= mgWindowFlag_internal_isResizeRB;
				}
			}
		}
		else
		{
		}

		if (!(w->cursorInfo & mgWindowCursorInfo_resizeRB) && (w->cursorInfoOld & mgWindowCursorInfo_resizeRB))
			mgSetCursor_f(w->context, w->context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);

		if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			if (w->flagsInternal & mgWindowFlag_internal_isCloseButton)
				w->flagsInternal ^= mgWindowFlag_internal_isCloseButton;
		}

		if ((w->flagsInternal & mgWindowFlag_internal_isMove) && (w->flags & mgWindowFlag_canMove))
		{
			if (w->context->input->mouseButtonFlags2 & MG_MBFL_LMBHOLD)
			{

				posXlerp += w->context->input->mouseMoveDelta.x;
				w->position.x = (int)lerp((float)w->position.x, posXlerp, w->context->deltaTime * 30.f);

				posYlerp += w->context->input->mouseMoveDelta.y;
				w->position.y = (int)lerp((float)w->position.y, posYlerp, w->context->deltaTime * 30.f);

				//printf("%f %f %i\n", posXlerp, posX, w->position.x);
				//w->position.x = w->position.x + w->context->input->mouseMoveDelta.x;
				//w->position.y = w->position.y + w->context->input->mouseMoveDelta.y;
			}

			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
			{
				w->flagsInternal ^= mgWindowFlag_internal_isMove;
				needUpdateTransform = 1;
				if (g_windowToDockPanelMode)
				{
					g_windowToDockPanelMode = 0;
					mgDockAddWindow_f(w, g_dockPanelWindow, g_dockElIdOrWhere);
					needUpdateTransform = 2;
				}
			}
		}
		else if (w->flags & mgWindowFlag_canResize)
		{
			if (w->flagsInternal & mgWindowFlag_internal_isResizeRB)
			{
				//if (w->flagsInternal & mgWindowFlag_internal_isExpand)
				{
					int px = w->context->input->mousePosition.x - firstClick.x;
					int py = w->context->input->mousePosition.y - firstClick.y;

					w->size.x = px + saveSize.x;
					w->size.y = py + saveSize.y;

					if (w->size.x < w->sizeMinimum.x)
						w->size.x = w->sizeMinimum.x;
					if (w->size.y < w->sizeMinimum.y + topIndent)
						w->size.y = w->sizeMinimum.y + topIndent;

					if (w->size.x < 0)
						w->size.x = 0;
					if (w->size.y < 0)
						w->size.y = 0;
					needUpdateTransform = 1;
					
					w->flagsInternal |= mgWindowFlag_internal_updateContentHeight;

					if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
					{
						w->flagsInternal ^= mgWindowFlag_internal_isResizeRB;
						goto begin;/*fix some sht*/
					}
				}
			}
		}

		if (w->position.x + w->size.x < 30)
			w->position.x += 30 - (w->position.x + w->size.x);

		if (w->position.x > w->context->windowSize.x - 30)
			w->position.x = w->context->windowSize.x - 30;

		if (w->position.y < 0)
			w->position.y = 0;

		if (w->position.y > w->context->windowSize.y - topIndent)
			w->position.y = w->context->windowSize.y - topIndent;

		w->rect.left = w->position.x;
		w->rect.top = w->position.y;
		w->rect.right = w->rect.left + w->size.x;
		w->rect.bottom = windowBtm;
		w->rootElement->transformLocal.buildArea = w->rect;
		w->rootElement->transformLocal.clipArea = w->rect;
		w->rootElement->transformLocal.clipArea.top += topIndent;
		
		if (w->contentHeight > w->clientHeight)
		{
			w->rootElement->transformLocal.buildArea.right -= w->scrollbarWidth;
			w->rootElement->transformLocal.clipArea.right -= w->scrollbarWidth;
		}

		w->rootElement->transformWorld = w->rootElement->transformLocal;

		if (w->context->input->keyboardModifier == MG_KBMOD_CTRL)
		{
			g_windowToDockPanelMode = 0;
			g_dockPanelWindow = 0;
			g_dockElIdOrWhere = 0;
		}

		if (w->flagsInternal & mgWindowFlag_internal_isMove)
		{
			needUpdateTransform = 1;

			if ((w->flags & mgWindowFlag_canDock) 
				&& !w->dockPanelWindow 
				&& w->context->dockPanel
				&& w->context->input->keyboardModifier != MG_KBMOD_CTRL)
			{
				g_windowToDockPanelMode = 0;
				g_dockPanelWindow = 0;
				g_dockElIdOrWhere = 0;

				/*first, check if cursor inside mgDockPanelWindow*/
				/*only after check if cursor in elements[i].addWindowRect*/
				for (int i = 1; i < w->context->dockPanel->elementsSize; ++i)
				{
					if (mgPointInRect(&w->context->dockPanel->elements[i].rect, &w->context->input->mousePosition))
					{
						if (w->context->dockPanel->elements[i].panelWindows)
						{
							for (int o = 0; o < w->context->dockPanel->elements[i].panelWindowsSize; ++o)
							{
								mgDockPanelWindow* pnl = w->context->dockPanel->elements[i].panelWindows[o];
								if (mgPointInRect(&pnl->rect, &w->context->input->mousePosition))
								{
									g_dockPanelWindow = pnl;
									i = w->context->dockPanel->elementsSize;
									break;
								}
							}
						}
						break;
					}
				}

				if (g_dockPanelWindow)
				{
					/*now find where*/
					/*g_dockPanelWindow->rect*/

					mgRect rLeft = g_dockPanelWindow->rect;
					mgRect rTop = g_dockPanelWindow->rect; 
					mgRect rRight = g_dockPanelWindow->rect;
					mgRect rBottom = g_dockPanelWindow->rect;
					mgRect rCenter;

					int wx = g_dockPanelWindow->rect.right - g_dockPanelWindow->rect.left;
					int wy = g_dockPanelWindow->rect.bottom - g_dockPanelWindow->rect.top;

					if (wx < 25 || wy < 25)
					{
						g_windowToDockRect = g_dockPanelWindow->rect;
						g_windowToDockPanelMode = 1;
						g_dockElIdOrWhere = 4;
					}
					else
					{
						int szY = wy / 4;
						int szX = wx / 4;

						rTop.bottom = rTop.top + szY;
						rBottom.top = rBottom.bottom - szY;
						rLeft.right = rLeft.left + szX;
						rRight.left = rRight.right - szX;

						rCenter.left = rLeft.right;
						rCenter.top = rTop.bottom;
						rCenter.right = rRight.left;
						rCenter.bottom = rBottom.top;
						if (mgPointInRect(&rCenter, &w->context->input->mousePosition))
						{
							g_windowToDockPanelMode = 1;
							g_windowToDockRect = g_dockPanelWindow->rect;
							g_dockElIdOrWhere = 4;
						}
						else if (mgPointInRect(&rTop, &w->context->input->mousePosition))
						{
							g_windowToDockPanelMode = 1;
							g_windowToDockRect = rTop;
							g_dockElIdOrWhere = 1;
						}
						else if (mgPointInRect(&rBottom, &w->context->input->mousePosition))
						{
							g_windowToDockPanelMode = 1;
							g_windowToDockRect = rBottom;
							g_dockElIdOrWhere = 3;
						}
						else if (mgPointInRect(&rLeft, &w->context->input->mousePosition))
						{
							g_windowToDockPanelMode = 1;
							g_windowToDockRect = rLeft;
							g_dockElIdOrWhere = 0;
						}
						else if (mgPointInRect(&rRight, &w->context->input->mousePosition))
						{
							g_windowToDockPanelMode = 1;
							g_windowToDockRect = rRight;
							g_dockElIdOrWhere = 2;
						}
					}
				}
				else
				{
					for (int i = 1; i < w->context->dockPanel->elementsSize; ++i)
					{
						if (w->context->dockPanel->elements[i].panelWindows)
							continue;

						if (mgPointInRect(&w->context->dockPanel->elements[i].addWindowRect, &w->context->input->mousePosition))
						{
							g_dockElIdOrWhere = i - 1;
							g_windowToDockPanelMode = 1;
							g_windowToDockRect = w->context->dockPanel->elements[i].addWindowRect;
							int rszX = g_windowToDockRect.right - g_windowToDockRect.left;
							int rszY = g_windowToDockRect.bottom - g_windowToDockRect.top;

							switch (w->context->dockPanel->elements[i].info.where)
							{
							case 0:
								if (rszX < w->size.x)
									g_windowToDockRect.right += w->size.x - rszX;
								break;
							case 1:
								if (rszY < w->size.y)
									g_windowToDockRect.bottom += w->size.y - rszY;
								break;
							case 2:
								if (rszX < w->size.x)
									g_windowToDockRect.left -= w->size.x - rszX;
								break;
							case 3:
								if (rszY < w->size.y)
									g_windowToDockRect.top -= w->size.y - rszY;
								break;
							}
							break;
						}
					}
				}

			}
		}
	
		if ((w->flags & mgWindowFlag_withTitlebar))
		{
			w->rootElement->transformWorld.buildArea.top += topIndent;
			w->rootElement->transformWorld.buildArea.bottom += topIndent;
		}
	}

	w->rootElement->scrollValueWorld = w->rootElement->scrollValue;

	if (needUpdateTransform)
	{
		mgUpdateTransformElement(w->rootElement);
		if (needUpdateTransform == 2)
		{
			mgDockPanelUpdateWindow(w->context);
		}
	}
	//printf("%i %i\n", w->position.x, w->context->windowSize.x);

	if (w->flagsInternal & mgWindowFlag_internal_updateContentHeight)
	{
		w->flagsInternal ^= mgWindowFlag_internal_updateContentHeight;
		
		if (w->dockPanelWindow)
		{
			w->clientHeight = w->dockPanelWindow->rect.bottom - w->dockPanelWindow->tabRect.bottom;
		}
		else
		{
			w->clientHeight = w->rect.bottom - w->rect.top;

			if (w->flags & mgWindowFlag_withTitlebar)
				w->clientHeight -= topIndent;
		}

		w->contentHeight = 0;
		mgWindowUpdateContentHeight(w, w->rootElement);

		if (w->contentHeight > w->clientHeight)
		{
			w->flagsInternal |= mgWindowFlag_internal_canScroll;
		}
		else
		{
			if(w->flagsInternal & mgWindowFlag_internal_canScroll)
				w->flagsInternal ^= mgWindowFlag_internal_canScroll;
		}
	}
	

	if (w->contentHeight > w->clientHeight)
	{
		static int locklmb = 0;
		float m1 = 1.f / (float)w->contentHeight;
		float m2 = (float)w->clientHeight * m1;

		if (mgPointInRect(&w->scrollbarElementRect, &w->context->input->mousePosition))
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				scrollOnClick = w->rootElement->scrollValue;
				locklmb = 1;
			}

			if ((w->flagsInternal & mgWindowFlag_internal_isResizeRB) == 0)
			{
				if ((w->context->input->mouseButtonFlags2 & MG_MBFL_LMBHOLD)
					&& locklmb)
					w->flagsInternal |= mgWindowFlag_internal_scrollMode;
			}
		}

		if (w->flagsInternal & mgWindowFlag_internal_scrollMode)
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
			{
				w->flagsInternal ^= mgWindowFlag_internal_scrollMode;
				locklmb = 0;
			}

			int y = w->context->input->mousePosition.y - firstClick.y;
			if (w->context->input->mousePosition.y <= w->scrollbarBGRect.top)
			{
				w->rootElement->scrollValueTarget = 0.f;
			}
			else if (w->context->input->mousePosition.y >= w->scrollbarBGRect.bottom)
			{
				int v = w->contentHeight - w->clientHeight;
				w->rootElement->scrollValueTarget = (float)v;
			}
			else
			{
				w->rootElement->scrollValueTarget = scrollOnClick + ((float)y / m2);
				if (w->rootElement->scrollValueTarget < 0.f)
					w->rootElement->scrollValueTarget = 0.f;
			}
		}

		mgRect r;
		if (w->dockPanelWindow)
			r = w->dockPanelWindow->rect;
		else
			r = w->rect;

		if (mgPointInRect(&r, &w->context->input->mousePosition))
		{
			if (w->context->input->mouseWheelDelta < 0.f)
			{
				w->rootElement->scrollValueTarget += 10.f;
			}
			else if (w->context->input->mouseWheelDelta > 0.f)
			{
				w->rootElement->scrollValueTarget -= 10.f;

				if (w->rootElement->scrollValueTarget < 0.f)
					w->rootElement->scrollValueTarget = 0.f;
			}
		}

		if (w->dockPanelWindow)
		{
			w->scrollbarBGRect.right = w->dockPanelWindow->rect.right;
			w->scrollbarBGRect.left = w->scrollbarBGRect.right - w->scrollbarWidth;
			w->scrollbarBGRect.top = w->dockPanelWindow->tabRect.bottom + topIndent;
			w->scrollbarBGRect.bottom = w->dockPanelWindow->rect.bottom;
		}
		else
		{
			w->scrollbarBGRect.right = w->rect.right;
			w->scrollbarBGRect.left = w->scrollbarBGRect.right - w->scrollbarWidth;
			
			w->scrollbarBGRect.top = w->rect.top + topIndent;

			w->scrollbarBGRect.bottom = w->rect.bottom;
		}

		w->scrollbarElementRect = w->scrollbarBGRect;
		float screlsz = ((float)(w->scrollbarElementRect.bottom - w->scrollbarElementRect.top) * m2);
		if (screlsz < 20.f)
			screlsz = 20.f;
		w->scrollbarElementRect.bottom = w->scrollbarElementRect.top + (int)screlsz;
		{
			int v = w->contentHeight - w->clientHeight;
			if (w->rootElement->scrollValue > (float)v)
				w->rootElement->scrollValue = (float)v;
			if (w->rootElement->scrollValueTarget > (float)v)
				w->rootElement->scrollValueTarget = (float)v;

			float v2 = w->rootElement->scrollValue * m1;
			v2 = v2 / (1.f / (float)w->clientHeight);
			w->scrollbarElementRect.top += (int)v2;
			w->scrollbarElementRect.bottom += (int)v2;
		}

		w->rootElement->scrollValue = lerp(w->rootElement->scrollValue, w->rootElement->scrollValueTarget, 0.1f);
	}


	if (w->menu && !w->context->popupUnderCursor 
		&& !g_dockpanel_splitterModeElement
		&& !g_dockpanel_splitterModePanel)
	{
		mgWindowRebuildMenu(w);
		w->menu->hoverItem = 0;

		if (w->menu->activeItem)
		{
			if (w->menu->activeItem && mgPointInRect(&w->menuRect, &w->context->input->mousePosition))
			{
				for (int i = 0; i < w->menu->itemsSize; ++i)
				{
					if (mgPointInRect(&w->menu->items[i].rect, &w->context->input->mousePosition))
					{
						if (&w->menu->items[i] != w->menu->activeItem)
						{
							mgShowPopup_f(w->context, 0, 0);
							w->context->activeMenu = w->menu;
							w->menu->activeItem = &w->menu->items[i];
							if (w->menu->activeItem->info.popup)
							{
								mgPoint pt;
								pt.x = w->menu->activeItem->rect.left;
								pt.y = w->menu->activeItem->rect.bottom - 5;
								mgShowPopup_f(w->context, w->menu->activeItem->info.popup, &pt);
							}
							break;
						}
					}
				}
			}
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				w->menu->activeItem = 0;
				w->context->activeMenu = 0;
				mgShowPopup_f(w->context, 0, 0);
			}
		}
		else
		{
			if (mgPointInRect(&w->menuRect, &w->context->input->mousePosition))
			{
				for (int i = 0; i < w->menu->itemsSize; ++i)
				{
					if (mgPointInRect(&w->menu->items[i].rect, &w->context->input->mousePosition))
					{
						w->menu->hoverItem = &w->menu->items[i];
						if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
						{
							w->menu->activeItem = &w->menu->items[i];
							w->context->activeMenu = w->menu;
							if (w->menu->activeItem->info.popup)
							{

								mgPoint pt;
								pt.x = w->menu->activeItem->rect.left;
								pt.y = w->menu->activeItem->rect.bottom - 5;
								mgShowPopup_f(w->context, w->menu->activeItem->info.popup, &pt);
							}
						}
						break;
					}
				}
			}
		}
	}
	

	w->cursorInfoOld = w->cursorInfo;
}

MG_API
void
MG_C_DECL
mgSetWindowTitle_f(struct mgWindow_s* w, const wchar_t* t)
{
	assert(w);
	int newLen = 0;
	if (t)
	{
		newLen = wcslen(t);
		w->titlebarText = t;
	}
	w->titlebarTextLen = newLen;
}

MG_API
void
MG_C_DECL
mgShowWindow_f(struct mgWindow_s* w, int i)
{
	assert(w);
	if (i)
	{
		w->flagsInternal |= mgWindowFlag_internal_visible;
	}
	else
	{
		if (w->flagsInternal & mgWindowFlag_internal_visible)
			w->flagsInternal ^= mgWindowFlag_internal_visible;
	}
}