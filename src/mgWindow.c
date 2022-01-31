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

void mgDestroyElement_f(mgElement* e);
void mgrootobject_cb(mgElement*e) {}
void mgDrawElement(mgElement* e);
void mgUpdateTransformElement(mgElement* e);

float 
lerp(float v0, float v1, float t) 
{
	return (1.f - t) * v0 + t * v1;
}

MG_API
struct mgWindow_s*
MG_C_DECL
mgCreateWindow_f(struct mgContext_s* ctx, int px, int py, int sx, int sy)
{
	assert(ctx);

	/*static int uniqueID = 1;*/

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

	newWindow->titlebarHeight = 15;
	newWindow->flags 
		|= mgWindowFlag_internal_visible
		| mgWindowFlag_internal_isExpand;
	/*newWindow->uniqueID = uniqueID++;*/
	
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

	if (w->titlebarText)
		free(w->titlebarText);

	free(w);
}

MG_API
void
MG_C_DECL
mgBringWindowToTop_f(struct mgWindow_s* w)
{
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

	mgStyle* style = w->userStyle ? w->userStyle : w->context->activeStyle;

	mgColor clrbg = w->context->firstWindow->left == w ? style->windowBGColorTopWindow : style->windowBGColor;
	mgColor clrttl = w->context->firstWindow->left == w ? style->windowTitlebarColorTopWindow : style->windowTitlebarColor;

	if ((w->flags & mgWindowFlag_drawBG) && (w->flags & mgWindowFlag_internal_isExpand))
		w->context->gpu->drawRectangle(mgDrawRectangleReason_windowBG, &w->position, &w->size, &clrbg, &clrbg, 0, 0, 0);

	if (w->flags & mgWindowFlag_withTitlebar)
	{
		float tbhalfsz = (float)w->titlebarHeight * 0.5f;

		mgPoint sz;

		sz.x = w->size.x;
		sz.y = w->titlebarHeight;
		w->context->gpu->drawRectangle(mgDrawRectangleReason_windowTitlebar, &w->position, &sz, &clrttl, &clrttl, 0, 0, 0);
		
		int text_move = 0;
		if (w->flags & mgWindowFlag_collapseButton)
		{
			if (w->icons)
			{
				mgPoint pos;
				pos.x = w->position.x + 3;
				pos.y = w->position.y;
				int iconID = w->iconExpandButton;

				if (w->flags & mgWindowFlag_internal_isExpand)
					iconID = w->iconCollapseButton;

				sz = w->icons->icons[iconID].sz;


				text_move = sz.x + 3;

				mgColor wh;
				mgColorSet(&wh, 1.f, 1.f, 1.f, 1.f);

				{
					float ichalfsz = (float)sz.y * 0.5f;
					pos.y += (int)(tbhalfsz - ichalfsz);
				}

				w->collapseButtonRect.left = pos.x;
				w->collapseButtonRect.top = pos.y;
				w->collapseButtonRect.right = w->collapseButtonRect.left + sz.x;
				w->collapseButtonRect.bottom = w->collapseButtonRect.top + sz.y;

				w->context->currentIcon.left = w->icons->icons[iconID].lt.x;
				w->context->currentIcon.top = w->icons->icons[iconID].lt.y;
				w->context->currentIcon.right = w->icons->icons[iconID].sz.x;
				w->context->currentIcon.bottom = w->icons->icons[iconID].sz.y;
				w->context->gpu->drawRectangle(mgDrawRectangleReason_windowCollapseButton, &pos, &sz, &wh, &wh, 0, w->icons->texture, 0);
			}
		}

		if (w->titlebarFont && w->titlebarText)
		{
			mgPoint pos;
			pos.x = w->position.x + text_move + 3;
			pos.y = w->position.y;

			w->context->gpu->drawText(mgDrawTextReason_windowTitlebar,
				&pos,
				w->titlebarText,
				w->titlebarTextLen,
				&style->windowTitlebarTextColor,
				w->titlebarFont);
		}

		if (w->flags & mgWindowFlag_closeButton)
		{
			if (w->icons)
			{
				mgColor wh;
				mgColorSet(&wh, 1.f, 1.f, 1.f, 1.f);
				sz = w->icons->icons[w->iconCloseButton].sz;
				mgPoint pos;
				pos.x = w->position.x + w->size.x - sz.x-3;
				pos.y = w->position.y;

				{
					float ichalfsz = (float)sz.y * 0.5f;
					pos.y += (int)(tbhalfsz - ichalfsz);
				}

				int iconID = w->iconCloseButton;

				w->closeButtonRect.left = pos.x;
				w->closeButtonRect.top = pos.y;
				w->closeButtonRect.right = w->closeButtonRect.left + sz.x;
				w->closeButtonRect.bottom = w->closeButtonRect.top + sz.y;

				if (w->cursorInfo == mgWindowCursorInfo_closeButton)
				{
					iconID = w->iconCloseButtonMouseHover;

				}
				if (w->flags & mgWindowFlag_internal_isCloseButton)
					iconID = w->iconCloseButtonPress;

				w->context->currentIcon.left = w->icons->icons[iconID].lt.x;
				w->context->currentIcon.top = w->icons->icons[iconID].lt.y;
				w->context->currentIcon.right = w->icons->icons[iconID].sz.x;
				w->context->currentIcon.bottom = w->icons->icons[iconID].sz.y;
				w->context->gpu->drawRectangle(mgDrawRectangleReason_windowCloseButton, &pos, &sz, &wh, &wh, 0, w->icons->texture, 0);
			}
		}
	}

	if (w->flags & mgWindowFlag_internal_isExpand)
		mgDrawElement(w->rootElement);
}

