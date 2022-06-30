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
#include "framework/WinReg.h"

using namespace mgf;

WinReg::WinReg()
{
}

WinReg::~WinReg()
{
	Close();
}

bool WinReg::Open(hKey hk, const wchar_t* subkey)
{
	Close();
	HKEY _hk = 0;
	switch (hk)
	{
	case mgf::WinReg::hKey_CLASSES_ROOT:
		_hk = HKEY_CLASSES_ROOT;
		break;
	case mgf::WinReg::hKey_CURRENT_USER:
		_hk = HKEY_CURRENT_USER;
		break;
	case mgf::WinReg::hKey_LOCAL_MACHINE:
		_hk = HKEY_LOCAL_MACHINE;
		break;
	}
	return ERROR_SUCCESS == RegOpenKeyExW(_hk, subkey, 0, KEY_ALL_ACCESS, &m_key);
}

bool WinReg::Create(hKey hk, const wchar_t* subkey)
{
	Close();
	HKEY _hk = 0;
	switch (hk)
	{
	case mgf::WinReg::hKey_CLASSES_ROOT:
		_hk = HKEY_CLASSES_ROOT;
		break;
	case mgf::WinReg::hKey_CURRENT_USER:
		_hk = HKEY_CURRENT_USER;
		break;
	case mgf::WinReg::hKey_LOCAL_MACHINE:
		_hk = HKEY_LOCAL_MACHINE;
		break;
	}
	return ERROR_SUCCESS == RegCreateKeyExW(_hk, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, 
		KEY_ALL_ACCESS, NULL, &m_key, 0);
}

void WinReg::Close()
{
	if (m_key)
	{
		RegCloseKey(m_key);
		m_key = 0;
	}
}

bool WinReg::GetString(const wchar_t* name, StringW& out)
{
	if (!m_key)
		return false;

	DWORD len = 0;
	if (ERROR_SUCCESS == RegGetValueW(m_key, 0, name, RRF_RT_REG_SZ, 0, 0, &len))
	{
		out.clear();
		out.reserve(len);
		out.setSize(len);

		return ERROR_SUCCESS == RegGetValueW(m_key, 0, name, RRF_RT_REG_SZ, 0, out.data(), &len);
	}
	return false;
}

bool WinReg::SetString(const wchar_t* namevalue, StringW& in)
{
	if (!m_key)
		return false;

	return ERROR_SUCCESS == RegSetValueEx(m_key, namevalue, 0, REG_SZ, (BYTE*)in.data(),
		in.size() * sizeof(wchar_t));
}
