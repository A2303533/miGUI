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
#include "framework/ContextImpl.h"
#include "framework/SystemWindowImpl.h"

using namespace mgf;

ContextImpl::ContextImpl(
	SystemWindow::Type t,
	const mgPoint& windowPosition,
	const mgPoint& windowSize,
	Backend* backend)
{
	m_window = new mgf::SystemWindowImpl(t);
	m_window->m_context = this;
	
	m_backend = backend;
	m_backend->SetActiveWindow(m_window);
	m_backend->InitWindow(m_window);
	m_backend->UpdateBackbuffer();

	mgVideoDriverAPI* gpu = (mgVideoDriverAPI*)backend->GetVideoDriverAPI();
	m_gui_context = mgCreateContext(gpu, &m_input);
}

ContextImpl::~ContextImpl()
{
	if (m_gui_context)
		mgDestroyContext(m_gui_context);

	if (m_backend)
		m_backend->Release();

	if (m_window)
		m_window->Release();
}

mgf::SystemWindow* ContextImpl::GetWindow()
{
	return m_window;
}

void ContextImpl::OnWindowSize()
{

}
