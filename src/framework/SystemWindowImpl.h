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
#ifndef _MG_SYSWIND_IMPL_H_
#define _MG_SYSWIND_IMPL_H_

#ifdef MG_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace mgf
{
	/*class FrameworkImpl;*/
	class SystemWindowImpl : public SystemWindow
	{
	public:
		SystemWindowImpl(int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize);
		virtual ~SystemWindowImpl();
		
		virtual const SystemWindowOSData& GetOSData() override;

		virtual void SetTitle(const wchar_t*) override;
		virtual void Show() override;
		virtual void Hide() override;
		virtual const mgPoint& GetSize() override;
		virtual void UpdateBackbuffer() override;
		virtual void SetOnClose(SystemWindowOnClose) override;
		virtual void SetOnSize(SystemWindowOnSize) override;
		virtual bool IsVisible() override;

		bool m_isVisible = false;

#ifdef _WIN32
		HWND m_hWnd = 0;
		//HDC m_dc = 0;
		wchar_t m_className[20];

		/*double bufferring for GDI*/
		HDC m_hdcMem = 0;
		HBITMAP m_hbmMem = 0;
		HBITMAP m_hbmOld = 0;
#endif
		SystemWindowOSData m_OSData;

		Context* m_context = 0;
		mgPoint m_size;
		mgPoint m_borderSize;
		SystemWindowOnClose m_onClose = 0;
		SystemWindowOnSize m_onSize = 0;
	};

}

#endif