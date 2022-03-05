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

#pragma once
#ifndef _MG_POPUP_H_
#define _MG_POPUP_H_

struct mgPopup_s;

enum mgPopupItemType
{
	mgPopupItemType_default = 0,
	mgPopupItemType_separator,
	mgPopupItemType_check,
	mgPopupItemType_radio, /*use separator for groups*/
};

typedef struct mgPopupItemInfo_s {
	int id;
	const wchar_t* text;
	struct mgPopup_s* subMenu; /*if subMenu then skip callback*/
	void(*callback)(int id);
	int type;
	int isChecked;
	const wchar_t* shortcutText;
} mgPopupItemInfo;

struct mgPopupItem_s {
	struct mgPopupItemInfo_s info;
	int textLen;
	int shortcutTextLen;
	int indentForShortcutText;
	int shortcutTextWidth;
};

typedef struct mgPopup_s {
	mgRect rect;

	mgPoint indent;
	int itemHeight;
	mgFont* font;

	struct mgPopupItem_s* items;
	int itemsSize;

	struct mgPopupItem_s* nodeUnderCursor;
	mgRect nodeUnderCursorRect;

	int maxTextWidth;/*internal, in pixels*/
	int maxShortcutTextWidth;/*internal, in pixels*/
	int textShortcutTextIndent;/*space between text and shortcutText*/

	int iconLeftWidth;
	int iconRightWidth;

	int subVisible;

	struct mgPopup_s* parent;/*internal*/
} mgPopup;

#endif