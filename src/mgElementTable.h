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
#ifndef _MG_ELEMENTTABLE_H_
#define _MG_ELEMENTTABLE_H_

typedef struct mgElementTable_s {
	struct mgFont_s* font;
	uint32_t numRows;
	uint32_t numCols;

	uint32_t multiselect;/*0*/

	void** rows;
	uint32_t rowHeight;
	int* colsSizes; /*array[numCols], width for columns*/

	struct mgElement_s* textInput;

	/*
	* Return 0 - no draw
	*        1 - this is wchar_t* (you must set `text` and `textlen`)
	*        
	*/
	int(*onDrawRow)(struct mgElement_s*, void* row, uint32_t col, wchar_t** text, uint32_t* textlen);
	
	/*
	* If you want selection you must add this callbacks:
	*/
	/*
	* Return 1 if this row selected
	*/
	int(*onIsRowSelected)(struct mgElement_s*, void* row);
	/*
	* mouseButton: 1 - lmb, 2 - rmb, 3 - mmb
	*/
	void(*onRowClick)(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t mouseButton);
	/*
	* return 1 for textInput
	*/
	int(*onCellClick)(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t colIndex, uint32_t mouseButton);
	
	const wchar_t* (*onCellTextInputActivate)(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t colIndex);
	wchar_t(*onCellTextInputCharEnter)(struct mgElement_s*, wchar_t c);
	int(*onCellTextInputEndEdit)(struct mgElement_s*, int type, const wchar_t* textinputText, void* row, uint32_t rowIndex, uint32_t colIndex);

	uint32_t firstRowIndexForDraw;
	float rowScrollValue;
	float rowScrollValueTarget;
	float rowScrollValueWorld;
	float scrollSpeed;/*10.f*/
	void* hoverRow;
	uint32_t hoverRowIndex;
	uint32_t hoverColIndex;
	void* editRow;
	uint32_t editRowIndex;
	uint32_t editColIndex;

	uint32_t numOfLines;

	uint32_t drawItemBG;

	mgRect hoverRowClipRect;
	mgRect hoverRowBuildRect;
	mgRect hoverCellClipRect;

	/*
	* set this for column title, that can change column width.
	* return text for current column, set also textLen.
	* 
	*/
	const wchar_t* (*onColTitleText)(struct mgElement_s*, uint32_t* textLen, uint32_t colIndex);
	uint32_t colTitleHeight; /*20*/
	int activeColTitle;
	int hoverColTitle;
	void(*onColTitleClick)(struct mgElement_s*, uint32_t colIndex, uint32_t mouseButton);

	int cursorInColSplitter;
	int colSplitterMode;
	uint32_t colIndexSplitterMode;

	int(*onGetUserElementNum)(struct mgElement_s*, void* row, uint32_t rowIndex, uint32_t colIndex);
	struct mgElement_s* (*onGetUserElement)(struct mgElement_s*, uint32_t index);
} mgElementTable;



#endif