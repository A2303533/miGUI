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
