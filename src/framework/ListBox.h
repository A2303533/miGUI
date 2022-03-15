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
		ListBox(Window* w, void* arr, uint32_t arrSz, uint32_t dataTypeSizeOf, Font*);
		virtual ~ListBox();

		void SetMultiselect(bool);

		// Only for NOT multiselect
		void SetCurSel(uint32_t);
		uint32_t GetCurSel();
		
		void SetSelectWithRMB(bool);
		void SetDrawItemBG(bool);

		uint32_t GetArraySize();

		void SetFont(Font*);
		void SetItemHeight(uint32_t);
		void CanEdit(bool);
		void NoDeselect(bool);

		// 0 - no limit;
		void SetTextInputCharLimit(uint32_t);
		
		void SetData(void* arr, uint32_t arrSz);
		
		wchar_t(*onTextInputCharEnter)(ListBox*, wchar_t) = 0;
		/*
		* i:
		*	1 - Enter
		*   2 - click somewhere
		*   3 - Escape
		* str: new text
		* editItem: first byte for edited item
		*/
		int(*onTextInputEndEdit)(ListBox*, int i, const wchar_t* str, uint8_t* editItem) = 0;

		/*return 1 if need to select*/
		int(*onSelect)(ListBox*, uint8_t* item);
	};
}

#endif