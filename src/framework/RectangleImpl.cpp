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
#include "framework/Window.h"
#include "framework/WindowImpl.h"
#include "framework/Rectangle.h"
#include "framework/RectangleImpl.h"

using namespace mgf;

RectangleImpl::RectangleImpl(WindowImpl* w)
{
	mgPoint p;
	mgPointSet(&p, 0, 0);
	mgColor c;
	mgColorSet(&c, 1.f, 1.f, 1.f, 1.f);
	m_element = mgCreateRectangle(w->m_window, &p, &p, &c, &c);
	m_elementRectangle = (mgElementRectangle*)m_element->implementation;
}

RectangleImpl::~RectangleImpl()
{
	if (m_element)
		mgDestroyElement(m_element);
}

void RectangleImpl::SetRect(mgRect* r)
{
	assert(m_element);
	m_element->transformLocal.buildArea = *r;
	m_element->transformLocal.clipArea = *r;
	m_element->creationRect = *r;
}

void RectangleImpl::SetRect(int left, int top, int right, int bottom)
{
	assert(m_element);
	m_element->transformLocal.buildArea.left = left;
	m_element->transformLocal.buildArea.top = top;
	m_element->transformLocal.buildArea.right = right;
	m_element->transformLocal.buildArea.bottom = bottom;
	m_element->transformLocal.clipArea.left = left;
	m_element->transformLocal.clipArea.top = top;
	m_element->transformLocal.clipArea.right = right;
	m_element->transformLocal.clipArea.bottom = bottom;
	m_element->creationRect.left = left;
	m_element->creationRect.top = top;
	m_element->creationRect.right = right;
	m_element->creationRect.bottom = bottom;
}


