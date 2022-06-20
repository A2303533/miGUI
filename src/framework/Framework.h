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
#ifndef _MGF_FMWK_H_
#define _MGF_FMWK_H_

#include "Image.h"
#include "String.h"
#include "Color.h"

#include <vector>

#ifdef LoadImage
#undef LoadImage
#endif

namespace mgf
{
	class Framework : public BaseClass
	{
		std::vector<Context*> m_contexts;
		StringW m_appDirectory;
		void _initColors();

	public:
		Framework();
		virtual ~Framework();

	private:
		//int m_isCURLReady = 0;
	public:
		//void InitCURL();

		bool m_run;

		Context* CreateContext(mgf::SystemWindow*, Backend* backend);

		Icons* CreateIcons(const char* imageFile, Backend* backend);
		Icons* CreateIcons(const wchar_t* imageFile, Backend* backend);
		Icons* CreateIcons(const uint8_t* buffer, uint32_t bufferSize, ImageLoader, Backend* backend);

		Image* LoadImage(const uint8_t* buffer, uint32_t bufferSize, ImageLoader);
		Image* LoadImage(const char* imageFile);
		Image* LoadImage(const wchar_t* imageFile);

		

		mgStyle_s GetNewStyle(int isLightTheme);
		/*
		* Call every frame.
		* Return true if system work
		*/
		bool Run();
		void DrawAll();
		
		mgColor m_colors[static_cast<uint32_t>(ColorName::__count)];
		mgColor GetColor(ColorName);
		mgColor* GetColorPtr(ColorName);

		StringW* GetAppDir();
	};

	Framework* InitFramework(); // init or return
	Framework* GetFramework();  // only return
}

#endif