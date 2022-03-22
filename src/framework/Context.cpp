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
#include "framework/Context.h"
#include "framework/SystemWindowImpl.h"
#include "framework/Backend.h"
#include "framework/Window.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"

#ifdef CreateFont
#undef CreateFont
#endif

#ifdef CreateWindow
#undef CreateWindow
#endif

using namespace mgf;

Context::Context(
	int windowFlags,
	const mgPoint& windowPosition,
	const mgPoint& windowSize,
	Backend* backend)
{
	m_window = new mgf::SystemWindowImpl(windowFlags, windowPosition, windowSize);
	m_window->m_context = this;
	
	m_input = new mgInputContext_s;
	memset(m_input, 0, sizeof(mgInputContext));

	m_backend = backend;
	m_backend->SetActiveWindow(m_window);
	m_backend->SetActiveContext(this);
	m_backend->InitWindow(m_window);
	m_backend->UpdateBackbuffer();

	mgVideoDriverAPI* gpu = (mgVideoDriverAPI*)backend->GetVideoDriverAPI();
	m_gui_context = mgCreateContext(gpu, m_input);
	m_gui_context->getTextSize = m_backend->m_getTextSize;

	m_backend->GetDefaultFont();

	mgInitDefaultIcons(m_gui_context, m_backend->GetDefaultIcons());

	{
		RECT rc;
		GetClientRect(this->m_window->m_hWnd, &rc);
		m_gui_context->needRebuild = 1;
		m_gui_context->windowSize.x = rc.right - rc.left;
		m_gui_context->windowSize.y = rc.bottom - rc.top;
	}
	mgOnWindowSize(m_gui_context, m_gui_context->windowSize.x, m_gui_context->windowSize.y);
}

Context::~Context()
{
	if (m_gui_context)
	{
		mgDestroyContext(m_gui_context);
		m_gui_context = 0;
	}

	if (m_input)
		delete m_input;

	if (m_backend)
		m_backend->Release();

	if (m_window)
		m_window->Release();
}

void Context::OnWindowSize()
{
	m_gui_context->needRebuild = 1;
	m_gui_context->windowSize.x = m_window->m_size.x;
	m_gui_context->windowSize.y = m_window->m_size.y;
	mgOnWindowSize(m_gui_context, m_gui_context->windowSize.x, m_gui_context->windowSize.y);
}

Window* Context::CreateWindow()
{
	Window* newWindow = new Window;
	newWindow->m_window = mgCreateWindow(m_gui_context, 0, 0, 300, 200);
	newWindow->m_window->titlebarFont = ((mgf::FontImpl*)this->m_backend->GetDefaultFont())->m_font;
	return newWindow;
}

void Context::DrawAll()
{
	if (!m_gui_context)
		return;

	m_gui_context->gpu->beginDraw();
	mgDraw(m_gui_context);
	if (m_onDraw)
		m_onDraw(this, this->m_backend);
	m_gui_context->gpu->endDraw();
}

void Context::SetOnDraw(ContextOnDraw c)
{
	m_onDraw = c;
}

mgContext* Context::GetGUIContext()
{
	return m_gui_context;
}

mgf::SystemWindow* Context::GetSystemWindow()
{
	return m_window;
}

mgf::Backend* Context::GetBackend()
{
	return m_backend;
}

void Context::SetDefaultPopupFont(Font* f)
{
	m_gui_context->defaultPopupFont = ((mgf::FontImpl*)f)->m_font;
}
