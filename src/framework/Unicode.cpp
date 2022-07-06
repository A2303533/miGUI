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
#include "framework/Unicode.h"

using namespace mgf;

union UC
{
	uint8_t bytes[4];
	uint16_t shorts[2];
	uint32_t integer;
};

mgUnicodeCharNode* mgf::GetUnicodeTable()
{
	return mgGetUnicodeTable();
}

void UnicodeConverter::CharToWchar(StringA& in, StringW& out)
{
	U8ToU16((StringU8&)in, (StringU16&)out);
}

void UnicodeConverter::CharToU8(StringA& in, StringU8& out)
{
	out.clear();
	size_t sz = in.size();
	out.reserve(sz);
	for (size_t i = 0; i < sz; ++i)
	{
		out.push_back(in[i]);
	}
}

void UnicodeConverter::CharToU16(StringA& in, StringU16& out)
{
	U8ToU16((StringU8&)in, (StringU16&)out);
}

void UnicodeConverter::WcharToChar(StringW& in, StringA& out)
{
	U16ToU8((StringU16&)in, (StringU8&)out);
}

void UnicodeConverter::WcharToU8(StringW& in, StringU8& out)
{
	U16ToU8((StringU16&)in, (StringU8&)out);
}

void UnicodeConverter::WcharToU16(StringW& in, StringU16& out)
{
	U16ToWchar((StringU16&)in, (StringW&)out);
}

void UnicodeConverter::U8ToChar(StringU8& in, StringA& out)
{
	CharToU8((StringA&)in, (StringU8&)out);
}

void UnicodeConverter::U8ToWchar(StringU8& in, StringW& out)
{
	U8ToU16((StringU8&)in, (StringU16&)out);
}

void UnicodeConverter::U8ToU16(StringU8& in, StringU16& out)
{
	out.clear();
	size_t sz = in.size();
	auto ut = mgGetUnicodeTable();

	unsigned char c1 = 0;
	unsigned char c2 = 0;
	unsigned char c3 = 0;
	unsigned char c4 = 0;
	for (size_t i = 0; i < sz; ++i)
	{
		c1 = in[i];

		if (c1 <= 0x7F)
		{
			out.push_back((char16_t)c1);
		}
		else
		{
			if ((c1 & 0xE0) == 0xC0) //2 bytes
			{
				++i;
				if (i < sz)
				{
					c2 = in[i];
					if ((c2 & 0xC0) == 0x80)
					{
						char16_t wch = (c1 & 0x1F) << 6;
						wch |= (c2 & 0x3F);
						out.push_back(wch);
					}
				}
			}
			else if ((c1 & 0xF0) == 0xE0) //3
			{
				++i;
				if (i < sz)
				{
					c2 = in[i];
					if ((c2 & 0xC0) == 0x80)
					{
						++i;
						if (i < sz)
						{
							c3 = in[i];
							if ((c3 & 0xC0) == 0x80)
							{
								char16_t wch = (c1 & 0xF) << 12;
								wch |= (c2 & 0x3F) << 6;
								wch |= (c3 & 0x3F);
								out.push_back(wch);
							}
						}
					}
				}
			}
			else if ((c1 & 0xF8) == 0xF0) //4
			{
				++i;
				if (i < sz)
				{
					c2 = in[i];
					if ((c2 & 0xC0) == 0x80)
					{
						++i;
						if (i < sz)
						{
							c3 = in[i];
							if ((c3 & 0xC0) == 0x80)
							{
								++i;
								if (i < sz)
								{
									c4 = in[i];
									if ((c4 & 0xC0) == 0x80)
									{
										uint32_t u = (c1 & 0x7) << 18;
										u |= (c2 & 0x3F) << 12;
										u |= (c3 & 0x3F) << 6;
										u |= (c4 & 0x3F);

									//	out.push_back(0xD800 + ((u & 0xFFC00) >> 10));
									//	out.push_back(0xDC00 + (u & 0x3FF));
										out.push_back(MG_HI32(ut[u].m_utf16));
										out.push_back(MG_LO32(ut[u].m_utf16));
									}
								}
							}
						}
					}
				}
			}

		}
	}
}

void UnicodeConverter::U16ToChar(StringU16& in, StringA& out)
{
	U16ToU8((StringU16&)in, (StringU8&)out);
}

void UnicodeConverter::U16ToWchar(StringU16& in, StringW& out)
{
	out.clear();
	size_t sz = in.size();
	out.reserve(sz);
	for (size_t i = 0; i < sz; ++i)
	{
		out.push_back(in[i]);
	}
}