void
mgUpdateWindow(struct mgWindow_s* w)
{
	assert(w);

	static float posX = 0;
	static float posXlerp = 0;
	static float posY = 0;
	static float posYlerp = 0;

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
		if ((w->flags & mgWindowFlag_internal_isExpand) == 0)
			windowBtm = w->position.y + w->titlebarHeight;
	}

	/*w->rect.left = w->position.x;
	w->rect.top = w->position.y;
	w->rect.right = w->rect.left + w->size.x;
	w->rect.bottom = windowBtm;*/

	if (mgPointInRect(&w->rect, &w->context->input->mousePosition))
	{
		w->context->windowUnderCursor = w;
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
					w->cursorInfo = mgWindowCursorInfo_closeButton;
				else if (mgPointInRect(&w->collapseButtonRect, &w->context->input->mousePosition))
					w->cursorInfo = mgWindowCursorInfo_collapseButton;
			}
		}

		if (w->flags & mgWindowFlag_canResize)
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
		if (!(w->flags & mgWindowFlag_internal_isMove))
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				w->flags |= mgWindowFlag_internal_isMove;
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
			w->flags |= mgWindowFlag_internal_isCloseButton;
		}
		else if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			if (w->flags & mgWindowFlag_internal_isCloseButton)
			{
				if (w->onClose)
				{
					if(w->onClose(w))
						w->flags ^= mgWindowFlag_internal_visible;
				}
				else
				{
					w->flags ^= mgWindowFlag_internal_visible;
				}
			}
		}
	}
	else if (w->cursorInfo == mgWindowCursorInfo_collapseButton)
	{
		if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
		{
			if (w->flags & mgWindowFlag_internal_isExpand)
				w->flags ^= mgWindowFlag_internal_isExpand;
			else
			{
				w->flags |= mgWindowFlag_internal_isExpand;
				windowBtm = w->position.y + w->size.y;
			}
			needUpdateTransform = 1;
		}
	}
	else if (w->cursorInfo == mgWindowCursorInfo_resizeRB)
	{
		mgSetCursor_f(w->context, w->context->defaultCursors[mgCursorType_SizeNWSE], mgCursorType_Arrow);
		if (!(w->flags & mgWindowFlag_internal_isResizeRB))
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				w->flags |= mgWindowFlag_internal_isResizeRB;
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
		if(w->flags & mgWindowFlag_internal_isCloseButton)
			w->flags ^= mgWindowFlag_internal_isCloseButton;
	}

	if ((w->flags & mgWindowFlag_internal_isMove) && (w->flags & mgWindowFlag_canMove))
	{
		if (w->context->input->mouseButtonFlags2 & MG_MBFL_LMBHOLD)
		{

			posXlerp += w->context->input->mouseMoveDelta.x;
			posX = lerp((float)w->position.x, posXlerp, w->context->deltaTime * 30.f);
			w->position.x = (int)posX;

			posYlerp += w->context->input->mouseMoveDelta.y;
			posY = lerp((float)w->position.y, posYlerp, w->context->deltaTime * 30.f);
			w->position.y = (int)posY;

			//printf("%f %f %i\n", posXlerp, posX, w->position.x);
			//w->position.x = w->position.x + w->context->input->mouseMoveDelta.x;
			//w->position.y = w->position.y + w->context->input->mouseMoveDelta.y;
		}

		if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			w->flags ^= mgWindowFlag_internal_isMove;
			needUpdateTransform = 1;
		}
	}
	else if (w->flags & mgWindowFlag_canResize)
	{
		if (w->flags & mgWindowFlag_internal_isResizeRB)
		{
			//if (w->flags & mgWindowFlag_internal_isExpand)
			{
				int px = w->context->input->mousePosition.x - firstClick.x;
				int py = w->context->input->mousePosition.y - firstClick.y;

				w->size.x = px + saveSize.x;
				w->size.y = py + saveSize.y;

				if (w->size.x < w->sizeMinimum.x)
					w->size.x = w->sizeMinimum.x;
				if (w->size.y < w->sizeMinimum.y)
					w->size.y = w->sizeMinimum.y;

				if (w->size.x < 0)
					w->size.x = 0;
				if (w->size.y < 0)
					w->size.y = 0;
				needUpdateTransform = 1;
				if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
				{
					w->flags ^= mgWindowFlag_internal_isResizeRB;
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

	if (w->position.y > w->context->windowSize.y - w->titlebarHeight)
		w->position.y = w->context->windowSize.y - w->titlebarHeight;

	w->rect.left = w->position.x;
	w->rect.top = w->position.y;
	w->rect.right = w->rect.left + w->size.x;
	w->rect.bottom = windowBtm;
	w->rootElement->transformLocal.buildArea = w->rect;
	w->rootElement->transformLocal.clipArea = w->rect;
	w->rootElement->transformLocal.clipArea.top += w->titlebarHeight;
	w->rootElement->transformWorld = w->rootElement->transformLocal;

	if (w->flags & mgWindowFlag_internal_isMove)
		needUpdateTransform = 1;

	if(needUpdateTransform)
		mgUpdateTransformElement(w->rootElement);
	//printf("%i %i\n", w->position.x, w->context->windowSize.x);

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

		if (w->titlebarText)
		{
			int oldLen = wcslen(w->titlebarText);
			if (newLen > oldLen)
			{
				free(w->titlebarText);
				w->titlebarText = malloc((newLen * sizeof(wchar_t)) + sizeof(wchar_t));
			}
		}
		else
		{
			w->titlebarText = malloc((newLen*sizeof(wchar_t)) + sizeof(wchar_t));
		}
		memcpy(w->titlebarText, t, (newLen * sizeof(wchar_t)));
		w->titlebarText[newLen] = 0;
	}
	else if(w->titlebarText)
	{
		free(w->titlebarText);
		w->titlebarText = 0;
	}
	w->titlebarTextLen = newLen;
}

