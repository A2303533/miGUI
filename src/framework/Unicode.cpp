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
	WcharToU8((StringW&)in, (StringU8&)out);
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

										uint16_t sh1 = 0xD800 + ((u - 0x10000) >> 10);
										uint16_t sh2 = 0xDC00 + (u - 0x10000);
										out.push_back(sh1);
										out.push_back(sh2);
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

					out.push_back(0xF0 | ((u & 0x1C0000) >> 18));
					out.push_back(0x80 | ((u & 0x3F000) >> 12));
					out.push_back(0x80 | ((u & 0xFC0) >> 6));
					out.push_back(0x80 | (u & 0x3F));
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

