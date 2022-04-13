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
#include "framework/Icons.h"


using namespace mgf;

Icons::Icons(Backend* b)
	:
	m_backend(b)
{
}

Icons::~Icons()
{
	if (m_texture)
		m_backend->DestroyTexture(m_texture);
}

int Icons::Add(int left, int top, int sizeX, int sizeY)
{
	int id = (int)m_rects.size();

	mgRect r;
	mgRectSet(&r, left, top, sizeX - 1, sizeY - 1);
	m_rects.push_back(r);

	if (m_icons)
		mgDestroyIcons(m_icons);

	int sz = (int)m_rects.size();
	m_icons = mgCreateIcons(m_texture, m_textureSize.x, m_textureSize.y, sz);
	for (int i = 0; i < sz; ++i)
	{
		auto& rct = m_rects[i];
		mgSetIcon(m_icons, i, rct.left, rct.top, rct.right, rct.bottom);
	}

	return id;
}



