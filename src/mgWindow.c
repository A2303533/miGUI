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
	mgWindow* newWindow = calloc(1, sizeof(mgWindow));
	newWindow->position.x = px;
	newWindow->position.y = py;
	newWindow->size.x = sx;
	newWindow->size.y = sy;
	newWindow->context = ctx;
	newWindow->flags = mgWindowFlag_withTitlebar;
	newWindow->titlebarHeight = 15;
	
	newWindow->left = newWindow;
	newWindow->right = newWindow;

	newWindow->rootElement = calloc(1, sizeof(mgElement));
	newWindow->rootElement->window = newWindow;
	newWindow->rootElement->visible = 1;
	newWindow->rootElement->onDraw = mgrootobject_cb;
	newWindow->rootElement->onUpdate = mgrootobject_cb;
	newWindow->rootElement->onUpdateTransform = mgrootobject_cb;
	newWindow->rootElement->onRebuild = mgrootobject_cb;

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

void
mgDrawWindow(struct mgWindow_s* w)
{
	assert(w);

	mgStyle* style = w->userStyle ? w->userStyle : w->context->activeStyle;

	w->context->gpu->drawRectangle(mgDrawRectangleReason_windowBG, &w->position, &w->size, &style->windowBGColor, &style->windowBGColor, 0, 0, 0);

	if (w->flags & mgWindowFlag_withTitlebar)
	{
		mgPoint sz;
		sz.x = w->size.x;
		sz.y = w->titlebarHeight;
		w->context->gpu->drawRectangle(mgDrawRectangleReason_windowTitlebar, &w->position, &sz, &style->windowTitlebarColor, &style->windowTitlebarColor, 0, 0, 0);
		
		if (w->titlebarFont && w->titlebarText)
		{
			w->context->gpu->drawText(mgDrawTextReason_windowTitlebar,
				&w->position,
				w->titlebarText,
				w->titlebarTextLen,
				&style->windowTitlebarTextColor,
				w->titlebarFont);
		}
	}

	mgDrawElement(w->rootElement);
}

void
mgUpdateWindow(struct mgWindow_s* w)
{
	assert(w);

	static int isMove = 0;

	static float posX = 0;
	static float posXlerp = 0;
	static float posY = 0;
	static float posYlerp = 0;

	w->cursorInfo = mgWindowCursorInfo_out;
	if ((w->context->input->mousePosition.x > w->position.x) && (w->context->input->mousePosition.x < (w->position.x + w->size.x)))
	{
		if ((w->context->input->mousePosition.y > w->position.y) && (w->context->input->mousePosition.y < (w->position.y + w->size.y)))
		{
			w->cursorInfo = mgWindowCursorInfo_client;

			if (w->context->input->mousePosition.y < (w->position.y + w->titlebarHeight))
			{
				w->cursorInfo = mgWindowCursorInfo_titlebar;
			}
		}
	}

	if (w->cursorInfo == mgWindowCursorInfo_titlebar)
	{
		if (!isMove)
		{
			if (w->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				isMove = 1;
				posX = w->position.x;
				posY = w->position.y;
				posXlerp = posX;
				posYlerp = posY;
			}
		}
	}

	mgPoint oldPosition = w->position;

	if (isMove)
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
			isMove = 0;
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

	//printf("%i %i\n", w->position.x, w->context->windowSize.x);
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
