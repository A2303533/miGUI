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
#ifndef _MGF_BUTTON_H_
#define _MGF_BUTTON_H_

#include "Rectangle.h"

#include "String.h"
#include "Unicode.h"

namespace mgf
{
	class Button : public Element
	{
		mgElementButton_s* m_elementButton = 0;
		
		UnicodeString m_text;

		Icons* m_icons = 0;
		mgFont* m_font = 0;
		//friend mgFont* Button_onFont(struct mgElement_s* e);
		friend const mgUnicodeChar* Button_onText(struct mgElement_s* e, size_t* textLen);
	public:
		Button(Window* w);
		virtual ~Button();
		virtual void SetTextProcessor(TextProcessor*) override;

		virtual void SetText(const char*);
		virtual void SetText(const wchar_t*);
		virtual void SetText(const char8_t*);
		virtual void SetText(const char16_t*);
		virtual void SetText(const char32_t*);
		virtual void SetText(const UnicodeString&);

		virtual void SetAsPush(bool);
		
		virtual void SetIcons(
			Icons*, 
			int id1, //default
			int id2, //hover
			int id3, //push
			int id4  //disabled
		);

		virtual void OnPushOn(); /*only when pushButton */
		virtual void OnPushOff();

		virtual void SetTexture(mgTexture*);
		
		virtual void SetPushState(bool);
	};
}

#endif