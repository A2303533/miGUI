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
#ifndef _MGF_UNICODE_H_
#define _MGF_UNICODE_H_

#include "String.h"
#include <string>

namespace mgf
{
	mgUnicodeCharNode* GetUnicodeTable();

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

	/// <summary>
	/// Char is Unicode code point. Probably this is UTF-32 string.
	/// </summary>
	class UnicodeString
	{
	private:
		mgUnicodeChar* m_data = 0;
		size_t m_size = 0;
		size_t m_allocated = 0;

		void reallocate(size_t new_allocated);
	public:
		UnicodeString()
		{
			reallocate(0xF);
		}

		UnicodeString(const UnicodeString& other)
			:
			UnicodeString()
		{
			this->operator=(other);
		}

		UnicodeString& operator=(const UnicodeString& other)
		{
			Clear();
			Reserve(other.m_size);
			m_size = other.m_size;
			for (uint32_t i = 0; i < m_size; ++i)
			{
				m_data[i] = other.m_data[i];
			}
			m_data[m_size] = 0;
			return *this;
		}

		UnicodeString(UnicodeString&& other)
			:
			UnicodeString()
		{
			this->operator=(std::move(other));
		}
		UnicodeString& operator=(UnicodeString&& other)
		{
			free(m_data);
			m_data = other.m_data;
			m_allocated = other.m_allocated;
			m_size = other.m_size;

			other.m_data = 0;
			other.m_allocated = 0;
			other.m_size = 0;
			return *this;
		}

		UnicodeString(const mgUnicodeChar* str)
		{
			if (str)
			{
				size_t len = mgUnicodeStrlen(str);
				if (len)
					Assign(str, len);
			}
		}

		UnicodeString& operator=(const mgUnicodeChar* str) 
		{
			if (str)
			{
				size_t len = mgUnicodeStrlen(str);
				if (len)
					Assign(str, len);
			}
			return *this;
		}

		UnicodeString(const char* str, size_t len);
		UnicodeString(const wchar_t* str, size_t len);
		UnicodeString(const char8_t* str, size_t len);
		UnicodeString(const char16_t* str, size_t len);
		UnicodeString(const char32_t* str, size_t len);
		UnicodeString(const StringA& str);
		UnicodeString(const StringW& str);
		UnicodeString(const StringU8& str);
		UnicodeString(const StringU16& str);
		UnicodeString(const std::string& str);
		UnicodeString(const std::wstring& str);
		UnicodeString(const std::u16string& str);
		UnicodeString(const std::u32string& str);

		~UnicodeString() 
		{
			if (m_data)
				delete[] m_data;
		}

		/// <summary>
		/// Always do this if(str) a.Assign(str, len);
		/// </summary>
		void Assign(const char* str, size_t len);
		void Assign(const wchar_t* str, size_t len);
		void Assign(const char8_t* str, size_t len);
		void Assign(const char16_t* str, size_t len);
		void Assign(const char32_t* str, size_t len);
		void Assign(const StringA& str);
		void Assign(const StringW& str);
		void Assign(const StringU8& str);
		void Assign(const StringU16& str);
		void Assign(const std::string& str);
		void Assign(const std::wstring& str);
		void Assign(const std::u16string& str);
		void Assign(const std::u32string& str);
		void Assign(const UnicodeString& str);

		void Clear();
		void ClearFree();
		void Reserve(size_t);
		size_t Size() const;
		const mgUnicodeChar* Data() const;
		mgUnicodeChar* Data();

		void Append(char c);
		void Append(wchar_t c);
		void Append(char8_t c);
		void Append(char16_t c);
		void Append(char32_t c);
		void Append(const char* str, size_t len);
		void Append(const wchar_t* str, size_t len);
		void Append(const char8_t* str, size_t len);
		void Append(const char16_t* str, size_t len);
		void Append(const char32_t* str, size_t len);
		void Append(const StringA& str);
		void Append(const StringW& str);
		void Append(const StringU8& str);
		void Append(const StringU16& str);
		void Append(const std::string& str);
		void Append(const std::wstring& str);
		void Append(const std::u16string& str);
		void Append(const std::u32string& str);
		void Append(const UnicodeString& str);
		void Append(uint32_t);
		void Append(uint64_t);
		void Append(int32_t);
		void Append(float);
		void Append(double);

		void operator+=(char i) { Append(i); }
		void operator+=(wchar_t i) { Append(i); }
		void operator+=(char8_t i) { Append(i); }
		void operator+=(char16_t i) { Append(i); }
		void operator+=(char32_t i) { Append(i); }
		void operator+=(const char* str) { Append(str, strlen(str)); }
		void operator+=(const wchar_t* str) { Append(str, wcslen(str)); }
		void operator+=(const char8_t* str) { Append(str, strlen((const char*)str)); }
		void operator+=(const char16_t* str) { Append(str, wcslen((const wchar_t*)str)); }
		void operator+=(const char32_t* str) { Append(str, mgUnicodeStrlen(str)); }
		void operator+=(int32_t i) { Append(i); }
		void operator+=(uint32_t i) { Append(i); }
		void operator+=(uint64_t i) { Append(i); }
		void operator+=(float i) { Append(i); }
		void operator+=(double i) { Append(i); }

		void Get(StringA& str);
		void Get(StringW& str);
		void Get(StringU8& str);
		void Get(StringU16& str);
		void Get(std::string& str);
		void Get(std::wstring& str);
		void Get(std::u16string& str);
		void Get(std::u32string& str);

		void PopBack();
		void PopBackBefore(mgUnicodeChar before_this);
	};
}

#endif