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
#ifndef _MGF_TEXTPROC_H_
#define _MGF_TEXTPROC_H_

namespace mgf
{
	/// <summary>
	/// With this class you can set font and color for each character in text.
	/// All elements can have TextProcessor*. By default, they use default text processor.
	/// You must also implement OnGetTextSize because you decide what font to use and
	///   different fonts have different sizes.
	/// </summary>
	class TextProcessor : public BaseClass
	{
	protected:
		mgTextProcessor* m_tp = 0;
	public:
		//virtual void OnDrawText(int reason, mgUnicodeChar c) = 0;
		virtual mgFont_s* OnFont(int reason, mgUnicodeChar c) = 0;
		virtual mgColor* OnColor(int reason, mgUnicodeChar c, mgStyle_s* s) = 0;
		virtual void OnGetTextSize(int reason, const mgUnicodeChar* text, size_t textLen, mgPoint* p) = 0;

		mgTextProcessor* GetTextProcessor() { return m_tp; }
		void SetTextProcessor(mgTextProcessor* tp) { m_tp = tp; }
	};
}

#endif