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
#ifndef _MGF_LISTBOX_H_
#define _MGF_LISTBOX_H_

#include "Rectangle.h"

#include <string>

namespace mgf
{
	enum ListBoxFlag
	{
		ListBoxFlag_select = 0x1,
	};

	class ListBox : public Element
	{
		mgElementList_s* m_elementList = 0;
	public:
		ListBox(Window* w);
		virtual ~ListBox();
		virtual void SetTextProcessor(TextProcessor*) override;

		void SetDrawItemBG(bool);
		
		void SetData(void** arr, uint32_t arrSz);
		void** GetData();

		//void SetFont(Font*); // must be SetTextProcessor

		void SetItemHeight(uint32_t);
		void CanEdit(bool);

		// 0 - no limit;
		void SetTextInputCharLimit(uint32_t);
		
		//wchar_t(*onTextInputCharEnter)(ListBox*, wchar_t);
		virtual mgUnicodeChar OnTextInputCharEnter(ListBox*, mgUnicodeChar);

		// i:
		//	1 - Enter
		//  2 - click somewhere
		//  3 - Escape
		// str: new text
		// editItem: first byte for edited item
		//int(*onTextInputEndEdit)(ListBox*, int i, const wchar_t* str, void* editItem);
		virtual int OnTextInputEndEdit(ListBox*, int i, const mgUnicodeChar* str, void* editItem);

		// If you need selection
		// return 1 if this item selected
		//int(*onIsItemSelected)(ListBox* e, void* item);
		virtual int OnIsItemSelected(ListBox* e, void* item);

		// select or deselect here
		// mouseButton: 1 - lmb, 2 - rmb, 3 - mmb
		//void(*onItemClick)(ListBox* e, void* item, uint32_t itemIndex, uint32_t mouseButton);
		virtual void OnItemClick(ListBox* e, void* item, uint32_t itemIndex, uint32_t mouseButton);


		// Return 0 - no draw
		//       1 - draw (you must set `text` and `textlen`)
		//int(*onDrawItem)(ListBox*, void* item, uint32_t itemIndex, wchar_t** text, uint32_t* textlen);
		virtual int OnDrawItem(ListBox*, void* item, uint32_t itemIndex, mgUnicodeChar** text, size_t* textlen);

	};
}

#endif