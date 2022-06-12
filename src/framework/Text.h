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
#ifndef _MGF_TEXT_H_
#define _MGF_TEXT_H_

#include "Element.h"

#include <string>

namespace mgf
{
	class Text : public Element
	{
		std::wstring m_text;
		mgElementText_s* m_elementText = 0;
		mgFont* m_font = 0;
		mgColor m_color;
		Window* m_window = 0;
		friend mgColor* Text_onColor(mgElement_s* e);
		friend mgFont* Text_onFont(struct mgElement_s* e);
		friend const wchar_t* Text_onText(struct mgElement_s* e, size_t* textLen);

	public:
		Text(Window* w, const wchar_t* t, Font* f);
		virtual ~Text();

		void SetText(const wchar_t*);
		void SetText(const char*);
		void SetTextF(const wchar_t*, ...);
		void SetTextF(const char*, ...);
		void SetFont(Font*);
		void SetPosition(mgPoint*);
		void SetPosition(int x, int y);
		void SetColor(mgColor*);
		void SetColor(int); // ARGB 0xFFrrggbb

		uint32_t GetWidth();
	};
}

#endif