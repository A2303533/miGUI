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
#include "miGUILoader.h"

#include "framework/mgf.h"
#include "framework/mgfImpl.h"
#include "framework/ContextImpl.h"
#include "framework/SystemWindowImpl.h"


using namespace mgf;

Backend* g_backend = 0;
FrameworkImpl* g_mgf = 0;

#ifndef MG_NO_DLL
MG_LIB_HANDLE g_migui_dll = 0;
#endif

Framework* mgf::InitFramework()
{
#ifndef MG_NO_DLL
	g_migui_dll = mgLoad();
	if (!g_migui_dll)
	{
		throw "Can't load migui.dll";
	}
#endif
	g_mgf = new FrameworkImpl();
	return g_mgf;
}

FrameworkImpl::FrameworkImpl()
{
}

FrameworkImpl::~FrameworkImpl()
{
#ifndef MG_NO_DLL
	if(g_migui_dll)
		mgUnload(g_migui_dll);
#endif
}


bool FrameworkImpl::Run()
{
	for (size_t i = 0, sz = m_contexts.size(); i < sz; ++i)
	{
		auto c = m_contexts[i];
		mgStartFrame(c->m_gui_context);
	}

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	for (size_t i = 0, sz = m_contexts.size(); i < sz; ++i)
	{
		auto c = m_contexts[i];
		if(c->m_window->m_isVisible)
			mgUpdate(c->m_gui_context);
	}

	return m_run;
}

void FrameworkImpl::DrawAll()
{
	for (size_t i = 0, sz = m_contexts.size(); i < sz; ++i)
	{
		auto c = m_contexts[i];
		if (!c->m_window->m_isVisible)
			continue;
		c->m_gui_context->gpu->beginDraw();
		mgDraw(c->m_gui_context);
		c->m_gui_context->gpu->endDraw();
	}
}

Context* FrameworkImpl::CreateContext(
	SystemWindow::Type t,
	const mgPoint& windowPosition,
	const mgPoint& windowSize,
	Backend* backend
)
{
	ContextImpl* c = new ContextImpl(t, windowPosition, windowSize, backend);
	m_contexts.push_back(c);
	return c;
}
