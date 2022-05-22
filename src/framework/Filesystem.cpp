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

#include "mgf.h"
#include "Filesystem.h"

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace mgf;

filesystem::path& filesystem::path::assign(const char* str)
{
    m_string.clear();
    std::mbstate_t state = std::mbstate_t();
    std::size_t len = 1 + std::mbsrtowcs(0, &str, 0, &state);
    std::vector<wchar_t> wstr(len);
    std::mbsrtowcs(&wstr[0], &str, wstr.size(), &state);
    m_string.append(&wstr[0]);
    return *this;
}

bool filesystem::exists(const filesystem::path& p)
{
#ifdef _WIN32
	unsigned int attr = GetFileAttributesW(p.c_str());
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		return true;
		//switch (attr)
		//{
		//default: return true;
		//case FILE_ATTRIBUTE_DIRECTORY: return __CRT_EXST_DIR;
		//}
	}
#endif

    return false;
}

std::string filesystem::path::string() const
{
	const wchar_t* old = m_string.c_str();
	std::basic_string<char> output;
	std::mbstate_t state = std::mbstate_t();
	std::size_t len = 1 + std::wcsrtombs(0, (const wchar_t**)&old, 0, &state);
	std::vector<char> mbstr(len);
	std::wcsrtombs(&mbstr[0], (const wchar_t**)&old, mbstr.size(), &state);
	output.append(&mbstr[0]);
	return output;
}

std::wstring filesystem::path::wstring() const
{
	return m_string;
}

filesystem::path filesystem::path::filename() const 
{
	const wchar_t* str = m_string.c_str();
	filesystem::path p;
	if (m_string.size())
	{
		size_t slashIndex = 0;
		for (size_t i = 0, sz = m_string.size(); i < sz; ++i)
		{
			if (str[i] == L'/' || str[i] == L'\\')
				slashIndex = i;
		}

		str = m_string.c_str();
		size_t startIndex = slashIndex;

		if (slashIndex)
		{
			++startIndex;
		}
		else
		{
			if(str[0] == L'/' || str[0] == L'\\')
				++startIndex;
		}

		while (true)
		{
			if (str[startIndex] == 0)
				break;

			p.m_string += str[startIndex];
			++startIndex;
		}
	}
	return p;
}

filesystem::path filesystem::path::extension() const
{
	const wchar_t* str = m_string.c_str();
	filesystem::path p;
	if (m_string.size()){
		size_t dotIndex = 0;
		for (size_t i = 0, sz = m_string.size(); i < sz; ++i){
			if (str[i] == L'.')
				dotIndex = i;
			if (str[i] == L'/' || str[i] == L'\\')
				dotIndex = 0;
		}
		str = m_string.c_str();
		if (dotIndex)
		{
			if (str[dotIndex] == L'.')
			{
				if ((str[dotIndex - 1] != L'/') && (str[dotIndex - 1] != L'\\'))
				{
					if (isgraph(str[dotIndex + 1]))
					{
						size_t startIndex = dotIndex;
						while (true) {
							if (str[startIndex] == 0)
								break;
							p.m_string += str[startIndex];
							++startIndex;
						}
					}
					else
					{
						if (dotIndex > 1)
						{
							if (str[dotIndex - 1] == L'.' && str[dotIndex - 2] == L'/')
								return p;
							if (str[dotIndex - 1] == L'.' && str[dotIndex - 2] == L'\\')
								return p;
							p.m_string += L'.';
						}
					}
				}
			}
		}
	}
	return p;
}


bool filesystem::is_regular_file(const path& p)
{
#ifdef _WIN32
	unsigned int attr = GetFileAttributesW(p.c_str());
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		return true;

		switch (attr)
		{
		default: 
			return true;
		case FILE_ATTRIBUTE_DIRECTORY: return false;
		}
	}
#endif

	return false;
}

std::uintmax_t filesystem::file_size(const filesystem::path& p)
{
	std::uintmax_t sz = 0;

	std::string stra = p.string();
	FILE* f = fopen(stra.c_str(), "rb");
	if (!f)
		return 0;

#ifdef _MSC_VER
	_fseeki64(f, 0, SEEK_END);
	sz = _ftelli64(f);
#else
#endif

	fclose(f);

	return sz;
}
