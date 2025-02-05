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
#ifndef _MGF_WINREG_H_
#define _MGF_WINREG_H_

#include "String.h"
#include "Array.h"

namespace mgf
{
	class WinReg
	{
		HKEY m_key = 0;
	public:

		enum hKey
		{
			hKey_CLASSES_ROOT,
			hKey_CURRENT_USER,
			hKey_LOCAL_MACHINE,
		};

		WinReg();
		~WinReg();

		bool Open(hKey, const wchar_t* subkey);
		bool Create(hKey, const wchar_t* subkey);
		void Close();

		bool GetString(const wchar_t* name, StringW& out);
		bool SetString(const wchar_t* name, StringW& in);
		bool GetString(const char* name, StringA& out);
		bool SetString(const char* name, StringA& in);
		
		bool SetBinary(const char* name, void* in, size_t size);
		bool GetBinary(const char* name, mgf::Array<uint8_t>& out);
	};
}

#endif