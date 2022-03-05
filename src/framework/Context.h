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
#ifndef _MGF_CONTEXT_H_
#define _MGF_CONTEXT_H_

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace mgf
{
	using ContextOnDraw = void(*)(Context*,Backend*);

	class SystemWindowImpl;

	class Context : public BaseClass
	{
		friend class Framework;
		friend class SystemWindowImpl;

		SystemWindowImpl* m_window = 0;
		Backend* m_backend = 0;
		ContextOnDraw m_onDraw = 0;
	public:
		Context(int windowFlags,const mgPoint& windowPosition,const mgPoint& windowSize,Backend* backend);
		virtual ~Context();

		mgContext_s* m_gui_context = 0;
		mgInputContext_s* m_input = 0;

		mgContext_s* GetGUIContext();
		mgf::SystemWindow* GetSystemWindow();
		mgf::Backend* GetBackend();

		void OnWindowSize();
		
		mgf::Window* CreateWindow();

		// Will draw after drawing all windows
		void SetOnDraw(ContextOnDraw);
		
		void DrawAll();
	};
}

#endif