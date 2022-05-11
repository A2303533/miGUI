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
#ifndef _MGF_BCK_H_
#define _MGF_BCK_H_

#ifdef MG_PLATFORM_WINDOWS
#include "framework/OS/UndefWindows.h"
#endif

namespace mgf
{
	class Backend : public BaseClass
	{
	protected:
		/// <summary>
		/// Indent for final drawing.
		/// For GDI its good to move it down that system menu will be draw correctly
		/// </summary>
		mgPoint m_endDrawIndent;
	public:

		Backend()
			:
			m_getTextSize(0)
		{}
	
		virtual ~Backend(){};

		virtual void SetEndDrawIndent(int x, int y)
		{
			m_endDrawIndent.x = x;
			m_endDrawIndent.y = y;
		}

		virtual void* GetVideoDriverAPI() = 0;

		/*
		* For example it's for SetPixelFormat if it's OpenGL
		*/
		virtual void InitWindow(mgf::SystemWindow*) = 0;
		virtual mgf::SystemWindow* SetCurrentWindow(mgf::SystemWindow*) = 0;
		virtual void SetActiveContext(mgf::Context*) = 0;

		virtual void BeginDraw(int reason) = 0;
		virtual void EndDraw() = 0;

		virtual mgTexture* CreateTexture(mgImage* img) = 0;
		virtual void DestroyTexture(mgTexture*) = 0;
		virtual void DrawRectangle(
			int reason,
			void* object,
			mgRect* rct,
			mgColor* color1,
			mgColor* color2,
			mgTexture* texture, /*optional*/
			mgVec4* UVRegion /*optional*/
		) = 0;
		virtual void DrawText(
			int reason,
			void* object,
			mgPoint* position,
			const wchar_t* text,
			int textLen,
			mgColor* color,
			mgFont* font) = 0;
		virtual void DrawLine(
			int reason, 
			void* object, 
			mgPoint* position, 
			mgPoint* where,
			mgColor* color, 
			int size) = 0;

		/*return prev rect*/
		virtual mgRect SetClipRect(mgRect* r) = 0;

		virtual void GetTextSize(const wchar_t* text, mgFont* font, mgPoint* sz) = 0;
		void (*m_getTextSize)(const wchar_t* text, mgFont*, mgPoint*);
		
		virtual mgTexture_s* GetDefaultIcons() = 0;
		virtual Font* CreateFont(const wchar_t* file, int size, bool bold, bool italic) = 0;
		virtual void DestroyFont(Font*) = 0;
		virtual Font* GetDefaultFont() = 0;
		
		/// <summary>
		/// Only for GDI. Create font from file.
		/// </summary>
		/// <param name="file"></param>
		/// <param name="size"></param>
		/// <param name="bold"></param>
		/// <param name="italic"></param>
		/// <param name="name"></param>
		/// <returns></returns>
		virtual Font* CreateFontPrivate(const wchar_t* file, int size, bool bold, bool italic, const wchar_t* name) { return 0; };

		virtual void SetIcon(mgf::Icons*, int id) {}
		
		/// <summary>
		/// Copy image to texture. Must be same size and same other parameters.
		/// </summary>
		/// <param name="t">- texture</param>
		/// <param name="i">- image</param>
		virtual void UpdateTexture(mgTexture* t, mgImage* i) {}

		/// <summary>
		/// Delete old buffers and create new
		/// </summary>
		virtual void UpdateBackBuffer() = 0;
		virtual void DeleteBackBuffer() = 0;
	};
}

#endif