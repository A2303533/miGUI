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
#ifndef _MGF_UNICODE_H_
#define _MGF_UNICODE_H_

#include "String.h"

namespace mgf
{
	struct UnicodeCharNode
	{
		uint32_t m_utf8;
		uint32_t m_utf16;
	};

	UnicodeCharNode* GetUnicodeTable();

	class UnicodeConverter
	{
	public:
		UnicodeConverter() {}
		~UnicodeConverter() {}

		void CharToWchar(StringA&, StringW&);
		void CharToU8(StringA&, StringU8&);
		void CharToU16(StringA&, StringU16&);
		
		void WcharToChar(StringW&, StringA&);
		void WcharToU8(StringW&, StringU8&);
		void WcharToU16(StringW&, StringU16&);

		void U8ToChar(StringU8&, StringA&);
		void U8ToWchar(StringU8&, StringW&);
		void U8ToU16(StringU8&, StringU16&);

		void U16ToChar(StringU16&, StringA&);
		void U16ToWchar(StringU16&, StringW&);
		void U16ToU8(StringU16&, StringU8&);
		
		char m_defaultCharacter = ' ';
	};
}

#endif