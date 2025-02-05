﻿/*
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
#ifndef _MG_ELEMENTTEXTIN_H_
#define _MG_ELEMENTTEXTIN_H_

#include "mgElementText.h"

typedef struct mgElementTextInput_s {
	struct mgElement_s* element;

	struct mgTextProcessor_s * textProcessor;

	mgUnicodeChar* text;
	size_t textLen;
	size_t allocated; /*in wchar_t*/
	
	const mgUnicodeChar* defaultText; /*default text like 'click to edit'*/
	size_t defaultTextLen;         /*set this too*/

	/*
	* this points to place where to put new char.
	* from 0 to textLen
	* 0 - put in front
	* textLen - push_back
	*/
	size_t textCursor;
	mgPoint textCursorPosition; /*will set when draw text*/
	float textCursorTimer;
	float textCursorTimerLimit; /*0.5f*/
	size_t drawTextCursor;/*bool, internal*/
	mgRect textCursorRect;
	
	size_t textBeginDrawIndex;

	uint32_t charLimit;/*0*/
	uint32_t monospace;/*0*/
	uint32_t canSelect;/*1*/
	uint32_t multiline;/*0*/

	/*
	* type: 0 unknown, 1 enter key, 2 click somewhere, 3 escape key
	* return 1 if need to deactivate input
	*/
	int(*onEndEdit)(struct mgElement_s*, int type);
	mgUnicodeChar(*onCharEnter)(struct mgElement_s*, mgUnicodeChar);
	void(*onActivate)(struct mgElement_s*);
	int(*onPaste)(struct mgElement_s*, mgUnicodeChar*, size_t);

	uint32_t isSelected;
	uint32_t selectionStart;
	uint32_t selectionEnd;

	double h_scroll;
	double h_scrollCurr;
	double h_scrollLimit;

	uint32_t textWidth;/*in pixels*/

	struct mgPopup_s* rightClickPopup;

	uint32_t charIndexUnderCursor;

} mgElementTextInput;



#endif