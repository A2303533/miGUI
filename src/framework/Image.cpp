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
#include "framework/Image.h"

using namespace mgf;

Image::Image()
{

}

Image::~Image()
{
	Free();
}

void Image::Free()
{
	if (m_image)
	{
		if (m_image->data)
			free(m_image->data);
		free(m_image);
		m_image = 0;
	}
}

void Image::Fill(const mgColor& c)
{
	struct rgba
	{
		uint8_t r, g, b, a;
	};
	rgba* ptr = (rgba*)m_image->data;
	for (uint32_t i = 0, sz = m_image->width * m_image->height; i < sz; ++i)
	{
		ptr->a = c.getAsByteAlpha();
		ptr->r = c.getAsByteRed();
		ptr->g = c.getAsByteGreen();
		ptr->b = c.getAsByteBlue();
		++ptr;
	}
}

void Image::Create(uint32_t x, uint32_t y, const mgColor& c)
{
	Free();
	m_image = (mgImage_s*)calloc(1, sizeof(mgImage_s));
	m_image->width = x;
	m_image->height = y;
	m_image->bits = 32;
	m_image->pitch = m_image->width * 4;
	m_image->type = mgImageType_r8g8b8a8;
	m_image->dataSize = m_image->pitch * m_image->height;
	m_image->data = (unsigned char*)malloc(m_image->dataSize);
	Fill(c);
}

uint32_t Image::GetPitch()
{
	if (!m_image)
		return 0;
	return m_image->pitch;
}

uint32_t Image::GetWidth()
{
	if (!m_image)
		return 0;
	return m_image->width;
}

uint32_t Image::GetHeight()
{
	if (!m_image)
		return 0;
	return m_image->height;
}

uint8_t* Image::GetData()
{
	if (!m_image)
		return 0;
	return m_image->data;
}

uint32_t Image::GetDataSize()
{
	if (!m_image)
		return 0;
	return m_image->dataSize;
}
