/*
  Copyright (C) 2021 Basov Artyom
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
	uint32_t isSelected;
	uint32_t curSel;
	uint32_t multiselect;/*0*/
	uint32_t editText;/*0*/
	uint32_t selectWithRMB;/*0*/
	uint32_t drawItemBG;/*0*/
	uint32_t useF2ForEdit;/*1*/
	uint32_t noDeselect;/*0, Some item will be always selected. 
						Select item by yourself. 
						Only for not multisilect.*/

	void* array;
	uint32_t arraySize;

	uint32_t firstItemIndexForDraw;/*internal*/
	uint32_t numOfLines;/*internal*/
	int32_t itemHeight;
	struct mgFont_s* font;

	float itemScrollValue;
	float itemScrollValueTarget;
	float itemScrollValueWorld;

	uint32_t dataTypeSizeOf;
	uint8_t* hoverItem;
	uint8_t* editItem;

	uint8_t* clickedItems[2];
	uint32_t clickedItemsCurr;

	/*return 1 if need to select*/
	int(*onSelect)(struct mgElement_s* e, uint8_t* item);
	
	wchar_t(*onTextInputCharEnter)(struct mgElement_s*, wchar_t);
	int(*onTextInputEndEdit)(struct mgElement_s*, int, const wchar_t*, uint8_t* editItem);


	struct mgElement_s* textInput;
	mgRect hoverItemClipRect;
	mgRect hoverItemBuildRect;
	const wchar_t* hoverItemText;
} mgElementList;



#endif