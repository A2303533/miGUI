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
#ifndef _MG_ELEMENTLIST_H_
#define _MG_ELEMENTLIST_H_

typedef struct mgElementList_s {
	uint32_t editText;/*0*/
	uint32_t drawItemBG;/*0*/
	uint32_t useF2ForEdit;/*1*/

	void** items;
	uint32_t itemsSize;

	uint32_t firstItemIndexForDraw;/*internal*/
	uint32_t numOfLines;/*internal*/
	int32_t itemHeight;
	//struct mgFont_s* font;
	struct mgTextProcessor_s* textProcessor;

	float itemScrollValue;
	float itemScrollValueTarget;
	float itemScrollValueWorld;

	void* hoverItem;
	uint32_t hoverItemIndex;
	void* editItem;

	void* clickedItems[2];
	uint32_t clickedItemsCurr;

	/*
	* If you need selection
	* return 1 if this item selected
	*/
	int(*onIsItemSelected)(struct mgElement_s* e, void* item);
	/*
	* select or deselect here
	* mouseButton: 1 - lmb, 2 - rmb, 3 - mmb
	*/
	void(*onItemClick)(struct mgElement_s* e, void* item, uint32_t itemIndex, uint32_t mouseButton);
	/*
	* Return 0 - no draw
	*        1 - draw (you must set `text` and `textlen`)
	*/
	int(*onDrawItem)(struct mgElement_s*, void* item, uint32_t itemIndex, mgUnicodeChar** text, uint32_t* textlen);
	
	mgUnicodeChar(*onTextInputCharEnter)(struct mgElement_s*, mgUnicodeChar);
	int(*onTextInputEndEdit)(struct mgElement_s*, int, const mgUnicodeChar*, void* editItem);


	struct mgElement_s* textInput;
	mgRect hoverItemClipRect;
	mgRect hoverItemBuildRect;
	const mgUnicodeChar* hoverItemText;

	int(*onGetUserElementNum)(struct mgElement_s*, void* item, uint32_t itemIndex);
	struct mgElement_s* (*onGetUserElement)(struct mgElement_s*, uint32_t index, void* item, uint32_t itemIndex);
	void (*onBeginGetUserElement)(struct mgElement_s*);
} mgElementList;



#endif