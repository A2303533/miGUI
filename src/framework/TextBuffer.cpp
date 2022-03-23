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


#include "miGUI.h"

#include "framework/mgf.h"
#include "framework/TextBuffer.h"

#include <cwctype>

using namespace mgf;

TextBuffer::TextBuffer()
{

}

TextBuffer::~TextBuffer()
{
	Free();
}

void TextBuffer::Free()
{
	Clear();
	m_buffer.shrink_to_fit();
}

void TextBuffer::Clear()
{
	m_buffer.clear();
	m_begin = 0;
	m_end = 0;
	m_curr = 0;
}

void TextBuffer::FromBuffer(const char* u8char)
{
	Free();
	StringA stra = u8char;
	m_buffer = stra.to_utf16();
	_onInit();
}

void TextBuffer::FromBuffer(const wchar_t* u16char)
{
	Free();
	m_buffer = u16char;
	_onInit();
}

void TextBuffer::FromFile(const char* fn)
{
	Free();
	FILE* f = fopen(fn, "rb");
	if (!f)
		return;

	fseek(f, 0, SEEK_END);
	size_t sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (sz)
	{
		uint8_t bom[3] = { 0, 0, 0 };
		fread(bom, 1, 3, f);

		union uu
		{
			char ch[2];
			wchar_t wch;
		};
		if (bom[0] == 0xFF && bom[1] == 0xFE)
		{
			uu _u;

			fseek(f, 2, SEEK_SET);
			sz -= 2;
			if (sz)
			{
				while (!feof(f))
				{
					_u.wch = 0;
					fread(&_u.wch, 1, 2, f);
					if(_u.wch)
						m_buffer.push_back(_u.wch);
				}
			}
		}
		else if (bom[0] == 0xFE && bom[1] == 0xFF)
		{
			fseek(f, 2, SEEK_SET);
			sz -= 2;
			if (sz)
			{
				uu _u;
				uu _u2;
				while (!feof(f))
				{
					_u.wch = 0;
					fread(&_u.wch, 1, 2, f);
					if (_u.wch)
					{
						_u2.ch[0] = _u.ch[1];
						_u2.ch[1] = _u.ch[0];
						m_buffer.push_back(_u2.wch);
					}
				}
			}
		}
		else
		{
			// everything else probably utf-8
			if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
			{
				sz -= 3;
			}
			else
			{
				fseek(f, 0, SEEK_SET);
			}

			uint8_t* u8buf = (uint8_t*)malloc(sz+1);
			if (u8buf)
			{
				fread(u8buf, 1, sz, f);
				u8buf[sz] = 0;
				StringA stra = u8buf;
				m_buffer = stra.to_utf16();
				free(u8buf);
			}
		}
	}
	fclose(f);
	_onInit();
}

void TextBuffer::_onInit()
{
	if (m_buffer.size())
	{
		m_begin = m_buffer.data();
		m_end = m_begin + m_buffer.size();
		m_curr = m_begin;
	}
}

void TextBuffer::FromFile(const wchar_t* fn)
{
	Free();
	StringW strw = fn;
	StringA stra = strw.to_utf8();
	FromFile(stra.data());
}

bool TextBuffer::GetToken(StringW* out,uint32_t flags)
{
	out->clear();

	if (m_curr >= m_end)
	{
		m_curr = m_end;
		return false;
	}

	enum
	{
		mode_free,
		mode_word,
		mode_int,
		mode_hex,
		mode_science,
		mode_float
	}
	mode = mode_free;

	enum
	{
		symbol_alpha,
		symbol_digit,
		symbol_punct,
	}
	symbol = symbol_alpha;

	while (m_curr < m_end)
	{
		wchar_t ch = *m_curr;
		

		switch (ch)
		{
		case L' ':
		{
			if (flags & gettokenFlag_getSpace)
				out->append(ch);
			++m_curr;
			return true;
		}break;
		case L'\t':
		{
			if (flags & gettokenFlag_getTab)
				out->append(ch);
			++m_curr;
			return true;
		}break;
		case L'\n':
		{
			if (flags & gettokenFlag_getNewline)
				out->append(ch);
			++m_curr;
			return true;
		}break;

		case L'~':
		case L'`':
		case L'@':
		case L'\"':
		case L'\'':
		case L'#':
		case L'№':
		case L'$':
		case L'%':
		case L'^':
		case L':':
		case L'?':
		case L'&':
		case L'*':
		case L'(':
		case L')':
		case L'=':
		case L'+':
		case L'-':
		case L';':
		case L'{':
		case L'}':
		case L'[':
		case L']':
		case L'\\':
		case L'/':
		case L'|':
		case L'<':
		case L'>':
		case L',':
		case L'.':
			symbol = symbol_punct;
			break;
		}

		if (std::iswcntrl(ch))
		{
			++m_curr;
			continue;
		}

		if (std::iswdigit(ch))
			symbol = symbol_digit;

		switch (mode)
		{
		case mode_free:
		{
			switch (symbol)
			{
			case symbol_alpha:
				mode = mode_word;
				out->append(ch);
				break;
			case symbol_digit:
				mode = mode_int;
				out->append(ch);
				break;
			case symbol_punct:
				out->append(ch);
				++m_curr;
				return true;
			default:
				break;
			}
		}break;
		case mode_word:
		{
			if (symbol == symbol_punct)
				return true;
			out->append(ch);
		}break;
		case mode_int:
		{
			if (symbol == symbol_punct || symbol == symbol_alpha)
				return true;

			out->append(ch);
		}break;
		default:
			break;
		}

		++m_curr;
	}

	return true;
}

bool TextBuffer::GetLine(StringW* out)
{
	out->clear();

	if (m_curr >= m_end)
	{
		m_curr = m_end;
		return false;
	}

	while (m_curr < m_end)
	{
		if (*m_curr != L'\r' && *m_curr != L'\n')
		{
			out->append(*m_curr);
		}
		else
		{
			if (*m_curr == L'\r')
				++m_curr;
			if (*m_curr == L'\n')
			{
				++m_curr;
				break;
			}
		}

		++m_curr;
	}

	return true;
}

bool TextBuffer::IsEnd()
{
	return m_curr == m_end;
}

void TextBuffer::ToBegin()
{
	m_curr = m_begin;
}

void TextBuffer::SkipLine()
{
	while (m_curr < m_end)
	{
		if (*m_curr == L'\r')
			++m_curr;
		if (*m_curr == L'\n')
		{
			++m_curr;
			break;
		}

		++m_curr;
	}
}
