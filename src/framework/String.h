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
#ifndef _MGF_STRING_H_
#define _MGF_STRING_H_

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cwchar>
#include <vector>

namespace mgf
{
	const uint32_t String_wordSize = 16;
	namespace _internal
	{
		const float string_to_float_table[17] =
		{
			0.f,
			0.1f,
			0.01f,
			0.001f,
			0.0001f,
			0.00001f,
			0.000001f,
			0.0000001f,
			0.00000001f,
			0.000000001f,
			0.0000000001f,
			0.00000000001f,
			0.000000000001f,
			0.0000000000001f,
			0.00000000000001f,
			0.000000000000001f,
			0.0000000000000001f,
		};
	}

	template<typename char_type>
	class String_base
	{
		typedef char_type* pointer;
		typedef const char_type* const_pointer;
		typedef char_type& reference;
		typedef const char_type& const_reference;
		typedef String_base this_type;
		typedef const String_base& this_const_reference;

		pointer m_data;
		size_t     m_allocated;
		size_t     m_size;

		void reallocate(size_t new_allocated)
		{
			char_type* new_data = (char_type*)std::malloc(new_allocated * sizeof(char_type));
			if (m_data)
			{
				std::memcpy(new_data, m_data, m_size * sizeof(char_type));
				std::free(m_data);
			}
			else
			{
				std::memset(new_data, 0, new_allocated);
			}
			m_data = new_data;
			m_allocated = new_allocated;
		}

		template<typename other_type>
		size_t getlen(const other_type* str)
		{
			unsigned int len = 0u;
			if (str[0] != 0)
			{
				const other_type* p = &str[0u];
				while ((size_t)*p++)
					len++;
			}
			return len;
		}

		template<typename dst_type, typename src_type>
		void copy(dst_type* dst, src_type* src) const
		{
			while ((size_t)*src)
			{
				*dst = static_cast<dst_type>(*src);
				dst++;
				src++;
			}
		}


	public:
		typedef char_type value_type;

		String_base()
			:m_data(nullptr), m_allocated(String_wordSize), m_size(0)
		{
			reallocate(m_allocated);
		}

		template<typename other_type>
		String_base(const other_type* str)
			: m_data(nullptr), m_allocated(String_wordSize), m_size(0)
		{
			reallocate(m_allocated);
			assign(str);
		}

		String_base(this_const_reference str)
			: m_data(nullptr), m_allocated(String_wordSize), m_size(0)
		{
			reallocate(m_allocated);
			assign(str);
		}

		String_base(this_type&& str)
			: m_data(nullptr), m_allocated(String_wordSize), m_size(0)
		{
			reallocate(m_allocated);
			assign(str);
		}

		String_base(char_type c)
			: m_data(nullptr), m_allocated(String_wordSize), m_size(0)
		{
			size_t new_size = 1u;
			reallocate((new_size + 1u) + String_wordSize);
			m_data[0u] = c;
			m_size = new_size;
			m_data[m_size] = static_cast<char_type>(0x0);
		}

		~String_base() {
			if (m_data)
				std::free(m_data);
		}

		size_t capacity() { return m_allocated; }

		void to_lower() {
			if (!m_size)
				return;
			for (size_t i = 0u; i < m_size; ++i)
			{
				auto c = m_data[i];
				if (c <= 'Z' && c >= 'A')
					m_data[i] += 32;
			}
		}

		//	abc -> cba
		void flip()
		{
			if (!m_size)
				return;
			String_base<char_type> flippedStr;
			for (size_t i = m_size - 1u; i >= 0u; --i)
			{
				flippedStr += m_data[i];
				if (!i)
					break;
			}
			this->operator=(flippedStr);
		}

		// '\\' -> '/'
		void flip_slash()
		{
			if (!m_size)
				return;
			for (size_t i = 0u; i < m_size; ++i)
			{
				if (m_data[i] == '\\')
					m_data[i] = '/';
			}
		}

		// '/' -> '\\'
		void flip_backslash()
		{
			if (!m_size)
				return;
			for (size_t i = 0u; i < m_size; ++i)
			{
				if (m_data[i] == '/')
					m_data[i] = '\\';
			}
		}

		void reserve(size_t size) {
			if (size > m_allocated)
			{
				reallocate(size);
				m_data[m_size] = 0;
			}
		}

