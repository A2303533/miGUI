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

#ifndef _MG_BCK_GDI_H_
#define _MG_BCK_GDI_H_

#ifdef MGF_BACKEND_GDI

#include <windows.h>
#include <objidl.h>
#define GDIPVER 0x0110

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"Msimg32.lib")


#include "framework/OS/UndefWindows.h"

namespace mgf
{
	class SystemWindow;
	class Context;

	class BackendGDI : public Backend
	{
		friend class Context;
		friend class TextProcessor_GDI;

		friend void BackendGDI_onDrawText(int reason, mgElement_s*, struct mgTextProcessor_s* tp, mgPoint* position,
			const mgUnicodeChar* text, size_t textLen, struct mgColor_s* c);
		friend struct mgFont_s* BackendGDI_onFont(int reason, mgElement_s*, struct mgTextProcessor_s* tp, mgUnicodeChar c);
		friend struct mgColor_s* BackendGDI_onColor(int reason, mgElement_s*, struct mgTextProcessor_s* tp, mgUnicodeChar c);
		friend void BackendGDI_onGetTextSize(int reason, mgElement_s*, struct mgTextProcessor_s* tp, const mgUnicodeChar* text, size_t textLen, mgPoint* p);
		//mgSystemWindowOSData* m_currSystemWindowOSData = 0;

		mgf::SystemWindow* m_currWindow = 0;
		mgf::Context* m_context = 0;

		void* m_gpu = 0;
		Gdiplus::GdiplusStartupInput m_gdiplusStartupInput = 0;
		ULONG_PTR m_gdiplusToken = 0;

		mgTexture_s* m_defaultIcons = 0;
		Font* m_defaultFont = 0;

		mgRect m_clipRect;

		void _createBackbuffer();

		Image* blackImage = 0;
		Gdiplus::Bitmap* blackBitmap = 0;

		void OnDrawText(int reason, mgPoint* position, const mgUnicodeChar* text, size_t textLen, struct mgColor_s* c);
	public:
		BackendGDI(mgf::SystemWindow*);
		virtual ~BackendGDI();
		virtual void* GetVideoDriverAPI() override;

		virtual void InitWindow(mgf::SystemWindow*) override;
		virtual mgf::SystemWindow* SetCurrentWindow(mgf::SystemWindow*) override;
		virtual void SetActiveContext(mgf::Context*) override;

		virtual void BeginDraw(int reason) override;
		virtual void EndDraw() override;

		virtual mgTexture* CreateTexture(mgImage* img)  override;
		virtual void DestroyTexture(mgTexture*)  override;
		virtual void DrawRectangle(int reason,void* object,mgRect* rct,mgColor* color1,mgColor* color2,
			mgTexture* texture,  mgVec4* UVRegion ) override;
		virtual int DrawText(int reason,mgPoint* position,const mgUnicodeChar* text, size_t textLen,
			mgColor* color, mgFont* font) override;
		virtual void DrawLine( int reason, void* object, mgPoint* position, mgPoint* where,
			mgColor* color, int size) override;
		virtual mgRect SetClipRect(mgRect* r) override;
		virtual void GetTextSize(const mgUnicodeChar* text, size_t len, mgFont* font, mgPoint* sz) override;
		virtual mgTexture_s* GetDefaultIcons() override;
		virtual Font* CreateFont(const wchar_t* file, int size, bool bold, bool italic) override;
		virtual void DestroyFont(Font*) override;
		virtual Font* GetDefaultFont() override;
		virtual void SetIcon(mgf::Icons*, int id) override;
		virtual Font* CreateFontPrivate(const wchar_t* file, int size, bool bold, bool italic, const wchar_t* name) /*override*/;
		virtual void UpdateTexture(mgTexture* t, mgImage* i) override;

		virtual void UpdateBackBuffer() override;
		virtual void DeleteBackBuffer() override;
	};

}

#endif

#endif