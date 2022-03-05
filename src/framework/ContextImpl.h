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

#ifndef _MGF_CONTEXT_IMPL_H_
#define _MGF_CONTEXT_IMPL_H_

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace mgf
{
	class SystemWindowImpl;

	class ContextImpl : public Context
	{
	public:
		ContextImpl(int windowFlags,
			const mgPoint& windowPosition,
			const mgPoint& windowSize,
			Backend* backend);
		virtual ~ContextImpl();

		virtual mgf::SystemWindow* GetSystemWindow() override;
		virtual mgf::Backend* GetBackend() override;

		virtual void OnWindowSize() override;
		virtual mgf::Window* CreateWindow() override;
		virtual void SetOnDraw(ContextOnDraw) override;

		void DrawAll();

		SystemWindowImpl* m_window = 0;
		mgInputContext m_input;
		mgContext* m_gui_context = 0;
		Backend* m_backend = 0;
		ContextOnDraw m_onDraw = 0;
	};
}

#endif