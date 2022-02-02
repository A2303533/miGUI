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

void
mgDockPanelOnSize(struct mgContext_s* c)
{
	assert(c);
	assert(c->dockPanel);
	c->dockPanel->rect.left = c->dockPanel->indent.left;
	c->dockPanel->rect.top = c->dockPanel->indent.top;
	c->dockPanel->rect.right = c->windowSize.x - c->dockPanel->indent.left;
	c->dockPanel->rect.bottom = c->windowSize.y - c->dockPanel->indent.bottom;
}

void
mgDrawDockPanel(struct mgContext_s* c)
{
	mgPoint pos;
	pos.x = c->dockPanel->rect.left;
	pos.y = c->dockPanel->rect.top;

	mgPoint sz;
	sz.x = c->dockPanel->rect.right - c->dockPanel->rect.left;
	sz.y = c->dockPanel->rect.bottom - c->dockPanel->rect.top;

	if (c->dockPanel->flags & mgDockPanelFlag_drawBG)
	{
		c->gpu->setClipRect(&c->dockPanel->rect);
		c->gpu->drawRectangle(mgDrawRectangleReason_dockBG, &pos, &sz, &c->activeStyle->dockpanelBGColor, &c->activeStyle->dockpanelBGColor, 0, 0, 0);
	}
}

MG_API
void MG_C_DECL
mgInitDockPanel_f(struct mgContext_s* c, int indentLeft, int indentTop, int indentRight, int indentBottom)
{
	assert(c);
	assert(indentLeft >= 0);
	assert(indentTop >= 0);
	assert(indentRight >= 0);
	assert(indentBottom >= 0);
	
	if (c->dockPanel)
		return;

	c->dockPanel = malloc(sizeof(struct mgDockPanel_s));
	c->dockPanel->indent.left = indentLeft;
	c->dockPanel->indent.top = indentTop;
	c->dockPanel->indent.right = indentRight;
	c->dockPanel->indent.bottom = indentBottom;

	mgDockPanelOnSize(c, c->windowSize.x, c->windowSize.y);
}
