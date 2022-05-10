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
#ifndef _MG_SYSWIND_H_
#define _MG_SYSWIND_H_

#ifdef MG_PLATFORM_WINDOWS
#include <Windows.h>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

/*Just like WINAPI*/
#define MGWS_OVERLAPPED       0x00000000L
#define MGWS_POPUP            0x80000000L
#define MGWS_CHILD            0x40000000L
#define MGWS_MINIMIZE         0x20000000L
#define MGWS_VISIBLE          0x10000000L
#define MGWS_DISABLED         0x08000000L
#define MGWS_CLIPSIBLINGS     0x04000000L
#define MGWS_CLIPCHILDREN     0x02000000L
#define MGWS_MAXIMIZE         0x01000000L
#define MGWS_CAPTION          0x00C00000L
#define MGWS_BORDER           0x00800000L
#define MGWS_DLGFRAME         0x00400000L
#define MGWS_VSCROLL          0x00200000L
#define MGWS_HSCROLL          0x00100000L
#define MGWS_SYSMENU          0x00080000L
#define MGWS_THICKFRAME       0x00040000L
#define MGWS_GROUP            0x00020000L
#define MGWS_TABSTOP          0x00010000L
#define MGWS_MINIMIZEBOX      0x00020000L
#define MGWS_MAXIMIZEBOX      0x00010000L
#define MGWS_TILED            MGWS_OVERLAPPED
#define MGWS_ICONIC           MGWS_MINIMIZE
#define MGWS_SIZEBOX          MGWS_THICKFRAME
#define MGWS_TILEDWINDOW      MGWS_OVERLAPPEDWINDOW
#define MGWS_OVERLAPPEDWINDOW (MGWS_OVERLAPPED     | \
                             MGWS_CAPTION        | \
                             MGWS_SYSMENU        | \
                             MGWS_THICKFRAME     | \
                             MGWS_MINIMIZEBOX    | \
                             MGWS_MAXIMIZEBOX)
#define MGWS_POPUPWINDOW      (MGWS_POPUP          | \
                             MGWS_BORDER         | \
                             MGWS_SYSMENU)
#define MGWS_CHILDWINDOW      (MGWS_CHILD)
#define MGCW_USEDEFAULT       ((int)0x80000000)

namespace mgf
{
	// now mgSystemWindowOSData
	/*struct SystemWindowOSData
	{
		SystemWindowOSData()
			: handle(0)
		{}
		void* handle;
	};*/

	class SystemWindow : public BaseClass
	{
		friend class BackendD3D11;
		friend class BackendGDI;
		friend class BackendOpenGL;
		friend class Context;
		friend class Framework;

		struct mgSystemWindowOSData m_OSData;
#ifdef MG_PLATFORM_WINDOWS
		wchar_t m_className[20];
		//HWND m_hWnd = 0;
		/*double bufferring for GDI*/
		//HDC m_hdcMem = 0;
		//HBITMAP m_hbmMem = 0;
		//HBITMAP m_hbmOld = 0;
#endif
		//mgSystemWindowOSData m_OSdata;

		void* m_userData = 0;
		bool m_isVisible = false;

		mgRect m_clientRect;
		mgPoint m_size;
		mgPoint m_borderSize;
		mgPoint m_sizeMinimum;

		bool m_isCustomTitlebar = false;
		mgRect m_customTitlebarHitRect;
		uint32_t m_customTitlebarSize = 20;
	public:

		/// <summary>
		/// No initialization
		/// </summary>
		SystemWindow();

		SystemWindow(int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize);
		virtual ~SystemWindow();

		mgSystemWindowOSData* GetOSData();

		void SetUserData(void*);
		void* GetUserData();
		const mgPoint& GetSize();
		
		virtual void SetTitle(const wchar_t*);
		virtual void SetVisible(bool);
		virtual bool IsVisible();

		// Only set m_size
		virtual void SetSize(int x, int y);

		mgPoint* GetSizeMinimum();

		virtual void UpdateBackbuffer();

		// it's for update back buffer and other thigs.
		virtual void OnSize();
		
		virtual void OnDraw();
		virtual void OnMove();

		bool IsUseCustomTitleBar();
		void SetUseCustomTitleBar(bool);
		mgRect* GetCustomTitleBarHitRect();
#ifdef MG_PLATFORM_WINDOWS
		void OnNCCalcSize();
#endif
		virtual void OnDrawCustomTitleBar();
		// return true if need to move window when click on titlebar.
		// cursor is already in m_customTitlebarHitRect.
		// You can return false if there is some GUI controls...
		virtual bool OnHTCaption();
		
		uint32_t GetCustomTitleBarSize();
		void SetCustomTitleBarSize(uint32_t);
		
		virtual void OnActivate();
		virtual void OnDeactivate();

		virtual void OnMaximize();
		virtual void OnMinimize();
		virtual void OnRestore();

		virtual void Maximize();
		virtual void Minimize();
		virtual void Restore();

		const mgPoint& GetBorderSize();

		// return true close application
		virtual bool OnClose();
		
		// call this after SetUseCustomTitleBar
		virtual void Rebuild();

		Context* m_context = 0;
	};
}

#endif