void UnicodeConverter::U16ToU8(StringU16& in, StringU8& out)
{
	out.clear();
	size_t sz = in.size();
	auto ut = mgGetUnicodeTable();

	for (size_t i = 0u; i < sz; ++i) 
	{
		char16_t ch16 = in[i];
		if (ch16 < 0x80) 
		{
			out.push_back((char8_t)ch16);
		}
		else if (ch16 < 0x800) 
		{
			out.push_back((char8_t)((ch16 >> 6) | 0xC0));
			out.push_back((char8_t)((ch16 & 0x3F) | 0x80));
		}
		else if ((ch16 & 0xFC00) == 0xD800)
		{
			++i;
			if (i < sz)
			{
				char16_t ch16_2 = in[i];
				if ((ch16_2 & 0xFC00) == 0xDC00)
				{
					uint32_t u = (ch16 - 0xD800) * 0x400;
					u += (ch16_2 - 0xDC00) + 0x10000;

				//	out.push_back(0xF0 + ((u & 0x1C0000) >> 18));
				//	out.push_back(0x80 + ((u & 0x3F000) >> 12));
				//	out.push_back(0x80 + ((u & 0xFC0) >> 6));
				//	out.push_back(0x80 + (u & 0x3F));
					uint8_t* buf = (uint8_t*)&ut[u].m_utf8;
					out.push_back(buf[3]);
					out.push_back(buf[2]);
					out.push_back(buf[1]);
					out.push_back(buf[0]);
				}
			}
		}
		else
		{
			out.push_back((char8_t)((ch16 & 0xF000) >> 12) + 0xE0);
			out.push_back((char8_t)((ch16 & 0xFC0) >> 6) + 0x80);
			out.push_back((char8_t)(ch16 & 0x3F) + 0x80);
		}
	}
}

UnicodeString::UnicodeString(const char* str, size_t len) :UnicodeString()
{
	this->Assign(str, len);
}

UnicodeString::UnicodeString(const wchar_t* str, size_t len) : UnicodeString()
{
	this->Assign(str, len);
}

UnicodeString::UnicodeString(const char8_t* str, size_t len) : UnicodeString()
{
	this->Assign(str, len);
}

UnicodeString::UnicodeString(const char16_t* str, size_t len) : UnicodeString()
{
	this->Assign(str, len);
}

UnicodeString::UnicodeString(const char32_t* str, size_t len) : UnicodeString()
{
	this->Assign(str, len);
}

UnicodeString::UnicodeString(const StringA& str) : UnicodeString()
{
	this->Assign(str);
}

UnicodeString::UnicodeString(const StringW& str) : UnicodeString()
{
	this->Assign(str);
}

UnicodeString::UnicodeString(const StringU8& str) : UnicodeString()
{
	this->Assign(str);
}

UnicodeString::UnicodeString(const StringU16& str) : UnicodeString()
{
	this->Assign(str);
}

UnicodeString::UnicodeString(const std::string& str) : UnicodeString()
{
	this->Assign(str);
}

UnicodeString::UnicodeString(const std::wstring& str) : UnicodeString()
{
	this->Assign(str);
}

UnicodeString::UnicodeString(const std::u16string& str) : UnicodeString()
{
	this->Assign(str);
}

UnicodeString::UnicodeString(const std::u32string& str) : UnicodeString()
{
	this->Assign(str);
}

void UnicodeString::Assign(const char* str, size_t len)
{
	Assign((const char8_t*)str, len);
}

void UnicodeString::Assign(const wchar_t* str, size_t len)
{
	Assign((const char16_t*)str, len);
}

void UnicodeString::Assign(const char8_t* str, size_t len)
{
	Clear();
	Append(str, len);
}

void UnicodeString::Assign(const char16_t* str, size_t len)
{
	Clear();
	Append(str, len);
}

void UnicodeString::Assign(const char32_t* str, size_t len)
{
	Clear();
	Append(str, len);
}

void UnicodeString::Assign(const StringA& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const StringW& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const StringU8& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const StringU16& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const std::string& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const std::wstring& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const std::u16string& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const std::u32string& str)
{
	Assign(str.data(), str.size());
}

void UnicodeString::Assign(const UnicodeString& str)
{
	Assign(str.m_data, str.m_size);
}

void UnicodeString::Clear()
{
	m_size = 0;
	m_data[0] = 0;
}

size_t UnicodeString::Size() const
{
	return m_size;
}

const mgUnicodeChar* UnicodeString::Data() const
{
	return m_data;
}

mgUnicodeChar* UnicodeString::Data()
{
	return m_data;
}