		template<typename other_type>
		void assign(other_type str) {
			m_size = 0u;
			m_data[m_size] = static_cast<char_type>(0x0);
			if (str)
				append(str);
		}

		void assign(this_const_reference str) {
			m_size = 0u;
			m_data[m_size] = static_cast<char_type>(0x0);
			append(str);
		}

		template<typename other_type>
		void append(const other_type* str) {
			size_t new_size = getlen(str) + m_size;

			if ((new_size + 1u) > m_allocated)
				reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));

			copy(&m_data[m_size], str);

			m_size = new_size;
			m_data[m_size] = static_cast<char_type>(0x0);
		}

		void append(this_const_reference str) {
			append(str.data());
		}

		void push_back(char_type c) {
			size_t new_size = m_size + 1u;
			if ((new_size + 1u) > m_allocated)
				reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));
			m_data[m_size] = c;
			m_size = new_size;
			m_data[m_size] = 0;
		}
		void append(char_type c) {
			size_t new_size = m_size + 1u;
			if ((new_size + 1u) > m_allocated)
				reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));
			m_data[m_size] = c;
			m_size = new_size;
			m_data[m_size] = 0;
		}

		void append(int32_t c) {
			char buf[32u];
			::std::sprintf(buf, "%i", c);
			append(buf);
		}

		void append(uint32_t c) {
			char buf[32u];
			::std::sprintf(buf, "%u", c);
			append(buf);
		}

		void append(uint64_t c) {
			char buf[32u];
			::std::sprintf(buf, "%llu", c);
			append(buf);
		}

		void append(double c) {
			char buf[32u];
			::std::sprintf(buf, "%.12f", c);
			append(buf);
		}

		void append(float c) {
			char buf[32u];
			::std::sprintf(buf, "%.4f", c);
			append(buf);
		}
		void append_float(float c) {
			char buf[32u];
			::std::sprintf(buf, "%.7f", c);
			append(buf);
		}

		const_pointer c_str() const { return m_data; }
		pointer data() const { return m_data; }
		const size_t size() const { return m_size; }

		void setSize(size_t size)
		{
			m_size = size;
		}

		this_type& operator=(this_const_reference str) {
			assign(str);
			return *this;
		}

		this_type& operator=(this_type&& str) {
			assign(str);
			return *this;
		}


		template<typename other_type>
		this_type& operator=(other_type* str) {
			assign(str);
			return *this;
		}

		this_type operator+(const_pointer str) {
			this_type ret(*this);
			ret.append(str);
			return ret;
		}

		this_type operator+(this_const_reference str) {
			return operator+(str.data());
		}

		this_type operator+(size_t num) {
			this_type r(*this);
			r.append(num);
			return r;
		}

		pointer begin() const { return m_data; }
		pointer end() const { return (m_data + (m_size)); }
		const_reference operator[](size_t i) const { return m_data[i]; }
		reference operator[](size_t i) { return m_data[i]; }
		void operator+=(int i) { append(i); }
		void operator+=(uint32_t i) { append(i); }
		void operator+=(uint64_t i) { append(i); }
		void operator+=(float i) { append(i); }
		void operator+=(double i) { append(i); }
		void operator+=(char_type c) { append(c); }

		template<typename other_type>
		void operator+=(other_type* str) { append(str); }
		void operator+=(this_const_reference str) { append(str); }

		bool operator==(this_const_reference other) const {
			if (other.size() != m_size)
				return false;

			const size_t sz = other.size();
			const auto* ptr = other.data();
			for (size_t i = 0u; i < sz; ++i)
			{
				if (ptr[i] != m_data[i])
					return false;
			}

			return true;
		}

		bool operator!=(this_const_reference other) const {
			if (other.size() != m_size)
				return true;

			const size_t sz = other.size();
			const auto* ptr = other.data();
			for (size_t i = 0u; i < sz; ++i)
			{
				if (ptr[i] != m_data[i])
					return true;
			}

			return false;
		}

		void clear() {
			m_size = 0u;
			m_data[m_size] = 0;
		}

		void pop_back() {
			if (m_size)
			{
				--m_size;
				m_data[m_size] = 0;
			}
		}

		char_type pop_back_return() {
			char_type r = '?';
			if (m_size)
			{
				--m_size;
				r = m_data[m_size];
				m_data[m_size] = 0;
			}
			return r;
		}

		void replace(char_type a, char_type b)
		{
			for (size_t i = 0; i < m_size; ++i)
			{
				if (m_data[i] == a)
					m_data[i] = b;
			}
		}

		void pop_back_before(char_type before_this) {
			if (size())
				pop_back();

			if (size())
			{
				for (size_t i = size() - 1u; i >= 0u; --i)
				{
					if (m_data[i] == (char_type)before_this)
						break;
					else
						pop_back();
					if (!i)
						break;
				}
			}
		}

		void shrink_to_fit() {
			if (m_size)
			{
				if (m_allocated > (m_size + (1 + (uint32_t)(m_size * 0.5f))))
				{
					reallocate(m_size + 1u);
					m_data[m_size] = static_cast<char_type>(0x0);
				}
			}
		}

		void set_size(size_t size) {
			m_size = size;
		}

		void erase(size_t begin, size_t end) {
			size_t numCharsToDelete = end - begin + 1u; // delete first char: 0 - 0 + 1
			size_t next = end + 1u;
			for (size_t i = begin; i < m_size; ++i)
			{
				if (next < m_size)
				{
					m_data[i] = m_data[next];
					++next;
				}
			}
			m_size -= numCharsToDelete;
			m_data[m_size] = static_cast<char_type>(0x0);
		}

		void pop_front() {
			erase(0u, 0u);
		}

		bool is_space(size_t index) {
			if (m_data[index] == (char_type)' ') return true;
			if (m_data[index] == (char_type)'\r') return true;
			if (m_data[index] == (char_type)'\n') return true;
			if (m_data[index] == (char_type)'\t') return true;
			return false;
		}

		String_base<wchar_t> to_string() const
		{
			String_base<wchar_t> ret;
			ret.reserve(m_size);
			ret = m_data;
			return ret;
		}

		int32_t to_int()
		{
			size_t len = m_size;
			int result = 0;
			int mul_val = 1;
			bool is_neg = m_data[0] == (char_type)'-';
			for (size_t i = 0, last = len - 1; i < len; ++i)
			{
				int char_value = (int)m_data[last] - 0x30;
				if (char_value >= 0 && char_value <= 9)
				{
					result += char_value * mul_val;
					mul_val *= 10;
				}
				--last;
			}
			if (is_neg)
				result = -result;
			return result;
		}

		float to_float()
		{
			float result = 0.f;
			auto ptr = m_data;
			bool is_negative = *ptr == (char_type)'-';

			if (is_negative)
				++ptr;

			int i = 0;
			while (*ptr >= (char_type)'0' && *ptr <= (char_type)'9')
			{
				i *= 10;
				i += *ptr - (char_type)'0';
				++ptr;
			}
			result = (float)i;

			i = 0;

			if (*ptr == (char_type)'.')
				++ptr;

			int part_2_count = 0;
			while (*ptr >= (char_type)'0' && *ptr <= (char_type)'9')
			{
				i *= 10;
				i += *ptr - (char_type)'0';
				++ptr;
				++part_2_count;
			}
			result += (float)i * _internal::string_to_float_table[part_2_count];

			return is_negative ? -result : result;
		}

		String_base<char> to_utf8() const
		{

			/*size_t sz = m_size;
			for (size_t i = 0u; i < sz; ++i)
			{
				char_type ch16 = m_data[i];
				if (ch16 < 0x80)
				{
					output += (char)ch16;
				}
				else if (ch16 < 0x800)
				{
					output += (char)((ch16 >> 6) | 0xc0);
					output += (char)((ch16 & 0x3f) | 0x80);
				}
			}*/

			const wchar_t* old = m_data;

			String_base<char> output;
			std::mbstate_t state = std::mbstate_t();
			std::size_t len = 1 + std::wcsrtombs(nullptr, (const wchar_t**)&old, 0, &state);
			std::vector<char> mbstr(len);
			std::wcsrtombs(&mbstr[0], (const wchar_t**)&old, mbstr.size(), &state);
			output.append(&mbstr[0]);
			return output;
		}

		String_base<wchar_t> to_utf16() const
		{
			String_base<wchar_t> output;
			const char* old = m_data;

			std::mbstate_t state = std::mbstate_t();
			std::size_t len = 1 + std::mbsrtowcs(nullptr, &old, 0, &state);
			std::vector<wchar_t> wstr(len);
			std::mbsrtowcs(&wstr[0], &old, wstr.size(), &state);
			output.append(&wstr[0]);

			//auto utf8_data = m_data;
			//while (*utf8_data)
			//{
			//	unsigned char utf8_char = *utf8_data;
			//	++utf8_data;

			//	if (utf8_char >= 0 && utf8_char < 0x80) // one byte
			//	{
			//		output.append((wchar_t)utf8_char);
			//	}
			//	else if (utf8_char >= 0xC0 && utf8_char < 0xE0) // 2
			//	{
			//		unsigned char utf8_char2 = *utf8_data;
			//		if (utf8_char2)
			//		{
			//			++utf8_data;

			//			wchar_t char16 = 0;
			//			char16 = utf8_char;
			//			char16 ^= 0xC0;
			//			char16 <<= 6;
			//			char16 |= (utf8_char2 ^ 0x80);

			//			output.append(char16);
			//		}
			//	}
			//	else if (utf8_char >= 0xE0 && utf8_char < 0xF0) // 3
			//	{
			//		unsigned char utf8_char2 = *utf8_data;
			//		if (utf8_char2)
			//		{
			//			++utf8_data;
			//			unsigned char utf8_char3 = *utf8_data;
			//			if (utf8_char3)
			//			{
			//				++utf8_data;

			//				wchar_t char16 = 0;
			//				char16 = utf8_char;
			//				char16 ^= 0xE0;
			//				char16 <<= 12;
			//				char16 |= (utf8_char2 ^ 0x80) << 6;
			//				char16 |= (utf8_char3 ^ 0x80);

			//				output.append(char16);
			//			}
			//		}
			//	}
			//}
			return output;
		}

		template<typename other_type>
		void insert(const other_type* str, size_t where) {
			size_t str_len = getlen(str);
			size_t new_size = str_len + m_size;
			if ((new_size + 1u) > m_allocated)
				reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));

			size_t i = m_size;
			while (i >= where)
			{
				m_data[i + str_len] = m_data[i];
				if (i == 0)
					break;
				--i;
			}
			for (size_t i = 0; i < str_len; ++i)
			{
				m_data[i + where] = str[i];
			}
			m_size = new_size;
			m_data[m_size] = 0;
		}

		void insert(char_type c, size_t where)
		{
			size_t new_size = m_size + 1u;
			if ((new_size + 1u) > m_allocated)
				reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));

			size_t i = m_size;
			while (i >= where)
			{
				auto next = i + 1;
				if (next < m_allocated)
					m_data[next] = m_data[i];

				if (i == 0)
					break;
				--i;
			}
			m_data[where] = c;
			m_size = new_size;
			m_data[m_size] = 0;
		}

		void trim_spaces()
		{
			for (size_t i = 0, sz = m_size; i < sz; ++i)
			{
				if (m_data[m_size - 1] == (char_type)' '
					|| m_data[m_size - 1] == (char_type)'\f'
					|| m_data[m_size - 1] == (char_type)'\n'
					|| m_data[m_size - 1] == (char_type)'\r'
					|| m_data[m_size - 1] == (char_type)'\t'
					|| m_data[m_size - 1] == (char_type)'\v')
				{
					pop_back();
				}
				else
					break;
			}
			
			if (m_data[0] == (char_type)' '
				|| m_data[0] == (char_type)'\f'
				|| m_data[0] == (char_type)'\n'
				|| m_data[0] == (char_type)'\r'
				|| m_data[0] == (char_type)'\t'
				|| m_data[0] == (char_type)'\v')
			{
				int howMany = 0;
				for (size_t i = 0, sz = m_size; i < sz; ++i)
				{
					if (m_data[i] == (char_type)' '
						|| m_data[i] == (char_type)'\f'
						|| m_data[i] == (char_type)'\n'
						|| m_data[i] == (char_type)'\r'
						|| m_data[i] == (char_type)'\t'
						|| m_data[i] == (char_type)'\v')
					{
						++howMany;
					}
					else
						break;
				}

				if (howMany == m_size)
					clear();
				else
					erase(0, howMany-1);
			}
		}
	};

	typedef String_base<wchar_t> StringW;
	typedef String_base<char> StringA;
}


#endif