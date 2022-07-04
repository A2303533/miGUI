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

mgMenu* MG_C_DECL
mgCreateMenu(struct mgContext_s* c, mgMenuItemInfo* items, int itemsSize, struct mgTextProcessor_s* tp)
{
	assert(tp);
	assert(items);
	assert(itemsSize > 0);

	mgMenu* newMenu = calloc(1, sizeof(mgMenu));
	newMenu->items = calloc(1, sizeof(mgMenuItem) * itemsSize);
	newMenu->itemsSize = itemsSize;
	newMenu->height = 20;
	newMenu->indent = 0;
	//newMenu->font = f;
	newMenu->textProcessor = tp;
	newMenu->textIndent = 5;
	for (int i = 0; i < itemsSize; ++i)
	{
		newMenu->items[i].info = items[i];
		newMenu->items[i].isEnabled = 1;

		if (newMenu->items[i].info.text)
		{
			newMenu->items[i].textLen = mgUnicodeStrlen(newMenu->items[i].info.text);

			mgPoint tsz;
			tp->onGetTextSize(
				mgDrawTextReason_menu,
				tp,
				newMenu->items[i].info.text,
				newMenu->items[i].textLen,
				&tsz);
			//c->getTextSize(newMenu->items[i].info.text, f, &tsz);

			newMenu->items[i].width = tsz.x;
		}
	}

	return newMenu;
}

void MG_C_DECL
mgDestroyMenu(mgMenu* m)
{
	assert(m);
	if (m->items)
		free(m->items);
	free(m);
}