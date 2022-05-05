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
#ifndef _MGF_TEXTBUFFER_H_
#define _MGF_TEXTBUFFER_H_

#include "String.h"

namespace mgf
{

	// A class that gives you the ability to work with text like a file.
	class TextBuffer : public BaseClass
	{
		void _onInit();
	public:
		TextBuffer();
		virtual ~TextBuffer();
		
		void Clear();
		void Free();
		
		void FromBuffer(const char* u8char);
		void FromBuffer(const wchar_t* u16char);
		void FromFile(const char* fn);
		void FromFile(const wchar_t* fn);
		
		bool GetLine(StringW* out);

		// Will return:
		// Words: abc1 _abc2 a_b3c
		// Numbers: only positive like 123 
		// other symbols
		bool GetToken(StringW* out, uint32_t gettokenFlags);
		enum
		{
			gettokenFlag_hexNumbers = 0x1,
			gettokenFlag_scientificNumbers = 0x2,
			gettokenFlag_getSpace = 0x4,
			gettokenFlag_getTab = 0x8,
			gettokenFlag_getNewline = 0x10,
			gettokenFlag_getFloat = 0x20, // 1.23 -1.23
		};


		StringW m_buffer;
		const wchar_t* m_begin;
		const wchar_t* m_end;
		const wchar_t* m_curr;

		bool IsEnd();
		void ToBegin();
		void SkipLine();
	};
}


#endif