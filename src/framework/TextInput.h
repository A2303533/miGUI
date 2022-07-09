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
#ifndef _MGF_TEXTINPUT_H_
#define _MGF_TEXTINPUT_H_

#include "Element.h"

#include <string>

namespace mgf
{
	class TextInput : public Element
	{
		mgElementTextInput_s* m_elementText;
	public:
		TextInput(Window* w);
		virtual ~TextInput();
		virtual void SetTextProcessor(TextProcessor*) override;

		// It will allocate memory and copy this text
		void SetText(const mgUnicodeChar*);

		// It will use pointer.
		void SetDefaultText(const mgUnicodeChar*);

		// 0 - no limit
		void SetCharLimit(uint32_t);

		const mgUnicodeChar* GetText();
		size_t GetTextSize();

		enum class EndEdit
		{
			_unknown,
			_enterKey,
			_click,
			_escapeKey,
		};
		/// <summary>
		/// When you enter text and press `Escape` or click somewhere
		/// you may want discard all changes. Do it here, by yourself.
		/// Return 1 if need to deactivate input
		/// </summary>
		virtual int OnEndEdit(EndEdit);

		/// <summary>
		/// Return this char if you want `enter` it to TextInput.
		/// Return 0 to skip.
		/// </summary>
		virtual mgUnicodeChar OnCharEnter(mgUnicodeChar);

		/// <summary>
		/// Will be called on activation.
		/// </summary>
		virtual void OnActivate();

		virtual int OnPaste(mgUnicodeChar*, size_t);
	};
}

#endif