void UnicodeString::Append(char c)
{
	Append((char32_t)c);
}

void UnicodeString::Append(wchar_t c)
{
	Append((char32_t)c);
}

void UnicodeString::Append(char8_t c)
{
	Append((char32_t)c);
}

void UnicodeString::Append(char16_t c)
{
	Append((char32_t)c);
}

void UnicodeString::Append(char32_t c)
{
	size_t new_size = m_size + 1;

	if ((new_size + 1u) > m_allocated)
		reallocate((new_size + 1u) + (1 + (uint32_t)(m_size * 0.5f)));

	m_data[m_size] = c;

	m_size = new_size;
	m_data[m_size] = static_cast<mgUnicodeChar>(0x0);
}

void UnicodeString::Append(const char* str, size_t len)
{
	Append((const char8_t*)str, len);
}

void UnicodeString::Append(const wchar_t* str, size_t len)
{
	Append((const char16_t*)str, len);
}

void UnicodeString::Append(const char8_t* str, size_t len)
{
	unsigned char c1 = 0;
	unsigned char c2 = 0;
	unsigned char c3 = 0;
	unsigned char c4 = 0;
	for (size_t i = 0; i < len; ++i)
	{
		c1 = str[i];

		if (c1 <= 0x7F)
		{
			Append((char32_t)c1);
		}
		else
		{
			if ((c1 & 0xE0) == 0xC0) //2 bytes
			{
				++i;
				if (i < len)
				{
					c2 = str[i];
					if ((c2 & 0xC0) == 0x80)
					{
						char16_t wch = (c1 & 0x1F) << 6;
						wch |= (c2 & 0x3F);
						Append((char32_t)wch);
					}
				}
			}
			else if ((c1 & 0xF0) == 0xE0) //3
			{
				++i;
				if (i < len)
				{
					c2 = str[i];
					if ((c2 & 0xC0) == 0x80)
					{
						++i;
						if (i < len)
						{
							c3 = str[i];
							if ((c3 & 0xC0) == 0x80)
							{
								char16_t wch = (c1 & 0xF) << 12;
								wch |= (c2 & 0x3F) << 6;
								wch |= (c3 & 0x3F);
								Append((char32_t)wch);
							}
						}
					}
				}
			}
			else if ((c1 & 0xF8) == 0xF0) //4
			{
				++i;
				if (i < len)
				{
					c2 = str[i];
					if ((c2 & 0xC0) == 0x80)
					{
						++i;
						if (i < len)
						{
							c3 = str[i];
							if ((c3 & 0xC0) == 0x80)
							{
								++i;
								if (i < len)
								{
									c4 = str[i];
									if ((c4 & 0xC0) == 0x80)
									{
										uint32_t u = (c1 & 0x7) << 18;
										u |= (c2 & 0x3F) << 12;
										u |= (c3 & 0x3F) << 6;
										u |= (c4 & 0x3F);

										Append((char32_t)u);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void UnicodeString::Append(const char16_t* str, size_t len)
{
	for (size_t i = 0u; i < len; ++i)
	{
		char16_t ch16 = str[i];
		if (ch16 < 0x80)
		{
			Append((char32_t)ch16);
		}
		else if (ch16 < 0x800)
		{
			Append((char32_t)ch16); //????
		}
		else if ((ch16 & 0xFC00) == 0xD800)
		{
			++i;
			if (i < len)
			{
				char16_t ch16_2 = str[i];
				if ((ch16_2 & 0xFC00) == 0xDC00)
				{
					uint32_t u = (ch16 - 0xD800) * 0x400;
					u += (ch16_2 - 0xDC00) + 0x10000;

					Append((char32_t)u);
				}
			}
		}
		else
		{
			Append((char32_t)ch16);
		}
	}
}

void UnicodeString::Append(const char32_t* str, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		Append(str[i]);
	}
}

void UnicodeString::Append(const StringA& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const StringW& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const StringU8& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const StringU16& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const std::string& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const std::wstring& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const std::u16string& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const std::u32string& str)
{
	Append(str.data(), str.size());
}

void UnicodeString::Append(const UnicodeString& str)
{
	Append(str.Data(), str.Size());
}

void UnicodeString::Get(StringA& str)
{
	str.clear();
	UC uc;
	auto ut = mgGetUnicodeTable();
	for (size_t i = 0; i < m_size; ++i)
	{
		mgUnicodeChar c = m_data[i];
		if (c >= 0x32000)
			c = '?';
		
		uc.integer = ut[c].m_utf8;

		if (uc.bytes[3]) str.push_back(uc.bytes[3]);
		if (uc.bytes[2]) str.push_back(uc.bytes[2]);
		if (uc.bytes[1]) str.push_back(uc.bytes[1]);
		if (uc.bytes[0]) str.push_back(uc.bytes[0]);
	}
}

void UnicodeString::Get(StringW& str)
{
	str.clear();
	UC uc;
	auto ut = mgGetUnicodeTable();
	for (size_t i = 0; i < m_size; ++i)
	{
		mgUnicodeChar c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf16;

		if (uc.shorts[1]) str.push_back(uc.shorts[1]);
		if (uc.shorts[0]) str.push_back(uc.shorts[0]);
	}
}

void UnicodeString::Get(StringU8& str)
{
	str.clear();
	UC uc;
	auto ut = mgGetUnicodeTable();
	for (size_t i = 0; i < m_size; ++i)
	{
		mgUnicodeChar c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf8;

		if (uc.bytes[3]) str.push_back(uc.bytes[3]);
		if (uc.bytes[2]) str.push_back(uc.bytes[2]);
		if (uc.bytes[1]) str.push_back(uc.bytes[1]);
		if (uc.bytes[0]) str.push_back(uc.bytes[0]);
	}
}

void UnicodeString::Get(StringU16& str)
{
	str.clear();
	UC uc;
	auto ut = mgGetUnicodeTable();
	for (size_t i = 0; i < m_size; ++i)
	{
		mgUnicodeChar c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf16;

		if (uc.shorts[1]) str.push_back(uc.shorts[1]);
		if (uc.shorts[0]) str.push_back(uc.shorts[0]);
	}
}

void UnicodeString::Get(std::string& str)
{
	str.clear();
	UC uc;
	auto ut = mgGetUnicodeTable();
	for (size_t i = 0; i < m_size; ++i)
	{
		mgUnicodeChar c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf8;

		if (uc.bytes[3]) str.push_back(uc.bytes[3]);
		if (uc.bytes[2]) str.push_back(uc.bytes[2]);
		if (uc.bytes[1]) str.push_back(uc.bytes[1]);
		if (uc.bytes[0]) str.push_back(uc.bytes[0]);
	}
}

void UnicodeString::Get(std::wstring& str)
{
	str.clear();
	UC uc;
	auto ut = mgGetUnicodeTable();
	for (size_t i = 0; i < m_size; ++i)
	{
		mgUnicodeChar c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf16;

		if (uc.shorts[1]) str.push_back(uc.shorts[1]);
		if (uc.shorts[0]) str.push_back(uc.shorts[0]);
	}
}

void UnicodeString::Get(std::u16string& str)
{
	str.clear();
	UC uc;
	auto ut = mgGetUnicodeTable();
	for (size_t i = 0; i < m_size; ++i)
	{
		mgUnicodeChar c = m_data[i];
		if (c >= 0x32000)
			c = '?';

		uc.integer = ut[c].m_utf16;

		if (uc.shorts[1]) str.push_back(uc.shorts[1]);
		if (uc.shorts[0]) str.push_back(uc.shorts[0]);
	}
}

void UnicodeString::Get(std::u32string& str)
{
	str.clear();
	for (size_t i = 0; i < m_size; ++i)
	{
		str.push_back(m_data[i]);
	}
}

void UnicodeString::PopBack()
{
	if (m_size)
	{
		--m_size;
		m_data[m_size] = 0;
	}
}

void UnicodeString::PopBackBefore(mgUnicodeChar before_this)
{
	if (Size())
		PopBack();

	if (Size())
	{
		for (size_t i = Size() - 1u; i >= 0u; --i)
		{
			if (m_data[i] == before_this)
				break;
			else
				PopBack();
			if (!i)
				break;
		}
	}
}

void UnicodeString::Append(uint32_t v)
{
	mgUnicodeChar buf[20];
	int l = mgUnicodeSnprintf(buf, 20, U"%u", v);
	Append(buf, l);
}

void UnicodeString::Append(int32_t v)
{
	mgUnicodeChar buf[20];
	int l = mgUnicodeSnprintf(buf, 20, U"%i", v);
	Append(buf, l);
}

void UnicodeString::Append(float v)
{
	mgUnicodeChar buf[20];
	int l = mgUnicodeSnprintf(buf, 20, U"%f", v);
	Append(buf, l);
}

void UnicodeString::Append(double v)
{
	mgUnicodeChar buf[30];
	int l = mgUnicodeSnprintf(buf, 30, U"%f", v);
	Append(buf, l);
}
